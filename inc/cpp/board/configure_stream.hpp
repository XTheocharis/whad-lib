#ifndef __INC_WHAD_BOARD_CONFIGURE_STREAM_HPP
#define __INC_WHAD_BOARD_CONFIGURE_STREAM_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class ConfigureStream : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_ConfigureStreamRequest m_value;

        public:
            ConfigureStream(BoardMsg &message);
            ConfigureStream(uint32_t request_id);

            uint32_t getRequestId(void);
            board_ConfigureStreamRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_CONFIGURE_STREAM_HPP */
