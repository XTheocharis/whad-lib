#ifndef __INC_WHAD_BOARD_STORAGE_ERASE_LOG_HPP
#define __INC_WHAD_BOARD_STORAGE_ERASE_LOG_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class StorageEraseLog : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_StorageEraseLogRequest m_value;

        public:
            StorageEraseLog(BoardMsg &message);
            StorageEraseLog(uint32_t request_id);

            uint32_t getRequestId(void);
            board_StorageEraseLogRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_STORAGE_ERASE_LOG_HPP */
