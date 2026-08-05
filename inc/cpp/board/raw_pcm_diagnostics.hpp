#ifndef __INC_WHAD_BOARD_RAW_PCM_DIAGNOSTICS_HPP
#define __INC_WHAD_BOARD_RAW_PCM_DIAGNOSTICS_HPP

#include <string.h>
#include "message.hpp"
#include "common.hpp"
#include "board/base.hpp"

namespace whad::board {

    class RawPcmDiagnostics : public BoardMsg
    {
        private:
            void unpack();
            void pack();

            uint32_t m_requestId;
            board_RawPcmDiagnosticsRequest m_value;

        public:
            RawPcmDiagnostics(BoardMsg &message);
            RawPcmDiagnostics(uint32_t request_id);

            uint32_t getRequestId(void);
            board_RawPcmDiagnosticsRequest& getValue(void);
    };

}

#endif /* __INC_WHAD_BOARD_RAW_PCM_DIAGNOSTICS_HPP */
