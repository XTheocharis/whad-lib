#include <board/remote_profile.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a RemoteProfile message.
 *
 * @param[in]   message     Message to parse
 */

RemoteProfile::RemoteProfile(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new RemoteProfile message.
 *
 * @param[in]   request_id  Request identifier
 */

RemoteProfile::RemoteProfile(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t RemoteProfile::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_RemoteProfileResponse& RemoteProfile::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void RemoteProfile::pack()
{
    if (whad_board_remote_profile(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void RemoteProfile::unpack()
{
    if (whad_board_remote_profile_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
