/*
 * test_ble.c — host-side tests for the BLE domain C API (whad-lib).
 *
 * BUILD PATTERN: ble.c is textually included via
 * `#include "../src/domains/ble.c"` so the test has direct access to every
 * function (mirrors test_board.c). The Makefile links whad.c, transport.c,
 * ringbuf.c, phy.c and the nanopb .pb.c files separately.
 *
 * Coverage:
 *   - whad_ble_set_bdaddress pack/parse round-trip
 *   - whad_ble_set_adv_data + whad_ble_set_adv_data_parse callback stubs
 *     (Wave 1 T2: both return WHAD_ERROR until nanopb callback helpers exist)
 *   - whad_ble_scan_mode pack/parse round-trip
 *   - whad_ble_adv_mode pack/parse round-trip (adv_data + scanrsp)
 *   - whad_ble_peripheral_mode pack/parse round-trip
 *   - whad_ble_central_mode / start / stop (parameterless setters)
 *   - whad_ble_disconnect pack/parse round-trip
 *   - whad_ble_notify_disconnected pack/parse round-trip
 *   - whad_ble_pdu pack + parse round-trip (link-layer data PDU)
 *   - whad_ble_jam_adv (parameterless)
 *   - whad_ble_jam_adv_channel pack/parse round-trip
 *   - whad_ble_jam_active_conn pack/parse round-trip
 *   - whad_ble_hijack_master / hijack_slave / hijack_both pack/parse
 *   - whad_ble_get_message_type (NULL, non-BLE, BLE)
 *   - NULL pointer handling for representative pack and parse paths
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
 * Textual inclusion — same pattern as test_board.c. ble.c pulls in <whad.h>
 * and <domains/ble.h> via the -I include paths.
 *
 * ble.c has 5 pre-existing -Wextra warnings (1 unused-parameter in
 * prepare_sequence_on_recv; 4 enum-conversion in prepare_sequence_*_parse
 * and adv_pdu_parse from parallel-enum C wrappers — same class of bug T32
 * fixed for phy.c). They are out of T31's scope (tests-only); silence them
 * for the textual inclusion only. The test code below remains strict.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wenum-conversion"
#include "../src/domains/ble.c"
#pragma GCC diagnostic pop

#define TEST_ASSERT(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__, #condition); return false; } } while (0)

/* ------------------------------------------------------------------ */
/* whad_ble_set_bdaddress pack/parse round-trip                       */
/* ------------------------------------------------------------------ */

static bool test_set_bdaddress_round_trip(void)
{
    Message msg;
    uint8_t in_addr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    uint8_t out_addr[6];
    whad_ble_addrtype_t out_type;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_set_bdaddress(&msg, BLE_ADDR_RANDOM, in_addr) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_ble_tag);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_set_bd_addr_tag);

    TEST_ASSERT(whad_ble_set_bdaddress_parse(&msg, &out_type, out_addr) == WHAD_SUCCESS);
    TEST_ASSERT(out_type == BLE_ADDR_RANDOM);
    TEST_ASSERT(memcmp(out_addr, in_addr, 6) == 0);

    /* NULL message → ERROR */
    TEST_ASSERT(whad_ble_set_bdaddress(NULL, BLE_ADDR_PUBLIC, in_addr) == WHAD_ERROR);
    TEST_ASSERT(whad_ble_set_bdaddress_parse(NULL, &out_type, out_addr) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* SetAdvData callback stubs (Wave 1 T2 — C6/C7 un-stubbed to ERROR)  */
/* ------------------------------------------------------------------ */

static bool test_set_adv_data_callback_stubs_reject(void)
{
    Message msg;
    uint8_t adv[4] = {1, 2, 3, 4};
    uint8_t scanrsp[4] = {5, 6, 7, 8};
    uint8_t out_adv[8] = {0};
    uint8_t out_scanrsp[8] = {0};
    int out_adv_len = 0;
    int out_scanrsp_len = 0;

    memset(&msg, 0, sizeof(msg));

    /* SetAdvDataCmd.scan_data/.scanrsp_data are nanopb CALLBACK fields;
     * pack rejects until an encode-callback helper exists. */
    TEST_ASSERT(whad_ble_set_adv_data(&msg, adv, 4, scanrsp, 4) == WHAD_ERROR);
    /* Parse also rejects — bytes arrive via decode callback, not in struct. */
    TEST_ASSERT(whad_ble_set_adv_data_parse(
        &msg, out_adv, &out_adv_len, out_scanrsp, &out_scanrsp_len) == WHAD_ERROR);

    /* NULL inputs are also rejected. */
    TEST_ASSERT(whad_ble_set_adv_data(NULL, adv, 4, scanrsp, 4) == WHAD_ERROR);
    TEST_ASSERT(whad_ble_set_adv_data(NULL, NULL, 4, scanrsp, 4) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_scan_mode pack/parse round-trip                           */
/* ------------------------------------------------------------------ */

static bool test_scan_mode_round_trip(void)
{
    Message msg;
    bool out_active;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_scan_mode(&msg, true) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_ble_tag);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_scan_mode_tag);
    TEST_ASSERT(msg.msg.ble.msg.scan_mode.active_scan == true);

    TEST_ASSERT(whad_ble_scan_mode_parse(&msg, &out_active) == WHAD_SUCCESS);
    TEST_ASSERT(out_active == true);

    /* NULL handling */
    TEST_ASSERT(whad_ble_scan_mode(NULL, true) == WHAD_ERROR);
    TEST_ASSERT(whad_ble_scan_mode_parse(&msg, NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_adv_mode pack/parse round-trip                            */
/* ------------------------------------------------------------------ */

static bool test_adv_mode_round_trip(void)
{
    Message msg;
    uint8_t adv[] = {0x02, 0x01, 0x06, 0x05, 0x09, 'C', 'L', 'U', 'E'};
    uint8_t scanrsp[] = {0x03, 0x19, 0x80, 0x01};
    whad_ble_adv_mode_params_t out;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_adv_mode(&msg, adv, (int)sizeof(adv), scanrsp, (int)sizeof(scanrsp)) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_ble_tag);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_adv_mode_tag);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_ble_adv_mode_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.adv_data_length == (int)sizeof(adv));
    TEST_ASSERT(out.scanrsp_data_length == (int)sizeof(scanrsp));
    TEST_ASSERT(memcmp(out.adv_data, adv, sizeof(adv)) == 0);
    TEST_ASSERT(memcmp(out.scanrsp_data, scanrsp, sizeof(scanrsp)) == 0);

    /* NULL message → ERROR */
    TEST_ASSERT(whad_ble_adv_mode(NULL, adv, 1, scanrsp, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_peripheral_mode pack/parse round-trip                     */
/* ------------------------------------------------------------------ */

static bool test_periph_mode_round_trip(void)
{
    Message msg;
    uint8_t adv[] = {0x02, 0x01, 0x05};
    uint8_t scanrsp[] = {0x03, 0x19, 0x00, 0x80};
    whad_ble_adv_mode_params_t out;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_peripheral_mode(&msg, adv, (int)sizeof(adv), scanrsp, (int)sizeof(scanrsp)) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_ble_tag);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_periph_mode_tag);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_ble_peripheral_mode_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.adv_data_length == (int)sizeof(adv));
    TEST_ASSERT(out.scanrsp_data_length == (int)sizeof(scanrsp));
    return true;
}

/* ------------------------------------------------------------------ */
/* Parameterless mode setters: central / start / stop                 */
/* ------------------------------------------------------------------ */

static bool test_parameterless_mode_setters(void)
{
    Message msg;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_central_mode(&msg) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_ble_tag);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_central_mode_tag);

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_start(&msg) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_start_tag);

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_stop(&msg) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_stop_tag);

    /* NULL → ERROR for each */
    TEST_ASSERT(whad_ble_central_mode(NULL) == WHAD_ERROR);
    TEST_ASSERT(whad_ble_start(NULL) == WHAD_ERROR);
    TEST_ASSERT(whad_ble_stop(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_disconnect pack/parse round-trip                          */
/* ------------------------------------------------------------------ */

static bool test_disconnect_round_trip(void)
{
    Message msg;
    uint32_t out_handle = 0;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_disconnect(&msg, 0xCAFE) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_ble_tag);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_disconnect_tag);
    TEST_ASSERT(msg.msg.ble.msg.disconnect.conn_handle == 0xCAFE);

    TEST_ASSERT(whad_ble_disconnect_parse(&msg, &out_handle) == WHAD_SUCCESS);
    TEST_ASSERT(out_handle == 0xCAFE);

    TEST_ASSERT(whad_ble_disconnect(NULL, 1) == WHAD_ERROR);
    TEST_ASSERT(whad_ble_disconnect_parse(&msg, NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_notify_disconnected pack/parse round-trip                 */
/* ------------------------------------------------------------------ */

static bool test_notify_disconnected_round_trip(void)
{
    Message msg;
    whad_ble_disconnected_params_t out;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_notify_disconnected(&msg, 0x1234, 0x3E) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_disconnected_tag);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_ble_notify_disconnected_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.conn_handle == 0x1234);
    TEST_ASSERT(out.reason == 0x3E);

    TEST_ASSERT(whad_ble_notify_disconnected(NULL, 1, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_pdu pack + parse round-trip                               */
/* ------------------------------------------------------------------ */

static bool test_pdu_round_trip(void)
{
    Message msg;
    uint8_t pdu[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    whad_ble_pdu_t out;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_pdu(&msg, pdu, (int)sizeof(pdu), BLE_MASTER_TO_SLAVE,
                             0x0001, true, false) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_ble_tag);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_pdu_tag);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_ble_pdu_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.conn_handle == 0x0001);
    TEST_ASSERT(out.processed == true);
    TEST_ASSERT(out.decrypted == false);
    TEST_ASSERT(out.pdu_length == sizeof(pdu));
    TEST_ASSERT(memcmp(out.p_pdu, pdu, sizeof(pdu)) == 0);

    TEST_ASSERT(whad_ble_pdu(NULL, pdu, 1, BLE_MASTER_TO_SLAVE, 1, false, false) == WHAD_ERROR);
    TEST_ASSERT(whad_ble_pdu(&msg, NULL, 1, BLE_MASTER_TO_SLAVE, 1, false, false) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_jam_adv (parameterless)                                   */
/* ------------------------------------------------------------------ */

static bool test_jam_adv(void)
{
    Message msg;
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_jam_adv(&msg) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_ble_tag);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_jam_adv_tag);
    TEST_ASSERT(whad_ble_jam_adv(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_jam_adv_channel pack/parse round-trip                     */
/* ------------------------------------------------------------------ */

static bool test_jam_adv_channel_round_trip(void)
{
    Message msg;
    uint32_t out_channel = 0;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_jam_adv_channel(&msg, 37) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_jam_adv_chan_tag);
    TEST_ASSERT(msg.msg.ble.msg.jam_adv_chan.channel == 37);

    TEST_ASSERT(whad_ble_jam_adv_channel_parse(&msg, &out_channel) == WHAD_SUCCESS);
    TEST_ASSERT(out_channel == 37);

    TEST_ASSERT(whad_ble_jam_adv_channel(NULL, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_jam_active_conn pack/parse round-trip                     */
/* ------------------------------------------------------------------ */

static bool test_jam_active_conn_round_trip(void)
{
    Message msg;
    uint32_t out_aa = 0;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_jam_active_conn(&msg, 0xDEADBEEF) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_jam_conn_tag);

    TEST_ASSERT(whad_ble_jam_active_conn_parse(&msg, &out_aa) == WHAD_SUCCESS);
    TEST_ASSERT(out_aa == 0xDEADBEEF);

    TEST_ASSERT(whad_ble_jam_active_conn(NULL, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_hijack_master / slave / both pack/parse round-trip        */
/* ------------------------------------------------------------------ */

static bool test_hijack_round_trip(void)
{
    Message msg;
    uint32_t out_aa = 0;

    /* master */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_hijack_master(&msg, 0xAAAA1111) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_hijack_master_tag);
    TEST_ASSERT(whad_ble_hijack_master_parse(&msg, &out_aa) == WHAD_SUCCESS);
    TEST_ASSERT(out_aa == 0xAAAA1111);
    TEST_ASSERT(whad_ble_hijack_master(NULL, 1) == WHAD_ERROR);

    /* slave */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_hijack_slave(&msg, 0xBBBB2222) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_hijack_slave_tag);
    TEST_ASSERT(whad_ble_hijack_slave_parse(&msg, &out_aa) == WHAD_SUCCESS);
    TEST_ASSERT(out_aa == 0xBBBB2222);
    TEST_ASSERT(whad_ble_hijack_slave(NULL, 1) == WHAD_ERROR);

    /* both */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_hijack_both(&msg, 0xCCCC3333) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_hijack_both_tag);
    TEST_ASSERT(whad_ble_hijack_both_parse(&msg, &out_aa) == WHAD_SUCCESS);
    TEST_ASSERT(out_aa == 0xCCCC3333);
    TEST_ASSERT(whad_ble_hijack_both(NULL, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_connect_to pack/parse round-trip                          */
/* ------------------------------------------------------------------ */

static bool test_connect_to_round_trip(void)
{
    Message msg;
    uint8_t bdaddr[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint8_t channelmap[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0x1F};
    whad_ble_connect_params_t out;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_ble_connect_to(&msg, bdaddr, BLE_ADDR_PUBLIC,
                                    0x11223344, channelmap, 6, 6, 0x555555) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.ble.which_msg == ble_Message_connect_tag);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_ble_connect_to_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(memcmp(out.bdaddr, bdaddr, 6) == 0);
    TEST_ASSERT(out.addr_type == BLE_ADDR_PUBLIC);
    TEST_ASSERT(out.access_address == 0x11223344);
    TEST_ASSERT(out.hop_interval == 6);
    TEST_ASSERT(out.hop_increment == 6);
    TEST_ASSERT(out.crc_init == 0x555555);

    /* NULL bdaddr → ERROR */
    TEST_ASSERT(whad_ble_connect_to(NULL, bdaddr, BLE_ADDR_PUBLIC, 1, channelmap, 1, 1, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_ble_get_message_type — NULL / non-BLE / BLE                   */
/* ------------------------------------------------------------------ */

static bool test_get_message_type(void)
{
    Message msg;
    memset(&msg, 0, sizeof(msg));

    /* Non-BLE domain (esb) → UNKNOWN */
    msg.which_msg = Message_esb_tag;
    TEST_ASSERT(whad_ble_get_message_type(&msg) == WHAD_BLE_UNKNOWN);

    /* BLE disconnect → mapped enum */
    msg.which_msg = Message_ble_tag;
    msg.msg.ble.which_msg = ble_Message_disconnect_tag;
    TEST_ASSERT(whad_ble_get_message_type(&msg) == WHAD_BLE_DISCONNECT);
    return true;
}

/* ------------------------------------------------------------------ */
/* main — chain all tests                                             */
/* ------------------------------------------------------------------ */

int main(void)
{
    TEST_ASSERT(test_set_bdaddress_round_trip());
    TEST_ASSERT(test_set_adv_data_callback_stubs_reject());
    TEST_ASSERT(test_scan_mode_round_trip());
    TEST_ASSERT(test_adv_mode_round_trip());
    TEST_ASSERT(test_periph_mode_round_trip());
    TEST_ASSERT(test_parameterless_mode_setters());
    TEST_ASSERT(test_disconnect_round_trip());
    TEST_ASSERT(test_notify_disconnected_round_trip());
    TEST_ASSERT(test_pdu_round_trip());
    TEST_ASSERT(test_jam_adv());
    TEST_ASSERT(test_jam_adv_channel_round_trip());
    TEST_ASSERT(test_jam_active_conn_round_trip());
    TEST_ASSERT(test_hijack_round_trip());
    TEST_ASSERT(test_connect_to_round_trip());
    TEST_ASSERT(test_get_message_type());

    printf("All test_ble tests passed!\n");
    return 0;
}
