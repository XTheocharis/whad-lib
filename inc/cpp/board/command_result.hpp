#ifndef __INC_WHAD_BOARD_COMMAND_RESULT_HPP
#define __INC_WHAD_BOARD_COMMAND_RESULT_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class CommandResult : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_CommandResult m_value;

        public:
            CommandResult(BoardMsg &message);
            CommandResult(uint32_t request_id);

            uint32_t getRequestId(void);
            board_CommandResult& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_COMMAND_RESULT_HPP */
