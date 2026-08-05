#include <board/audio_configured.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a AudioConfigured message.
 *
 * @param[in]   message     Message to parse
 */

AudioConfigured::AudioConfigured(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new AudioConfigured message.
 *
 * @param[in]   request_id  Request identifier
 */

AudioConfigured::AudioConfigured(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t AudioConfigured::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_AudioConfigureResponse& AudioConfigured::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void AudioConfigured::pack()
{
    if (whad_board_audio_configured(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void AudioConfigured::unpack()
{
    if (whad_board_audio_configured_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
