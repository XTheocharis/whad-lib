#include <board/calibration.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a Calibration message.
 *
 * @param[in]   message     Message to parse
 */

Calibration::Calibration(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new Calibration message.
 *
 * @param[in]   request_id  Request identifier
 */

Calibration::Calibration(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t Calibration::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_CalibrationResponse& Calibration::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void Calibration::pack()
{
    if (whad_board_calibration(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void Calibration::unpack()
{
    if (whad_board_calibration_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
