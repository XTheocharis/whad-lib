#include <board/remote_profile_get.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a RemoteProfileGet message.
 *
 * @param[in]   message     Message to parse
 */

RemoteProfileGet::RemoteProfileGet(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new RemoteProfileGet message.
 *
 * @param[in]   request_id  Request identifier
 */

RemoteProfileGet::RemoteProfileGet(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t RemoteProfileGet::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_RemoteProfileGetRequest& RemoteProfileGet::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void RemoteProfileGet::pack()
{
    if (whad_board_remote_profile_get(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void RemoteProfileGet::unpack()
{
    if (whad_board_remote_profile_get_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
