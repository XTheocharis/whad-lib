#include <board/adc_read.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a AdcRead message.
 *
 * @param[in]   message     Message to parse
 */

AdcRead::AdcRead(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new AdcRead message.
 *
 * @param[in]   request_id  Request identifier
 */

AdcRead::AdcRead(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t AdcRead::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_AdcReadRequest& AdcRead::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void AdcRead::pack()
{
    if (whad_board_adc_read(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void AdcRead::unpack()
{
    if (whad_board_adc_read_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
