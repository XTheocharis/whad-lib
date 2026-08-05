#include <board/i2c_result.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a I2cResult message.
 *
 * @param[in]   message     Message to parse
 */

I2cResult::I2cResult(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new I2cResult message.
 *
 * @param[in]   request_id  Request identifier
 */

I2cResult::I2cResult(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t I2cResult::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_I2cTransferResponse& I2cResult::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void I2cResult::pack()
{
    if (whad_board_i2c_result(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void I2cResult::unpack()
{
    if (whad_board_i2c_result_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
