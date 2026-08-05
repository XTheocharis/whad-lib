#include <board/storage_erase_log.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a StorageEraseLog message.
 *
 * @param[in]   message     Message to parse
 */

StorageEraseLog::StorageEraseLog(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new StorageEraseLog message.
 *
 * @param[in]   request_id  Request identifier
 */

StorageEraseLog::StorageEraseLog(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t StorageEraseLog::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_StorageEraseLogRequest& StorageEraseLog::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void StorageEraseLog::pack()
{
    if (whad_board_storage_erase_log(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void StorageEraseLog::unpack()
{
    if (whad_board_storage_erase_log_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
