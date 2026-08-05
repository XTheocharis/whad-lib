#ifndef __INC_WHAD_BOARD_RELEASE_PIN_HPP
#define __INC_WHAD_BOARD_RELEASE_PIN_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class ReleasePin : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_ReleasePinRequest m_value;

        public:
            ReleasePin(BoardMsg &message);
            ReleasePin(uint32_t request_id);

            uint32_t getRequestId(void);
            board_ReleasePinRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_RELEASE_PIN_HPP */
