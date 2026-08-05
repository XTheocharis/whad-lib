#include <board/gpio_read.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a GpioRead message.
 *
 * @param[in]   message     Message to parse
 */

GpioRead::GpioRead(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new GpioRead message.
 *
 * @param[in]   request_id  Request identifier
 */

GpioRead::GpioRead(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t GpioRead::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_GpioReadRequest& GpioRead::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void GpioRead::pack()
{
    if (whad_board_gpio_read(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void GpioRead::unpack()
{
    if (whad_board_gpio_read_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
