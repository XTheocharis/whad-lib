#include <board/get_board_info.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a GetBoardInfo message.
 *
 * @param[in]   message     Message to parse
 */

GetBoardInfo::GetBoardInfo(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new GetBoardInfo message.
 *
 * @param[in]   request_id  Request identifier
 */

GetBoardInfo::GetBoardInfo(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t GetBoardInfo::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_GetBoardInfoRequest& GetBoardInfo::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void GetBoardInfo::pack()
{
    if (whad_board_get_board_info(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void GetBoardInfo::unpack()
{
    if (whad_board_get_board_info_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
