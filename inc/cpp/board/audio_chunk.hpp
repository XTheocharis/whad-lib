#ifndef __INC_WHAD_BOARD_AUDIO_CHUNK_HPP
#define __INC_WHAD_BOARD_AUDIO_CHUNK_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class AudioChunk : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_AudioChunk m_value;

        public:
            AudioChunk(BoardMsg &message);
            AudioChunk(uint32_t request_id);

            uint32_t getRequestId(void);
            board_AudioChunk& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_AUDIO_CHUNK_HPP */
