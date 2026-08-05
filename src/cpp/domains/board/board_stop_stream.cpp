#include <board/stop_stream.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a StopStream message.
 *
 * @param[in]   message     Message to parse
 */

StopStream::StopStream(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new StopStream message.
 *
 * @param[in]   request_id  Request identifier
 */

StopStream::StopStream(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t StopStream::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_StopStreamRequest& StopStream::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void StopStream::pack()
{
    if (whad_board_stop_stream(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void StopStream::unpack()
{
    if (whad_board_stop_stream_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
