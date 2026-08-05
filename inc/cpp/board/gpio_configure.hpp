#ifndef __INC_WHAD_BOARD_GPIO_CONFIGURE_HPP
#define __INC_WHAD_BOARD_GPIO_CONFIGURE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class GpioConfigure : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GpioConfigureRequest m_value;

        public:
            GpioConfigure(BoardMsg &message);
            GpioConfigure(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GpioConfigureRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_GPIO_CONFIGURE_HPP */
