#ifndef __INC_WHAD_BOARD_GPIO_CONFIGURED_HPP
#define __INC_WHAD_BOARD_GPIO_CONFIGURED_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class GpioConfigured : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GpioConfigureResponse m_value;

        public:
            GpioConfigured(BoardMsg &message);
            GpioConfigured(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GpioConfigureResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_GPIO_CONFIGURED_HPP */
