#ifndef __INC_WHAD_BOARD_I2C_RESULT_HPP
#define __INC_WHAD_BOARD_I2C_RESULT_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class I2cResult : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_I2cTransferResponse m_value;

        public:
            I2cResult(BoardMsg &message);
            I2cResult(uint32_t request_id);

            uint32_t getRequestId(void);
            board_I2cTransferResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_I2C_RESULT_HPP */
