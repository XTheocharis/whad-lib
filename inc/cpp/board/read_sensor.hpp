#ifndef __INC_WHAD_BOARD_READ_SENSOR_HPP
#define __INC_WHAD_BOARD_READ_SENSOR_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class ReadSensor : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_ReadSensorRequest m_value;

        public:
            ReadSensor(BoardMsg &message);
            ReadSensor(uint32_t request_id);

            uint32_t getRequestId(void);
            board_ReadSensorRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_READ_SENSOR_HPP */
