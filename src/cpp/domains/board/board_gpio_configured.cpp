#include <board/gpio_configured.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a GpioConfigured message.
 *
 * @param[in]   message     Message to parse
 */

GpioConfigured::GpioConfigured(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new GpioConfigured message.
 *
 * @param[in]   request_id  Request identifier
 */

GpioConfigured::GpioConfigured(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t GpioConfigured::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_GpioConfigureResponse& GpioConfigured::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void GpioConfigured::pack()
{
    if (whad_board_gpio_configured(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void GpioConfigured::unpack()
{
    if (whad_board_gpio_configured_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
