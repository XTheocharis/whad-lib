#include <board/audio_chunk.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a AudioChunk message.
 *
 * @param[in]   message     Message to parse
 */

AudioChunk::AudioChunk(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new AudioChunk message.
 *
 * @param[in]   request_id  Request identifier
 */

AudioChunk::AudioChunk(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t AudioChunk::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_AudioChunk& AudioChunk::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void AudioChunk::pack()
{
    if (whad_board_audio_chunk(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void AudioChunk::unpack()
{
    if (whad_board_audio_chunk_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
