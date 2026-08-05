#ifndef __INC_WHAD_BOARD_STORAGE_INFO_HPP
#define __INC_WHAD_BOARD_STORAGE_INFO_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class StorageInfo : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_StorageInfoRequest m_value;

        public:
            StorageInfo(BoardMsg &message);
            StorageInfo(uint32_t request_id);

            uint32_t getRequestId(void);
            board_StorageInfoRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_STORAGE_INFO_HPP */
