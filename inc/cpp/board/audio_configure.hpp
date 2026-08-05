#ifndef __INC_WHAD_BOARD_AUDIO_CONFIGURE_HPP
#define __INC_WHAD_BOARD_AUDIO_CONFIGURE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class AudioConfigure : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_AudioConfigureRequest m_value;

        public:
            AudioConfigure(BoardMsg &message);
            AudioConfigure(uint32_t request_id);

            uint32_t getRequestId(void);
            board_AudioConfigureRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_AUDIO_CONFIGURE_HPP */
