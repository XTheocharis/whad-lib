#include <board/i2c_transfer.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a I2cTransfer message.
 *
 * @param[in]   message     Message to parse
 */

I2cTransfer::I2cTransfer(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new I2cTransfer message.
 *
 * @param[in]   request_id  Request identifier
 */

I2cTransfer::I2cTransfer(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t I2cTransfer::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_I2cTransferRequest& I2cTransfer::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void I2cTransfer::pack()
{
    if (whad_board_i2c_transfer(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void I2cTransfer::unpack()
{
    if (whad_board_i2c_transfer_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
