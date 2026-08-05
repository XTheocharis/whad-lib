#include <board/gpio_write.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a GpioWrite message.
 *
 * @param[in]   message     Message to parse
 */

GpioWrite::GpioWrite(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new GpioWrite message.
 *
 * @param[in]   request_id  Request identifier
 */

GpioWrite::GpioWrite(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t GpioWrite::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_GpioWriteRequest& GpioWrite::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void GpioWrite::pack()
{
    if (whad_board_gpio_write(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void GpioWrite::unpack()
{
    if (whad_board_gpio_write_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
