#ifndef __INC_WHAD_BOARD_GPIO_READ_HPP
#define __INC_WHAD_BOARD_GPIO_READ_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class GpioRead : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GpioReadRequest m_value;

        public:
            GpioRead(BoardMsg &message);
            GpioRead(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GpioReadRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_GPIO_READ_HPP */
