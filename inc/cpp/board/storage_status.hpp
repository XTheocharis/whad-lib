#ifndef __INC_WHAD_BOARD_STORAGE_STATUS_HPP
#define __INC_WHAD_BOARD_STORAGE_STATUS_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class StorageStatus : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_StorageInfoResponse m_value;

        public:
            StorageStatus(BoardMsg &message);
            StorageStatus(uint32_t request_id);

            uint32_t getRequestId(void);
            board_StorageInfoResponse& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_STORAGE_STATUS_HPP */
