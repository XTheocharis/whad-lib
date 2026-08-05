#include <board/board_status.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a BoardStatus message.
 *
 * @param[in]   message     Message to parse
 */

BoardStatus::BoardStatus(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new BoardStatus message.
 *
 * @param[in]   request_id  Request identifier
 */

BoardStatus::BoardStatus(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t BoardStatus::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_BoardStatus& BoardStatus::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void BoardStatus::pack()
{
    if (whad_board_board_status(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void BoardStatus::unpack()
{
    if (whad_board_board_status_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
