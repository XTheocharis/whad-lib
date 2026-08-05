#ifndef __INC_WHAD_BOARD_CALIBRATE_HPP
#define __INC_WHAD_BOARD_CALIBRATE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class Calibrate : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_CalibrateRequest m_value;

        public:
            Calibrate(BoardMsg &message);
            Calibrate(uint32_t request_id);

            uint32_t getRequestId(void);
            board_CalibrateRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_CALIBRATE_HPP */
