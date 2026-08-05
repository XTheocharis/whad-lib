#include <board/spi_result.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a SpiResult message.
 *
 * @param[in]   message     Message to parse
 */

SpiResult::SpiResult(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new SpiResult message.
 *
 * @param[in]   request_id  Request identifier
 */

SpiResult::SpiResult(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t SpiResult::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_SpiTransferResponse& SpiResult::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void SpiResult::pack()
{
    if (whad_board_spi_result(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void SpiResult::unpack()
{
    if (whad_board_spi_result_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
