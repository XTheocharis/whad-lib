#ifndef __INC_WHAD_BOARD_CALIBRATION_HPP
#define __INC_WHAD_BOARD_CALIBRATION_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class Calibration : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_CalibrationResponse m_value;

        public:
            Calibration(BoardMsg &message);
            Calibration(uint32_t request_id);

            uint32_t getRequestId(void);
            board_CalibrationResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_CALIBRATION_HPP */
