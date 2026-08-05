#ifndef __INC_WHAD_BOARD_STOP_STREAM_HPP
#define __INC_WHAD_BOARD_STOP_STREAM_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class StopStream : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_StopStreamRequest m_value;

        public:
            StopStream(BoardMsg &message);
            StopStream(uint32_t request_id);

            uint32_t getRequestId(void);
            board_StopStreamRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_STOP_STREAM_HPP */
