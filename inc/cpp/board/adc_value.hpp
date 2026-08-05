#ifndef __INC_WHAD_BOARD_ADC_VALUE_HPP
#define __INC_WHAD_BOARD_ADC_VALUE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class AdcValue : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_AdcReadResponse m_value;

        public:
            AdcValue(BoardMsg &message);
            AdcValue(uint32_t request_id);

            uint32_t getRequestId(void);
            board_AdcReadResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_ADC_VALUE_HPP */
