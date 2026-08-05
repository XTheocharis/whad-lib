#include <board/storage_info.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a StorageInfo message.
 *
 * @param[in]   message     Message to parse
 */

StorageInfo::StorageInfo(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new StorageInfo message.
 *
 * @param[in]   request_id  Request identifier
 */

StorageInfo::StorageInfo(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t StorageInfo::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_StorageInfoRequest& StorageInfo::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void StorageInfo::pack()
{
    if (whad_board_storage_info(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void StorageInfo::unpack()
{
    if (whad_board_storage_info_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
