/*
 * test_phy.c — host-side tests for the PHY (generic physical layer) domain C API.
 *
 * BUILD PATTERN: phy.c is textually included via
 * `#include "../src/domains/phy.c"`. The Makefile MUST NOT also list phy.c
 * as a separate source file (would cause duplicate-symbol errors — same
 * pattern as test_board.c with board.c). whad.c, transport.c, ringbuf.c,
 * and the nanopb .pb.c files are linked separately.
 *
 * Coverage:
 *   - 8 modulation setters: ask, fsk, 4fsk, gfsk, bpsk, qpsk, msk, lora
 *   - whad_phy_set_freq / set_datarate / set_endianness / set_tx_power /
 *     set_packet_size round-trips
 *   - whad_phy_set_sync_word round-trip (with >10-byte length cap)
 *   - whad_phy_send round-trip (packet bytes + >255-byte length cap)
 *   - whad_phy_send_raw_iq callback stub (Wave 1 T2 — returns WHAD_ERROR)
 *   - whad_phy_sniff_mode / jam_mode / monitor_mode
 *   - whad_phy_start / stop (parameterless)
 *   - whad_phy_jammed round-trip (timestamp second/microsecond math)
 *   - whad_phy_sched_packet_sent round-trip (packet id)
 *   - whad_phy_packet_scheduled round-trip (id + optional full flag)
 *   - whad_phy_get_message_type (NULL, non-PHY, PHY)
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
 * Textual inclusion — same pattern as test_board.c with board.c. phy.c
 * pulls in <whad.h> and <domains/phy.h> via the -I include paths.
 */
#include "../src/domains/phy.c"

#define TEST_ASSERT(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__, #condition); return false; } } while (0)

/* ------------------------------------------------------------------ */
/* Modulation setters round-trip                                      */
/* ------------------------------------------------------------------ */

static bool test_modulation_setters(void)
{
    Message msg;
    bool out_ook = false;
    uint32_t out_dev = 0;
    bool out_offset = false;
    whad_phy_lora_params_t out_lora;

    /* ASK */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_set_ask_mod(&msg, true) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_phy_tag);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_mod_ask_tag);
    TEST_ASSERT(whad_phy_set_ask_mod_parse(&msg, &out_ook) == WHAD_SUCCESS);
    TEST_ASSERT(out_ook == true);
    TEST_ASSERT(whad_phy_set_ask_mod(NULL, true) == WHAD_ERROR);

    /* FSK */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_set_fsk_mod(&msg, 250000) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_mod_fsk_tag);
    TEST_ASSERT(whad_phy_set_fsk_mod_parse(&msg, &out_dev) == WHAD_SUCCESS);
    TEST_ASSERT(out_dev == 250000);

    /* 4FSK */
    memset(&msg, 0, sizeof(msg));
    out_dev = 0;
    TEST_ASSERT(whad_phy_set_4fsk_mod(&msg, 50000) == WHAD_SUCCESS);
    TEST_ASSERT(whad_phy_set_4fsk_mod_parse(&msg, &out_dev) == WHAD_SUCCESS);
    TEST_ASSERT(out_dev == 50000);

    /* GFSK */
    memset(&msg, 0, sizeof(msg));
    out_dev = 0;
    TEST_ASSERT(whad_phy_set_gfsk_mod(&msg, 350000) == WHAD_SUCCESS);
    TEST_ASSERT(whad_phy_set_gfsk_mod_parse(&msg, &out_dev) == WHAD_SUCCESS);
    TEST_ASSERT(out_dev == 350000);

    /* BPSK */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_set_bpsk_mod(&msg) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_mod_bpsk_tag);
    TEST_ASSERT(whad_phy_set_bpsk_mod(NULL) == WHAD_ERROR);

    /* QPSK */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_set_qpsk_mod(&msg, true) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_mod_qpsk_tag);
    TEST_ASSERT(whad_phy_set_qpsk_mod_parse(&msg, &out_offset) == WHAD_SUCCESS);
    TEST_ASSERT(out_offset == true);

    /* MSK */
    memset(&msg, 0, sizeof(msg));
    out_dev = 0;
    TEST_ASSERT(whad_phy_set_msk_mod(&msg, 175000) == WHAD_SUCCESS);
    TEST_ASSERT(whad_phy_set_msk_mod_parse(&msg, &out_dev) == WHAD_SUCCESS);
    TEST_ASSERT(out_dev == 175000);

    /* LoRa — full parameter struct round-trip */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_set_lora_mod(&msg, 125000, PHY_LORA_SF7, PHY_LORA_CR45,
                                      8, true, false, true) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_mod_lora_tag);
    memset(&out_lora, 0, sizeof(out_lora));
    TEST_ASSERT(whad_phy_set_lora_mod_parse(&msg, &out_lora) == WHAD_SUCCESS);
    TEST_ASSERT(out_lora.bandwidth == 125000);
    TEST_ASSERT(out_lora.sf == PHY_LORA_SF7);
    TEST_ASSERT(out_lora.cr == PHY_LORA_CR45);
    TEST_ASSERT(out_lora.preamble_length == 8);
    TEST_ASSERT(out_lora.enable_crc == true);
    TEST_ASSERT(out_lora.explicit_mode == false);
    TEST_ASSERT(out_lora.invert_iq == true);
    return true;
}

/* ------------------------------------------------------------------ */
/* Signal characteristics: freq / datarate / endianness / tx_power    */
/* ------------------------------------------------------------------ */

static bool test_signal_characteristics_round_trip(void)
{
    Message msg;
    uint32_t out_u32 = 0;
    whad_phy_endian_t out_endian = PHY_BIG_ENDIAN;
    whad_phy_txpower_t out_power = PHY_TXPOWER_LOW;

    /* freq */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_set_freq(&msg, 2402000000u) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_set_freq_tag);
    TEST_ASSERT(whad_phy_set_freq_parse(&msg, &out_u32) == WHAD_SUCCESS);
    TEST_ASSERT(out_u32 == 2402000000u);
    TEST_ASSERT(whad_phy_set_freq(NULL, 1) == WHAD_ERROR);

    /* datarate */
    memset(&msg, 0, sizeof(msg));
    out_u32 = 0;
    TEST_ASSERT(whad_phy_set_datarate(&msg, 2000000) == WHAD_SUCCESS);
    TEST_ASSERT(whad_phy_set_datarate_parse(&msg, &out_u32) == WHAD_SUCCESS);
    TEST_ASSERT(out_u32 == 2000000);

    /* endianness */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_set_endianness(&msg, PHY_LITTLE_ENDIAN) == WHAD_SUCCESS);
    TEST_ASSERT(whad_phy_set_endianness_parse(&msg, &out_endian) == WHAD_SUCCESS);
    TEST_ASSERT(out_endian == PHY_LITTLE_ENDIAN);

    /* tx_power */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_set_tx_power(&msg, PHY_TXPOWER_HIGH) == WHAD_SUCCESS);
    TEST_ASSERT(whad_phy_set_tx_power_parse(&msg, &out_power) == WHAD_SUCCESS);
    TEST_ASSERT(out_power == PHY_TXPOWER_HIGH);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_phy_set_packet_size round-trip                                */
/* ------------------------------------------------------------------ */

static bool test_set_packet_size_round_trip(void)
{
    Message msg;
    uint32_t out_size = 0;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_set_packet_size(&msg, 32) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_packet_size_tag);
    TEST_ASSERT(whad_phy_set_packet_size_parse(&msg, &out_size) == WHAD_SUCCESS);
    TEST_ASSERT(out_size == 32);

    TEST_ASSERT(whad_phy_set_packet_size(NULL, 1) == WHAD_ERROR);
    TEST_ASSERT(whad_phy_set_packet_size_parse(&msg, NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_phy_set_sync_word round-trip + length cap                     */
/* ------------------------------------------------------------------ */

static bool test_set_sync_word_round_trip(void)
{
    Message msg;
    uint8_t sync[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE};
    uint8_t too_long[11] = {0};
    whad_phy_syncword_t out;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_set_sync_word(&msg, sync, (int)sizeof(sync)) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_sync_word_tag);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_phy_set_sync_word_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.length == (int)sizeof(sync));
    TEST_ASSERT(memcmp(out.syncword, sync, sizeof(sync)) == 0);

    /* Length cap: 11 bytes > 10 → ERROR */
    TEST_ASSERT(whad_phy_set_sync_word(&msg, too_long, 11) == WHAD_ERROR);

    TEST_ASSERT(whad_phy_set_sync_word(NULL, sync, 1) == WHAD_ERROR);
    TEST_ASSERT(whad_phy_set_sync_word(&msg, NULL, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_phy_send round-trip + length cap                              */
/* ------------------------------------------------------------------ */

static bool test_send_round_trip(void)
{
    Message msg;
    uint8_t packet[] = {0xCA, 0xFE, 0xBA, 0xBE, 0x12, 0x34, 0x56, 0x78, 0x9A};
    whad_phy_packet_t out;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_send(&msg, packet, (int)sizeof(packet)) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_phy_tag);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_send_tag);

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_phy_send_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.length == (int)sizeof(packet));
    TEST_ASSERT(memcmp(out.payload, packet, sizeof(packet)) == 0);

    /* NULL → ERROR */
    TEST_ASSERT(whad_phy_send(NULL, packet, 1) == WHAD_ERROR);
    TEST_ASSERT(whad_phy_send(&msg, NULL, 1) == WHAD_ERROR);

    /* Length > 255 → ERROR (pack rejects; parse sets length cap to 256) */
    TEST_ASSERT(whad_phy_send(&msg, packet, 256) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_phy_send_raw_iq callback stub (Wave 1 T2 — C5)                */
/* ------------------------------------------------------------------ */

static bool test_send_raw_iq_callback_stub_rejects(void)
{
    Message msg;
    uint8_t iq[] = {0x11, 0x22, 0x33, 0x44};

    memset(&msg, 0, sizeof(msg));
    /* SendRawCmd.iq is a nanopb CALLBACK field; pack rejects until an
     * encode-callback helper exists. Wave 1 T2 set this to WHAD_ERROR. */
    TEST_ASSERT(whad_phy_send_raw_iq(&msg, iq, (int)sizeof(iq)) == WHAD_ERROR);

    /* NULL inputs also rejected */
    TEST_ASSERT(whad_phy_send_raw_iq(NULL, iq, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* Mode setters: sniff / jam / monitor                                */
/* ------------------------------------------------------------------ */

static bool test_mode_setters(void)
{
    Message msg;
    bool out_iq = false;
    whad_phy_jam_mode_t out_mode = PHY_JAM_MODE_CONTINUOUS;

    /* sniff_mode */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_sniff_mode(&msg, true) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_sniff_tag);
    TEST_ASSERT(whad_phy_sniff_mode_parse(&msg, &out_iq) == WHAD_SUCCESS);
    TEST_ASSERT(out_iq == true);

    /* jam_mode */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_jam_mode(&msg, PHY_JAM_MODE_REACTIVE) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_jam_tag);
    TEST_ASSERT(whad_phy_jam_mode_parse(&msg, &out_mode) == WHAD_SUCCESS);
    TEST_ASSERT(out_mode == PHY_JAM_MODE_REACTIVE);

    /* monitor_mode — parameterless */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_monitor_mode(&msg) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_monitor_tag);
    TEST_ASSERT(whad_phy_monitor_mode(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_phy_start / stop (parameterless)                              */
/* ------------------------------------------------------------------ */

static bool test_start_stop(void)
{
    Message msg;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_start(&msg) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_phy_tag);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_start_tag);
    TEST_ASSERT(whad_phy_start(NULL) == WHAD_ERROR);

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_stop(&msg) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_stop_tag);
    TEST_ASSERT(whad_phy_stop(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_phy_jammed round-trip — second/microsecond math               */
/* ------------------------------------------------------------------ */

static bool test_jammed_round_trip(void)
{
    Message msg;
    whad_phy_timestamp_t out;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_jammed(&msg, 12, 345678) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_jammed_tag);
    /* Internal field stores ts_sec*1e6 + ts_usec */
    TEST_ASSERT(msg.msg.phy.msg.jammed.timestamp == (12u * 1000000u + 345678u));

    memset(&out, 0, sizeof(out));
    TEST_ASSERT(whad_phy_jammed_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.ts_sec == 12);
    TEST_ASSERT(out.ts_usec == 345678);

    /* Zero timestamp path */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_jammed(&msg, 0, 0) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.msg.jammed.timestamp == 0);

    TEST_ASSERT(whad_phy_jammed(NULL, 1, 1) == WHAD_ERROR);
    TEST_ASSERT(whad_phy_jammed_parse(&msg, NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_phy_sched_packet_sent round-trip                              */
/* ------------------------------------------------------------------ */

static bool test_sched_packet_sent_round_trip(void)
{
    Message msg;
    uint32_t out_id = 0;

    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_sched_packet_sent(&msg, 7) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_sched_pkt_sent_tag);

    TEST_ASSERT(whad_phy_sched_packet_sent_parse(&msg, &out_id) == WHAD_SUCCESS);
    TEST_ASSERT(out_id == 7);

    TEST_ASSERT(whad_phy_sched_packet_sent(NULL, 1) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_phy_packet_scheduled round-trip — id + optional full flag     */
/* ------------------------------------------------------------------ */

static bool test_packet_scheduled_round_trip(void)
{
    Message msg;
    whad_phy_scheduled_packet_t out;

    /* With full=true */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_packet_scheduled(&msg, 3, true) == WHAD_SUCCESS);
    TEST_ASSERT(msg.msg.phy.which_msg == phy_Message_sched_pkt_rsp_tag);
    memset(&out, 0xFF, sizeof(out));
    TEST_ASSERT(whad_phy_packet_scheduled_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.id == 3);
    TEST_ASSERT(out.full == true);

    /* With full=false (omitted) */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_phy_packet_scheduled(&msg, 9, false) == WHAD_SUCCESS);
    memset(&out, 0xFF, sizeof(out));
    TEST_ASSERT(whad_phy_packet_scheduled_parse(&msg, &out) == WHAD_SUCCESS);
    TEST_ASSERT(out.id == 9);
    TEST_ASSERT(out.full == false);

    TEST_ASSERT(whad_phy_packet_scheduled(NULL, 1, false) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_phy_get_message_type — NULL / non-PHY / PHY                   */
/* ------------------------------------------------------------------ */

static bool test_get_message_type(void)
{
    Message msg;
    memset(&msg, 0, sizeof(msg));

    /* Non-PHY domain (board) → UNKNOWN */
    msg.which_msg = Message_board_tag;
    TEST_ASSERT(whad_phy_get_message_type(&msg) == WHAD_PHY_UNKNOWN);

    /* PHY set_freq → mapped enum */
    msg.which_msg = Message_phy_tag;
    msg.msg.phy.which_msg = phy_Message_set_freq_tag;
    TEST_ASSERT(whad_phy_get_message_type(&msg) == WHAD_PHY_SET_FREQ);
    return true;
}

/* ------------------------------------------------------------------ */
/* main — chain all tests                                             */
/* ------------------------------------------------------------------ */

int main(void)
{
    TEST_ASSERT(test_modulation_setters());
    TEST_ASSERT(test_signal_characteristics_round_trip());
    TEST_ASSERT(test_set_packet_size_round_trip());
    TEST_ASSERT(test_set_sync_word_round_trip());
    TEST_ASSERT(test_send_round_trip());
    TEST_ASSERT(test_send_raw_iq_callback_stub_rejects());
    TEST_ASSERT(test_mode_setters());
    TEST_ASSERT(test_start_stop());
    TEST_ASSERT(test_jammed_round_trip());
    TEST_ASSERT(test_sched_packet_sent_round_trip());
    TEST_ASSERT(test_packet_scheduled_round_trip());
    TEST_ASSERT(test_get_message_type());

    printf("All test_phy tests passed!\n");
    return 0;
}
