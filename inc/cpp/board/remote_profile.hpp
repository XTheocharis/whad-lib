#ifndef __INC_WHAD_BOARD_REMOTE_PROFILE_HPP
#define __INC_WHAD_BOARD_REMOTE_PROFILE_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class RemoteProfile : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_RemoteProfileResponse m_value;

        public:
            RemoteProfile(BoardMsg &message);
            RemoteProfile(uint32_t request_id);

            uint32_t getRequestId(void);
            board_RemoteProfileResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_REMOTE_PROFILE_HPP */
