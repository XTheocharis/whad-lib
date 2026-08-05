#include <board/set_runtime_mode.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a SetRuntimeMode message.
 *
 * @param[in]   message     Message to parse
 */

SetRuntimeMode::SetRuntimeMode(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new SetRuntimeMode message.
 *
 * @param[in]   request_id  Request identifier
 */

SetRuntimeMode::SetRuntimeMode(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t SetRuntimeMode::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_SetRuntimeModeRequest& SetRuntimeMode::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void SetRuntimeMode::pack()
{
    if (whad_board_set_runtime_mode(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void SetRuntimeMode::unpack()
{
    if (whad_board_set_runtime_mode_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
