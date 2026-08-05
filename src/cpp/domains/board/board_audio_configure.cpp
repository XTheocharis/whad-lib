#include <board/audio_configure.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a AudioConfigure message.
 *
 * @param[in]   message     Message to parse
 */

AudioConfigure::AudioConfigure(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new AudioConfigure message.
 *
 * @param[in]   request_id  Request identifier
 */

AudioConfigure::AudioConfigure(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t AudioConfigure::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_AudioConfigureRequest& AudioConfigure::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void AudioConfigure::pack()
{
    if (whad_board_audio_configure(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void AudioConfigure::unpack()
{
    if (whad_board_audio_configure_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
