#ifndef __INC_WHAD_BOARD_ADC_READ_HPP
#define __INC_WHAD_BOARD_ADC_READ_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class AdcRead : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_AdcReadRequest m_value;

        public:
            AdcRead(BoardMsg &message);
            AdcRead(uint32_t request_id);

            uint32_t getRequestId(void);
            board_AdcReadRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_ADC_READ_HPP */
