#ifndef __INC_WHAD_BOARD_GESTURE_EVENT_HPP
#define __INC_WHAD_BOARD_GESTURE_EVENT_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class GestureEvent : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GestureEvent m_value;

        public:
            GestureEvent(BoardMsg &message);
            GestureEvent(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GestureEvent& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_GESTURE_EVENT_HPP */
