#include <board/storage_read_log.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a StorageReadLog message.
 *
 * @param[in]   message     Message to parse
 */

StorageReadLog::StorageReadLog(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new StorageReadLog message.
 *
 * @param[in]   request_id  Request identifier
 */

StorageReadLog::StorageReadLog(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t StorageReadLog::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_StorageReadLogRequest& StorageReadLog::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void StorageReadLog::pack()
{
    if (whad_board_storage_read_log(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void StorageReadLog::unpack()
{
    if (whad_board_storage_read_log_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
