#ifndef __INC_WHAD_BOARD_STORAGE_READ_LOG_HPP
#define __INC_WHAD_BOARD_STORAGE_READ_LOG_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class StorageReadLog : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_StorageReadLogRequest m_value;

        public:
            StorageReadLog(BoardMsg &message);
            StorageReadLog(uint32_t request_id);

            uint32_t getRequestId(void);
            board_StorageReadLogRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_STORAGE_READ_LOG_HPP */
