#ifndef __INC_WHAD_BOARD_STREAM_CONFIGURED_HPP
#define __INC_WHAD_BOARD_STREAM_CONFIGURED_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class StreamConfigured : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_ConfigureStreamResponse m_value;

        public:
            StreamConfigured(BoardMsg &message);
            StreamConfigured(uint32_t request_id);

            uint32_t getRequestId(void);
            board_ConfigureStreamResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_STREAM_CONFIGURED_HPP */
