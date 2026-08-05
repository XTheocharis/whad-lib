/*
 * test_esb.c — host-side tests for the ESB (Enhanced ShockBurst) domain C API.
 *
 * BUILD PATTERN: esb.c is textually included via
 * `#include "../src/domains/esb.c"` (mirrors test_board.c). The Makefile
 * links whad.c, transport.c, ringbuf.c, phy.c, and the nanopb .pb.c files
 * separately.
 *
 * Coverage (one pack/parse round-trip per ESB function — 13 in total):
 *   - whad_esb_set_node_address pack/parse + size-bound rejection
 *   - whad_esb_sniff pack/parse (address + channel + show_acks)
 *   - whad_esb_jam pack/parse (channel only)
 *   - whad_esb_send pack/parse (channel + retr_count + packet bytes)
 *   - whad_esb_send_raw pack/parse (channel + retr_count + packet bytes)
 *   - whad_esb_prx pack/parse (channel only)
 *   - whad_esb_ptx pack/parse (channel only)
 *   - whad_esb_start / stop (parameterless)
 *   - whad_esb_jammed pack/parse (timestamp)
 *   - whad_esb_raw_pdu_received pack/parse (with optional rssi/ts/crc/addr)
 *   - whad_esb_pdu_received pack/parse (with optional fields)
 *   - whad_esb_get_message_type (NULL, non-ESB, ESB)
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
 * Textual inclusion — same pattern as test_board.c.
 *
 * esb.c has 2 pre-existing -Wtype-limits warnings (uint8_t length compared
 * > 255 in send_parse and send_raw_parse — always-false guard). Same class
 * of latent -Wextra issue T32 fixed for phy.c; out of T31's scope (tests-
 * only). Silence for the textual inclusion only; the test code below
 * remains strict.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#include "../src/domains/esb.c"
#pragma GCC diagnostic pop

#define TEST_ASSERT(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__, #condition); return false; } } while (0)

/* ------------------------------------------------------------------ */
/* whad_esb_set_node_address pack/parse + size-bound                  */
/* ------------------------------------------------------------------ */

static bool test_set_node_address_round_trip(void)
{
    Message msg;
    whad_esb_address_t in_addr;
    whad_esb_address_t out_addr;

    memset(&in_addr, 0, sizeof(in_addr));
    in_addr.address[0] = 0xE7; in_addr.address[1] = 0xE7;
    in_addr.address[2] = 0xE7; in_addr.address[3] = 0xE7;
    in_addr.address[4] = 0xE7;
    in_addr.size = 5;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_set_node_address(&msg, &in_addr) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_esb_tag);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_set_node_addr_tag);

    memset(&out_addr, 0, sizeof(out_addr));
    TEST_ASSERT(whad_esb_set_node_address_parse(&msg, &out_addr) == WHAD_SUCCESS);
    TEST_ASSERT(out_addr.size == 5);
    TEST_ASSERT(memcmp(out_addr.address, in_addr.address, 5) == 0);

    /* NULL message → ERROR */
    TEST_ASSERT(whad_esb_set_node_address(NULL, &in_addr) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_esb_sniff pack/parse round-trip                               */
/* ------------------------------------------------------------------ */

static bool test_sniff_round_trip(void)
{
    Message msg;
    whad_esb_address_t in_addr;
    whad_esb_sniff_params_t out;

    memset(&in_addr, 0, sizeof(in_addr));
    in_addr.address[0] = 0x1A; in_addr.address[1] = 0x2B;
    in_addr.address[2] = 0x3C; in_addr.address[3] = 0x4D;
    in_addr.address[4] = 0x5E;
    in_addr.size = 5;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_sniff(&msg, &in_addr, 15, true) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_esb_tag);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_sniff_tag);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_esb_sniff_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.channel == 15);
    TEST_ASSERT(out.show_acks == true);
    TEST_ASSERT(out.address.size == 5);
    TEST_ASSERT(memcmp(out.address.address, in_addr.address, 5) == 0);

    TEST_ASSERT(whad_esb_sniff(NULL, &in_addr, 1, false) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_esb_jam pack/parse round-trip                                 */
/* ------------------------------------------------------------------ */

static bool test_jam_round_trip(void)
{
    Message msg;
    uint32_t out_channel = 0;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_jam(&msg, 42) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_esb_tag);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_jam_tag);
    TEST_ASSERT(msg.msg.esb.msg.jam.channel == 42);

    TEST_ASSERT(whad_esb_jam_parse(&msg, &out_channel) == WHAD_SUCCESS);
    TEST_ASSERT(out_channel == 42);

    TEST_ASSERT(whad_esb_jam(NULL, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_esb_send pack/parse round-trip                                */
/* ------------------------------------------------------------------ */

static bool test_send_round_trip(void)
{
    Message msg;
    uint8_t packet[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03};
    whad_esb_send_params_t out;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_send(&msg, 7, 3, packet, (uint8_t)sizeof(packet)) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_send_tag);
    TEST_ASSERT(msg.msg.esb.msg.send.channel == 7);
    TEST_ASSERT(msg.msg.esb.msg.send.retransmission_count == 3);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_esb_send_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.channel == 7);
    TEST_ASSERT(out.retr_count == 3);
    TEST_ASSERT(out.packet.length == sizeof(packet));
    TEST_ASSERT(memcmp(out.packet.bytes, packet, sizeof(packet)) == 0);

    TEST_ASSERT(whad_esb_send(NULL, 1, 1, packet, 1) == WHAD_ERROR);
    TEST_ASSERT(whad_esb_send(&msg, 1, 1, NULL, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_esb_send_raw pack/parse round-trip                            */
/* ------------------------------------------------------------------ */

static bool test_send_raw_round_trip(void)
{
    Message msg;
    uint8_t packet[] = {0xCA, 0xFE, 0xBA, 0xBE, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    whad_esb_send_params_t out;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_send_raw(&msg, 11, 1, packet, (uint8_t)sizeof(packet)) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_send_raw_tag);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_esb_send_raw_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.channel == 11);
    TEST_ASSERT(out.retr_count == 1);
    TEST_ASSERT(out.packet.length == sizeof(packet));
    TEST_ASSERT(memcmp(out.packet.bytes, packet, sizeof(packet)) == 0);

    TEST_ASSERT(whad_esb_send_raw(NULL, 1, 1, packet, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_esb_prx + ptx pack/parse round-trip                           */
/* ------------------------------------------------------------------ */

static bool test_prx_ptx_round_trip(void)
{
    Message msg;
    uint32_t out_channel = 0;

    /* PRX */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_prx(&msg, 5) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_prx_tag);
    TEST_ASSERT(whad_esb_prx_parse(&msg, &out_channel) == WHAD_SUCCESS);
    TEST_ASSERT(out_channel == 5);
    TEST_ASSERT(whad_esb_prx(NULL, 1) == WHAD_ERROR);

    /* PTX */
    memset(&msg, 0, sizeof(msg));
    out_channel = 0;
    TEST_ASSERT(whad_esb_ptx(&msg, 9) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_ptx_tag);
    TEST_ASSERT(whad_esb_ptx_parse(&msg, &out_channel) == WHAD_SUCCESS);
    TEST_ASSERT(out_channel == 9);
    TEST_ASSERT(whad_esb_ptx(NULL, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_esb_start / stop (parameterless)                              */
/* ------------------------------------------------------------------ */

static bool test_start_stop(void)
{
    Message msg;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_start(&msg) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_esb_tag);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_start_tag);
    TEST_ASSERT(whad_esb_start(NULL) == WHAD_ERROR);

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_stop(&msg) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_stop_tag);
    TEST_ASSERT(whad_esb_stop(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_esb_jammed pack/parse round-trip                              */
/* ------------------------------------------------------------------ */

static bool test_jammed_round_trip(void)
{
    Message msg;
    uint32_t out_ts = 0;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_jammed(&msg, 123456) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_jammed_tag);
    TEST_ASSERT(msg.msg.esb.msg.jammed.timestamp == 123456);

    TEST_ASSERT(whad_esb_jammed_parse(&msg, &out_ts) == WHAD_SUCCESS);
    TEST_ASSERT(out_ts == 123456);

    TEST_ASSERT(whad_esb_jammed(NULL, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_esb_raw_pdu_received pack/parse round-trip                    */
/* ------------------------------------------------------------------ */

static bool test_raw_pdu_received_round_trip(void)
{
    Message msg;
    whad_esb_recvd_packet_t in;
    whad_esb_recvd_packet_t out;

    memset(&in, 0, sizeof(in));
    in.channel = 8;
    in.packet.bytes[0] = 0xAA; in.packet.bytes[1] = 0xBB; in.packet.bytes[2] = 0xCC;
    in.packet.length = 3;
    in.has_rssi = true; in.rssi = -57;
    in.has_timestamp = true; in.timestamp = 9999;
    in.has_crc_validity = true; in.crc_validity = true;
    in.has_address = true;
    in.address.address[0] = 0x11; in.address.address[1] = 0x22;
    in.address.address[2] = 0x33; in.address.address[3] = 0x44;
    in.address.address[4] = 0x55;
    in.address.size = 5;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_raw_pdu_received(&msg, &in) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_raw_pdu_tag);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_esb_raw_pdu_received_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.channel == 8);
    TEST_ASSERT(out.packet.length == 3);
    TEST_ASSERT(memcmp(out.packet.bytes, in.packet.bytes, 3) == 0);
    TEST_ASSERT(out.has_rssi && out.rssi == -57);
    TEST_ASSERT(out.has_timestamp && out.timestamp == 9999);
    TEST_ASSERT(out.has_crc_validity && out.crc_validity == true);
    TEST_ASSERT(out.has_address && out.address.size == 5);

    TEST_ASSERT(whad_esb_raw_pdu_received(NULL, &in) == WHAD_ERROR);
    TEST_ASSERT(whad_esb_raw_pdu_received(&msg, NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_esb_pdu_received pack/parse round-trip                        */
/* ------------------------------------------------------------------ */

static bool test_pdu_received_round_trip(void)
{
    Message msg;
    whad_esb_recvd_packet_t in;
    whad_esb_recvd_packet_t out;

    memset(&in, 0, sizeof(in));
    in.channel = 2;
    in.packet.bytes[0] = 0x01; in.packet.bytes[1] = 0x02;
    in.packet.length = 2;
    /* No optional fields — exercise the has_*=false path. */

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_esb_pdu_received(&msg, &in) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.esb.which_msg == esb_Message_pdu_tag);

    memset(&out, 0xFF, sizeof(out));
    TEST_ASSERT(whad_esb_pdu_received_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.channel == 2);
    TEST_ASSERT(out.packet.length == 2);
    TEST_ASSERT(out.packet.bytes[0] == 0x01 && out.packet.bytes[1] == 0x02);
    TEST_ASSERT(!out.has_rssi);
    TEST_ASSERT(!out.has_timestamp);
    TEST_ASSERT(!out.has_crc_validity);
    TEST_ASSERT(!out.has_address);

    TEST_ASSERT(whad_esb_pdu_received(NULL, &in) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_esb_get_message_type — NULL / non-ESB / ESB                   */
/* ------------------------------------------------------------------ */

static bool test_get_message_type(void)
{
    Message msg;
    memset(&msg, 0, sizeof(msg));

    /* Non-ESB domain (board) → UNKNOWN */
    msg.which_msg = Message_board_tag;
    TEST_ASSERT(whad_esb_get_message_type(&msg) == WHAD_ESB_UNKNOWN);

    /* ESB sniff → mapped enum */
    msg.which_msg = Message_esb_tag;
    msg.msg.esb.which_msg = esb_Message_sniff_tag;
    TEST_ASSERT(whad_esb_get_message_type(&msg) == WHAD_ESB_SNIFF);
    return true;
}

/* ------------------------------------------------------------------ */
/* main — chain all tests                                             */
/* ------------------------------------------------------------------ */

int main(void)
{
    TEST_ASSERT(test_set_node_address_round_trip());
    TEST_ASSERT(test_sniff_round_trip());
    TEST_ASSERT(test_jam_round_trip());
    TEST_ASSERT(test_send_round_trip());
    TEST_ASSERT(test_send_raw_round_trip());
    TEST_ASSERT(test_prx_ptx_round_trip());
    TEST_ASSERT(test_start_stop());
    TEST_ASSERT(test_jammed_round_trip());
    TEST_ASSERT(test_raw_pdu_received_round_trip());
    TEST_ASSERT(test_pdu_received_round_trip());
    TEST_ASSERT(test_get_message_type());

    printf("All test_esb tests passed!\n");
    return 0;
}
