Board domain
============

The Board domain exposes host-side access to on-board peripherals (sensors,
GPIO/ADC/SPI/I2C expert I/O, QSPI storage, runtime mode control) on capable
WHAD interfaces such as the Adafruit CLUE. It is a request/response domain
rather than a packet-stream domain, so Board messages flow through
:cpp:func:`whad_board_get_message_type` and a per-command dispatch switch
rather than through the scapy-based packet pipeline used by BLE, ESB, and PHY.

Parsing Board messages
----------------------

When processing Board domain messages sent from a compatible interface, the
:cpp:func:`whad_board_get_message_type` function returns the corresponding
message type (:cpp:enum:`whad_board_msgtype_t`). For host-bound commands, the
companion :cpp:func:`whad_board_command_from_message_type` maps a message type
back into a :cpp:type:`board_BoardCommand` enum value used by the interface's
capability bitmask.

The X-macro ``WHAD_BOARD_MESSAGE_LIST(X)`` enumerates all 49 pack/parse
function pairs (28 commands plus 21 responses and events) and is the single
source of truth kept in sync with ``board.proto`` by hand. The following
example shows a minimal message-processing function for an interface that
handles a few Board commands and falls back to a generic result for the rest.

.. code-block:: c

    void process_board_message(Message *p_message)
    {
        Message response;

        switch (whad_board_get_message_type(p_message))
        {
            case WHAD_BOARD_GET_BOARD_INFO:
            {
                board_GetBoardInfoRequest req;
                uint32_t request_id;

                /* Parse the request and populate board info ... (custom code) */
                whad_board_get_board_info_parse(p_message, &request_id, &req);

                /* Return a BoardInfo response. */
                /* ... */

                /* Fall back to a success result. */
                whad_generic_cmd_result(&response, WHAD_RESULT_SUCCESS);
            }
            break;

            case WHAD_BOARD_GPIO_READ:
            {
                board_GpioReadRequest req;
                uint32_t request_id;

                whad_board_gpio_read_parse(p_message, &request_id, &req);
                /* Read the pin ... (custom code) */

                whad_generic_cmd_result(&response, WHAD_RESULT_SUCCESS);
            }
            break;

            case WHAD_BOARD_ADC_READ:
            {
                board_AdcReadRequest req;
                uint32_t request_id;

                whad_board_adc_read_parse(p_message, &request_id, &req);
                /* Sample the ADC channel ... (custom code) */

                whad_generic_cmd_result(&response, WHAD_RESULT_SUCCESS);
            }
            break;

            default:
            {
                /* Not handled by this interface. */
                whad_generic_cmd_result(&response, WHAD_RESULT_ERROR);
            }
            break;
        }

        /* Send the response to the host. */
        whad_send_message(&response);
    }

Board domain API reference
--------------------------

.. doxygenfile:: inc/domains/board.h
    :sections: define enums

.. doxygenfile:: src/domains/board.c
