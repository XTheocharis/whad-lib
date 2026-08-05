#include <board/log_chunk.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a LogChunk message.
 *
 * @param[in]   message     Message to parse
 */

LogChunk::LogChunk(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new LogChunk message.
 *
 * @param[in]   request_id  Request identifier
 */

LogChunk::LogChunk(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t LogChunk::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_LogChunk& LogChunk::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void LogChunk::pack()
{
    if (whad_board_log_chunk(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void LogChunk::unpack()
{
    if (whad_board_log_chunk_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
