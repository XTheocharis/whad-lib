#ifndef __INC_WHAD_BOARD_SET_RUNTIME_MODE_HPP
#define __INC_WHAD_BOARD_SET_RUNTIME_MODE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class SetRuntimeMode : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_SetRuntimeModeRequest m_value;

        public:
            SetRuntimeMode(BoardMsg &message);
            SetRuntimeMode(uint32_t request_id);

            uint32_t getRequestId(void);
            board_SetRuntimeModeRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_SET_RUNTIME_MODE_HPP */
