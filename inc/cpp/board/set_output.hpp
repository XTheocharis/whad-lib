#ifndef __INC_WHAD_BOARD_SET_OUTPUT_HPP
#define __INC_WHAD_BOARD_SET_OUTPUT_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class SetOutput : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_SetOutputRequest m_value;

        public:
            SetOutput(BoardMsg &message);
            SetOutput(uint32_t request_id);

            uint32_t getRequestId(void);
            board_SetOutputRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_SET_OUTPUT_HPP */
