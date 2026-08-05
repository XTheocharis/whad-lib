#include <board/gesture_event.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a GestureEvent message.
 *
 * @param[in]   message     Message to parse
 */

GestureEvent::GestureEvent(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new GestureEvent message.
 *
 * @param[in]   request_id  Request identifier
 */

GestureEvent::GestureEvent(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t GestureEvent::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_GestureEvent& GestureEvent::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void GestureEvent::pack()
{
    if (whad_board_gesture_event(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void GestureEvent::unpack()
{
    if (whad_board_gesture_event_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
