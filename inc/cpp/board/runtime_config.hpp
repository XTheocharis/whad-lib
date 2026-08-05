#ifndef __INC_WHAD_BOARD_RUNTIME_CONFIG_HPP
#define __INC_WHAD_BOARD_RUNTIME_CONFIG_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class RuntimeConfig : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_RuntimeConfigResponse m_value;

        public:
            RuntimeConfig(BoardMsg &message);
            RuntimeConfig(uint32_t request_id);

            uint32_t getRequestId(void);
            board_RuntimeConfigResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_RUNTIME_CONFIG_HPP */
