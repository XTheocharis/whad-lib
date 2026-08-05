#ifndef __INC_WHAD_BOARD_GPIO_VALUE_HPP
#define __INC_WHAD_BOARD_GPIO_VALUE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class GpioValue : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GpioReadResponse m_value;

        public:
            GpioValue(BoardMsg &message);
            GpioValue(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GpioReadResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_GPIO_VALUE_HPP */
