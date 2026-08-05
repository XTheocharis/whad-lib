#include <board/gpio_value.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a GpioValue message.
 *
 * @param[in]   message     Message to parse
 */

GpioValue::GpioValue(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new GpioValue message.
 *
 * @param[in]   request_id  Request identifier
 */

GpioValue::GpioValue(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t GpioValue::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_GpioReadResponse& GpioValue::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void GpioValue::pack()
{
    if (whad_board_gpio_value(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void GpioValue::unpack()
{
    if (whad_board_gpio_value_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
