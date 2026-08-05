#ifndef __INC_WHAD_BOARD_GET_RUNTIME_CONFIG_HPP
#define __INC_WHAD_BOARD_GET_RUNTIME_CONFIG_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class GetRuntimeConfig : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_GetRuntimeConfigRequest m_value;

        public:
            GetRuntimeConfig(BoardMsg &message);
            GetRuntimeConfig(uint32_t request_id);

            uint32_t getRequestId(void);
            board_GetRuntimeConfigRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_GET_RUNTIME_CONFIG_HPP */
