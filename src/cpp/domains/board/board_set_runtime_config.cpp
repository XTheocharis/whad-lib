#include <board/set_runtime_config.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a SetRuntimeConfig message.
 *
 * @param[in]   message     Message to parse
 */

SetRuntimeConfig::SetRuntimeConfig(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new SetRuntimeConfig message.
 *
 * @param[in]   request_id  Request identifier
 */

SetRuntimeConfig::SetRuntimeConfig(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t SetRuntimeConfig::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_SetRuntimeConfigRequest& SetRuntimeConfig::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void SetRuntimeConfig::pack()
{
    if (whad_board_set_runtime_config(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void SetRuntimeConfig::unpack()
{
    if (whad_board_set_runtime_config_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
