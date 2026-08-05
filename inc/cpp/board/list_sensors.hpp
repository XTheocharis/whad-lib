#ifndef __INC_WHAD_BOARD_LIST_SENSORS_HPP
#define __INC_WHAD_BOARD_LIST_SENSORS_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class ListSensors : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_ListSensorsRequest m_value;

        public:
            ListSensors(BoardMsg &message);
            ListSensors(uint32_t request_id);

            uint32_t getRequestId(void);
            board_ListSensorsRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_LIST_SENSORS_HPP */
