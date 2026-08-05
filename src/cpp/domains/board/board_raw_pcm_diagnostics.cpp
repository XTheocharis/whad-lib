#include <board/raw_pcm_diagnostics.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a RawPcmDiagnostics message.
 *
 * @param[in]   message     Message to parse
 */

RawPcmDiagnostics::RawPcmDiagnostics(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new RawPcmDiagnostics message.
 *
 * @param[in]   request_id  Request identifier
 */

RawPcmDiagnostics::RawPcmDiagnostics(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t RawPcmDiagnostics::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_RawPcmDiagnosticsRequest& RawPcmDiagnostics::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void RawPcmDiagnostics::pack()
{
    if (whad_board_raw_pcm_diagnostics(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void RawPcmDiagnostics::unpack()
{
    if (whad_board_raw_pcm_diagnostics_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
