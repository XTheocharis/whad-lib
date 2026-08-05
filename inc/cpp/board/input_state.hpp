#ifndef __INC_WHAD_BOARD_INPUT_STATE_HPP
#define __INC_WHAD_BOARD_INPUT_STATE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class InputState : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_InputStateResponse m_value;

        public:
            InputState(BoardMsg &message);
            InputState(uint32_t request_id);

            uint32_t getRequestId(void);
            board_InputStateResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_INPUT_STATE_HPP */
