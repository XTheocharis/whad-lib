#include <board/stream_configured.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a StreamConfigured message.
 *
 * @param[in]   message     Message to parse
 */

StreamConfigured::StreamConfigured(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new StreamConfigured message.
 *
 * @param[in]   request_id  Request identifier
 */

StreamConfigured::StreamConfigured(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t StreamConfigured::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_ConfigureStreamResponse& StreamConfigured::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void StreamConfigured::pack()
{
    if (whad_board_stream_configured(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void StreamConfigured::unpack()
{
    if (whad_board_stream_configured_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
