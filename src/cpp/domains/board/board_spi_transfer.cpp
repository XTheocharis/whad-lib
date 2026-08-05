#include <board/spi_transfer.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a SpiTransfer message.
 *
 * @param[in]   message     Message to parse
 */

SpiTransfer::SpiTransfer(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new SpiTransfer message.
 *
 * @param[in]   request_id  Request identifier
 */

SpiTransfer::SpiTransfer(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t SpiTransfer::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_SpiTransferRequest& SpiTransfer::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void SpiTransfer::pack()
{
    if (whad_board_spi_transfer(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void SpiTransfer::unpack()
{
    if (whad_board_spi_transfer_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
