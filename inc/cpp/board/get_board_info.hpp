#ifndef __INC_WHAD_BOARD_GET_BOARD_INFO_HPP
#define __INC_WHAD_BOARD_GET_BOARD_INFO_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class GetBoardInfo : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GetBoardInfoRequest m_value;

        public:
            GetBoardInfo(BoardMsg &message);
            GetBoardInfo(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GetBoardInfoRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_GET_BOARD_INFO_HPP */
