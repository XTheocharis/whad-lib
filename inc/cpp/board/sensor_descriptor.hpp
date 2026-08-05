#ifndef __INC_WHAD_BOARD_SENSOR_DESCRIPTOR_HPP
#define __INC_WHAD_BOARD_SENSOR_DESCRIPTOR_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class SensorDescriptor : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_ListSensorsResponse m_value;

        public:
            SensorDescriptor(BoardMsg &message);
            SensorDescriptor(uint32_t request_id);

            uint32_t getRequestId(void);
            board_ListSensorsResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_SENSOR_DESCRIPTOR_HPP */
