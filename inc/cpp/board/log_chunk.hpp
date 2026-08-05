#ifndef __INC_WHAD_BOARD_LOG_CHUNK_HPP
#define __INC_WHAD_BOARD_LOG_CHUNK_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class LogChunk : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_LogChunk m_value;

        public:
            LogChunk(BoardMsg &message);
            LogChunk(uint32_t request_id);

            uint32_t getRequestId(void);
            board_LogChunk& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_LOG_CHUNK_HPP */
