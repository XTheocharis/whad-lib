#ifndef __INC_WHAD_BOARD_GPIO_WRITE_HPP
#define __INC_WHAD_BOARD_GPIO_WRITE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class GpioWrite : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GpioWriteRequest m_value;

        public:
            GpioWrite(BoardMsg &message);
            GpioWrite(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GpioWriteRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_GPIO_WRITE_HPP */
