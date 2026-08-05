#include <board/runtime_config.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a RuntimeConfig message.
 *
 * @param[in]   message     Message to parse
 */

RuntimeConfig::RuntimeConfig(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new RuntimeConfig message.
 *
 * @param[in]   request_id  Request identifier
 */

RuntimeConfig::RuntimeConfig(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t RuntimeConfig::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_RuntimeConfigResponse& RuntimeConfig::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void RuntimeConfig::pack()
{
    if (whad_board_runtime_config(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void RuntimeConfig::unpack()
{
    if (whad_board_runtime_config_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
