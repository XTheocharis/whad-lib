#ifndef __INC_WHAD_BOARD_GET_INPUT_STATE_HPP
#define __INC_WHAD_BOARD_GET_INPUT_STATE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class GetInputState : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GetInputStateRequest m_value;

        public:
            GetInputState(BoardMsg &message);
            GetInputState(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GetInputStateRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_GET_INPUT_STATE_HPP */
