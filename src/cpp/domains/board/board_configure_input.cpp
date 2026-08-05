#include <board/configure_input.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a ConfigureInput message.
 *
 * @param[in]   message     Message to parse
 */

ConfigureInput::ConfigureInput(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new ConfigureInput message.
 *
 * @param[in]   request_id  Request identifier
 */

ConfigureInput::ConfigureInput(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t ConfigureInput::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_ConfigureInputRequest& ConfigureInput::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void ConfigureInput::pack()
{
    if (whad_board_configure_input(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void ConfigureInput::unpack()
{
    if (whad_board_configure_input_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
