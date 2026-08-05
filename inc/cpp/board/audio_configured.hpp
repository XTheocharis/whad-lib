#ifndef __INC_WHAD_BOARD_AUDIO_CONFIGURED_HPP
#define __INC_WHAD_BOARD_AUDIO_CONFIGURED_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class AudioConfigured : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_AudioConfigureResponse m_value;

        public:
            AudioConfigured(BoardMsg &message);
            AudioConfigured(uint32_t request_id);

            uint32_t getRequestId(void);
            board_AudioConfigureResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_AUDIO_CONFIGURED_HPP */
