#include <board/release_pin.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a ReleasePin message.
 *
 * @param[in]   message     Message to parse
 */

ReleasePin::ReleasePin(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new ReleasePin message.
 *
 * @param[in]   request_id  Request identifier
 */

ReleasePin::ReleasePin(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t ReleasePin::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_ReleasePinRequest& ReleasePin::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void ReleasePin::pack()
{
    if (whad_board_release_pin(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void ReleasePin::unpack()
{
    if (whad_board_release_pin_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
