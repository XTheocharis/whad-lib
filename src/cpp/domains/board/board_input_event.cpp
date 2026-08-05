#include <board/input_event.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a InputEvent message.
 *
 * @param[in]   message     Message to parse
 */

InputEvent::InputEvent(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new InputEvent message.
 *
 * @param[in]   request_id  Request identifier
 */

InputEvent::InputEvent(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t InputEvent::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_InputEvent& InputEvent::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void InputEvent::pack()
{
    if (whad_board_input_event(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void InputEvent::unpack()
{
    if (whad_board_input_event_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
