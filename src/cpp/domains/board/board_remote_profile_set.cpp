#include <board/remote_profile_set.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a RemoteProfileSet message.
 *
 * @param[in]   message     Message to parse
 */

RemoteProfileSet::RemoteProfileSet(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new RemoteProfileSet message.
 *
 * @param[in]   request_id  Request identifier
 */

RemoteProfileSet::RemoteProfileSet(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t RemoteProfileSet::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_RemoteProfileSetRequest& RemoteProfileSet::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void RemoteProfileSet::pack()
{
    if (whad_board_remote_profile_set(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void RemoteProfileSet::unpack()
{
    if (whad_board_remote_profile_set_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
