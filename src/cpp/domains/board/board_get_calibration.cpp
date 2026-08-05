#include <board/get_calibration.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a GetCalibration message.
 *
 * @param[in]   message     Message to parse
 */

GetCalibration::GetCalibration(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new GetCalibration message.
 *
 * @param[in]   request_id  Request identifier
 */

GetCalibration::GetCalibration(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t GetCalibration::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_GetCalibrationRequest& GetCalibration::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void GetCalibration::pack()
{
    if (whad_board_get_calibration(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void GetCalibration::unpack()
{
    if (whad_board_get_calibration_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
