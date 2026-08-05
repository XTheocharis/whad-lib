#ifndef __INC_WHAD_BOARD_INPUT_EVENT_HPP
#define __INC_WHAD_BOARD_INPUT_EVENT_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class InputEvent : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_InputEvent m_value;

        public:
            InputEvent(BoardMsg &message);
            InputEvent(uint32_t request_id);

            uint32_t getRequestId(void);
            board_InputEvent& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_INPUT_EVENT_HPP */
