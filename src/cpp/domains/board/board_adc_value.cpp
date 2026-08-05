#include <board/adc_value.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a AdcValue message.
 *
 * @param[in]   message     Message to parse
 */

AdcValue::AdcValue(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new AdcValue message.
 *
 * @param[in]   request_id  Request identifier
 */

AdcValue::AdcValue(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t AdcValue::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_AdcReadResponse& AdcValue::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void AdcValue::pack()
{
    if (whad_board_adc_value(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void AdcValue::unpack()
{
    if (whad_board_adc_value_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
