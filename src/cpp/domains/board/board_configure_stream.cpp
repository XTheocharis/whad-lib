#include <board/configure_stream.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a ConfigureStream message.
 *
 * @param[in]   message     Message to parse
 */

ConfigureStream::ConfigureStream(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new ConfigureStream message.
 *
 * @param[in]   request_id  Request identifier
 */

ConfigureStream::ConfigureStream(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t ConfigureStream::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_ConfigureStreamRequest& ConfigureStream::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void ConfigureStream::pack()
{
    if (whad_board_configure_stream(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void ConfigureStream::unpack()
{
    if (whad_board_configure_stream_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
