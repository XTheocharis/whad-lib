#include <board/input_state.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a InputState message.
 *
 * @param[in]   message     Message to parse
 */

InputState::InputState(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new InputState message.
 *
 * @param[in]   request_id  Request identifier
 */

InputState::InputState(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t InputState::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_InputStateResponse& InputState::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void InputState::pack()
{
    if (whad_board_input_state(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void InputState::unpack()
{
    if (whad_board_input_state_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
