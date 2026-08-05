#ifndef __INC_WHAD_BOARD_SENSOR_SAMPLE_HPP
#define __INC_WHAD_BOARD_SENSOR_SAMPLE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class SensorSample : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_SensorSample m_value;

        public:
            SensorSample(BoardMsg &message);
            SensorSample(uint32_t request_id);

            uint32_t getRequestId(void);
            board_SensorSample& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_SENSOR_SAMPLE_HPP */
