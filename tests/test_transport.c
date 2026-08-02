#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "transport.h"

#define TEST_ASSERT(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__, #condition); return false; } } while (0)

static uint8_t sent_bytes[4096];
static int sent_size;

static void capture_send(uint8_t *p_buffer, int size)
{
    memcpy(&sent_bytes[sent_size], p_buffer, (size_t)size);
    sent_size += size;
}

static void reset_transport(int max_tx_size, whad_transport_data_send_buffer_cb_t callback)
{
    whad_transport_cfg_t config;

    memset(sent_bytes, 0, sizeof(sent_bytes));
    sent_size = 0;
    config.max_txbuf_size = max_tx_size;
    config.pfn_data_send_buffer = callback;
    whad_transport_init(&config);
}

static void drain_transport(void)
{
    while (whad_transport_send_pending() == WHAD_SUCCESS)
    {
        whad_transport_data_sent();
    }
}

static bool test_zero_size_frame(void)
{
    reset_transport(WHAD_RINGBUF_CAPACITY, capture_send);

    TEST_ASSERT(whad_transport_send_message(NULL, 0) == WHAD_SUCCESS);
    TEST_ASSERT(whad_transport_get_txbuf_size() == WHAD_TRANSPORT_FRAME_HEADER_SIZE);
    drain_transport();
    TEST_ASSERT(sent_size == WHAD_TRANSPORT_FRAME_HEADER_SIZE);
    TEST_ASSERT(sent_bytes[0] == 0xAC);
    TEST_ASSERT(sent_bytes[1] == 0xBE);
    TEST_ASSERT(sent_bytes[2] == 0x00);
    TEST_ASSERT(sent_bytes[3] == 0x00);

    return true;
}

static bool test_limit_frame(void)
{
    uint8_t payload[WHAD_MAX_ENCODED_MESSAGE_SIZE];
    int i;

    reset_transport(WHAD_RINGBUF_CAPACITY, capture_send);
    for (i=0; i<WHAD_MAX_ENCODED_MESSAGE_SIZE; i++)
    {
        payload[i] = (uint8_t)i;
    }

    TEST_ASSERT(whad_transport_send_message(payload, WHAD_MAX_ENCODED_MESSAGE_SIZE) == WHAD_SUCCESS);
    TEST_ASSERT(whad_transport_get_txbuf_size() == WHAD_RINGBUF_CAPACITY);
    drain_transport();
    TEST_ASSERT(sent_size == WHAD_RINGBUF_CAPACITY);
    TEST_ASSERT(sent_bytes[0] == 0xAC);
    TEST_ASSERT(sent_bytes[1] == 0xBE);
    TEST_ASSERT(sent_bytes[2] == (WHAD_MAX_ENCODED_MESSAGE_SIZE & 0xff));
    TEST_ASSERT(sent_bytes[3] == (WHAD_MAX_ENCODED_MESSAGE_SIZE >> 8));
    TEST_ASSERT(memcmp(&sent_bytes[WHAD_TRANSPORT_FRAME_HEADER_SIZE], payload, sizeof(payload)) == 0);

    return true;
}

static bool test_limit_plus_one_rejected_without_mutation(void)
{
    uint8_t payload[WHAD_MAX_ENCODED_MESSAGE_SIZE + 1];

    reset_transport(WHAD_RINGBUF_CAPACITY, capture_send);

    TEST_ASSERT(whad_transport_send_message(payload, WHAD_MAX_ENCODED_MESSAGE_SIZE + 1) == WHAD_ERROR);
    TEST_ASSERT(whad_transport_get_txbuf_size() == 0);

    return true;
}

static bool test_full_ring_rejects_without_partial_frame(void)
{
    uint8_t payload[WHAD_MAX_ENCODED_MESSAGE_SIZE];
    uint8_t one = 0x5a;

    memset(payload, 0x11, sizeof(payload));
    reset_transport(WHAD_RINGBUF_CAPACITY, NULL);

    TEST_ASSERT(whad_transport_send_message(payload, WHAD_MAX_ENCODED_MESSAGE_SIZE) == WHAD_SUCCESS);
    TEST_ASSERT(whad_transport_get_txbuf_size() == WHAD_RINGBUF_CAPACITY);
    TEST_ASSERT(whad_transport_send_message(&one, 1) == WHAD_RINGBUF_FULL);
    TEST_ASSERT(whad_transport_get_txbuf_size() == WHAD_RINGBUF_CAPACITY);

    return true;
}

static bool test_wraparound_frame_order(void)
{
    uint8_t first[20];
    uint8_t second[1000];
    int i;

    reset_transport(10, capture_send);
    for (i=0; i<20; i++)
    {
        first[i] = (uint8_t)(0x20 + i);
    }
    for (i=0; i<1000; i++)
    {
        second[i] = (uint8_t)(0x80 + i);
    }

    TEST_ASSERT(whad_transport_send_message(first, sizeof(first)) == WHAD_SUCCESS);
    TEST_ASSERT(whad_transport_send_pending() == WHAD_SUCCESS);
    whad_transport_data_sent();
    TEST_ASSERT(whad_transport_send_message(second, sizeof(second)) == WHAD_SUCCESS);
    drain_transport();
    TEST_ASSERT(sent_size == 1028);
    TEST_ASSERT(sent_bytes[0] == 0xAC);
    TEST_ASSERT(sent_bytes[1] == 0xBE);
    TEST_ASSERT(sent_bytes[2] == sizeof(first));
    TEST_ASSERT(memcmp(&sent_bytes[4], first, sizeof(first)) == 0);
    TEST_ASSERT(sent_bytes[24] == 0xAC);
    TEST_ASSERT(sent_bytes[25] == 0xBE);
    TEST_ASSERT(sent_bytes[26] == (sizeof(second) & 0xff));
    TEST_ASSERT(sent_bytes[27] == (sizeof(second) >> 8));
    TEST_ASSERT(memcmp(&sent_bytes[28], second, sizeof(second)) == 0);

    return true;
}

static bool test_partial_input_waits_for_payload(void)
{
    uint8_t part1[] = {0xAC, 0xBE, 0x03, 0x00, 0x01};
    uint8_t part2[] = {0x02, 0x03};
    uint8_t out[3];
    int size = sizeof(out);

    reset_transport(WHAD_RINGBUF_CAPACITY, capture_send);

    TEST_ASSERT(whad_transport_data_received(part1, sizeof(part1)) == WHAD_SUCCESS);
    TEST_ASSERT(whad_transport_get_message(out, &size) == WHAD_SUCCESS);
    TEST_ASSERT(size == 0);
    TEST_ASSERT(whad_transport_data_received(part2, sizeof(part2)) == WHAD_SUCCESS);
    size = sizeof(out);
    TEST_ASSERT(whad_transport_get_message(out, &size) == WHAD_SUCCESS);
    TEST_ASSERT(size == 3);
    TEST_ASSERT(out[0] == 0x01);
    TEST_ASSERT(out[1] == 0x02);
    TEST_ASSERT(out[2] == 0x03);

    return true;
}

static bool read_until_payload(uint8_t *p_out, int *p_size, int attempts)
{
    int i;

    for (i=0; i<attempts; i++)
    {
        int size = *p_size;
        whad_result_t result = whad_transport_get_message(p_out, &size);

        if ((result == WHAD_SUCCESS) && (size > 0))
        {
            *p_size = size;
            return true;
        }
    }

    return false;
}

static bool test_invalid_magic_resynchronizes(void)
{
    uint8_t input[] = {0x00, 0xAC, 0x00, 0xAC, 0xBE, 0x01, 0x00, 0x5a};
    uint8_t out[1];
    int size = sizeof(out);

    reset_transport(WHAD_RINGBUF_CAPACITY, capture_send);

    TEST_ASSERT(whad_transport_data_received(input, sizeof(input)) == WHAD_SUCCESS);
    TEST_ASSERT(read_until_payload(out, &size, 5));
    TEST_ASSERT(size == 1);
    TEST_ASSERT(out[0] == 0x5a);

    return true;
}

static bool test_malicious_length_resynchronizes(void)
{
    uint8_t input[] = {0xAC, 0xBE, 0xff, 0xff, 0xAC, 0xBE, 0x01, 0x00, 0x42};
    uint8_t out[1];
    int size = sizeof(out);

    reset_transport(WHAD_RINGBUF_CAPACITY, capture_send);

    TEST_ASSERT(whad_transport_data_received(input, sizeof(input)) == WHAD_SUCCESS);
    TEST_ASSERT(whad_transport_get_message(out, &size) == WHAD_ERROR);
    size = sizeof(out);
    TEST_ASSERT(read_until_payload(out, &size, 5));
    TEST_ASSERT(size == 1);
    TEST_ASSERT(out[0] == 0x42);

    return true;
}

int main(void)
{
    TEST_ASSERT(test_zero_size_frame());
    TEST_ASSERT(test_limit_frame());
    TEST_ASSERT(test_limit_plus_one_rejected_without_mutation());
    TEST_ASSERT(test_full_ring_rejects_without_partial_frame());
    TEST_ASSERT(test_wraparound_frame_order());
    TEST_ASSERT(test_partial_input_waits_for_payload());
    TEST_ASSERT(test_invalid_magic_resynchronizes());
    TEST_ASSERT(test_malicious_length_resynchronizes());

    return 0;
}
