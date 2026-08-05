#ifndef __INC_WHAD_BOARD_SET_RUNTIME_CONFIG_HPP
#define __INC_WHAD_BOARD_SET_RUNTIME_CONFIG_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class SetRuntimeConfig : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_SetRuntimeConfigRequest m_value;

        public:
            SetRuntimeConfig(BoardMsg &message);
            SetRuntimeConfig(uint32_t request_id);

            uint32_t getRequestId(void);
            board_SetRuntimeConfigRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_SET_RUNTIME_CONFIG_HPP */
