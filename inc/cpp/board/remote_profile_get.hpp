#ifndef __INC_WHAD_BOARD_REMOTE_PROFILE_GET_HPP
#define __INC_WHAD_BOARD_REMOTE_PROFILE_GET_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class RemoteProfileGet : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_RemoteProfileGetRequest m_value;

        public:
            RemoteProfileGet(BoardMsg &message);
            RemoteProfileGet(uint32_t request_id);

            uint32_t getRequestId(void);
            board_RemoteProfileGetRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_REMOTE_PROFILE_GET_HPP */
