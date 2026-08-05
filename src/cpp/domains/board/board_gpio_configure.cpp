#include <board/gpio_configure.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a GpioConfigure message.
 *
 * @param[in]   message     Message to parse
 */

GpioConfigure::GpioConfigure(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new GpioConfigure message.
 *
 * @param[in]   request_id  Request identifier
 */

GpioConfigure::GpioConfigure(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t GpioConfigure::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_GpioConfigureRequest& GpioConfigure::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void GpioConfigure::pack()
{
    if (whad_board_gpio_configure(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void GpioConfigure::unpack()
{
    if (whad_board_gpio_configure_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
