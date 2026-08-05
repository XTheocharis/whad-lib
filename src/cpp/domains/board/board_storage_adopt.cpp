#include <board/storage_adopt.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a StorageAdopt message.
 *
 * @param[in]   message     Message to parse
 */

StorageAdopt::StorageAdopt(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new StorageAdopt message.
 *
 * @param[in]   request_id  Request identifier
 */

StorageAdopt::StorageAdopt(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t StorageAdopt::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_StorageAdoptRequest& StorageAdopt::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void StorageAdopt::pack()
{
    if (whad_board_storage_adopt(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void StorageAdopt::unpack()
{
    if (whad_board_storage_adopt_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
