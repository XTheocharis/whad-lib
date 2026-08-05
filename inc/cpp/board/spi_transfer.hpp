#ifndef __INC_WHAD_BOARD_SPI_TRANSFER_HPP
#define __INC_WHAD_BOARD_SPI_TRANSFER_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class SpiTransfer : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_SpiTransferRequest m_value;

        public:
            SpiTransfer(BoardMsg &message);
            SpiTransfer(uint32_t request_id);

            uint32_t getRequestId(void);
            board_SpiTransferRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_SPI_TRANSFER_HPP */
