#ifndef __INC_WHAD_BOARD_BOARD_INFO_HPP
#define __INC_WHAD_BOARD_BOARD_INFO_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class BoardInfo : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GetBoardInfoResponse m_value;

        public:
            BoardInfo(BoardMsg &message);
            BoardInfo(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GetBoardInfoResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_BOARD_INFO_HPP */
