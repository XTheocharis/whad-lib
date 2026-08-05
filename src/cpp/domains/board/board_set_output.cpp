#include <board/set_output.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a SetOutput message.
 *
 * @param[in]   message     Message to parse
 */

SetOutput::SetOutput(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new SetOutput message.
 *
 * @param[in]   request_id  Request identifier
 */

SetOutput::SetOutput(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t SetOutput::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_SetOutputRequest& SetOutput::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void SetOutput::pack()
{
    if (whad_board_set_output(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void SetOutput::unpack()
{
    if (whad_board_set_output_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
