#ifndef __INC_WHAD_BOARD_BOARD_STATUS_HPP
#define __INC_WHAD_BOARD_BOARD_STATUS_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class BoardStatus : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_BoardStatus m_value;

        public:
            BoardStatus(BoardMsg &message);
            BoardStatus(uint32_t request_id);

            uint32_t getRequestId(void);
            board_BoardStatus& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_BOARD_STATUS_HPP */
