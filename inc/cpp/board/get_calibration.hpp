#ifndef __INC_WHAD_BOARD_GET_CALIBRATION_HPP
#define __INC_WHAD_BOARD_GET_CALIBRATION_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class GetCalibration : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GetCalibrationRequest m_value;

        public:
            GetCalibration(BoardMsg &message);
            GetCalibration(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GetCalibrationRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_GET_CALIBRATION_HPP */
