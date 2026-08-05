#include <board/storage_status.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a StorageStatus message.
 *
 * @param[in]   message     Message to parse
 */

StorageStatus::StorageStatus(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new StorageStatus message.
 *
 * @param[in]   request_id  Request identifier
 */

StorageStatus::StorageStatus(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t StorageStatus::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_StorageInfoResponse& StorageStatus::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void StorageStatus::pack()
{
    if (whad_board_storage_status(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void StorageStatus::unpack()
{
    if (whad_board_storage_status_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
