#include <board/get_runtime_config.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a GetRuntimeConfig message.
 *
 * @param[in]   message     Message to parse
 */

GetRuntimeConfig::GetRuntimeConfig(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new GetRuntimeConfig message.
 *
 * @param[in]   request_id  Request identifier
 */

GetRuntimeConfig::GetRuntimeConfig(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t GetRuntimeConfig::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_GetRuntimeConfigRequest& GetRuntimeConfig::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void GetRuntimeConfig::pack()
{
    if (whad_board_get_runtime_config(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void GetRuntimeConfig::unpack()
{
    if (whad_board_get_runtime_config_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
