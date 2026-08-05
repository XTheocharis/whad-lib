#ifndef __INC_WHAD_BOARD_SPI_RESULT_HPP
#define __INC_WHAD_BOARD_SPI_RESULT_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class SpiResult : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_SpiTransferResponse m_value;

        public:
            SpiResult(BoardMsg &message);
            SpiResult(uint32_t request_id);

            uint32_t getRequestId(void);
            board_SpiTransferResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_SPI_RESULT_HPP */
