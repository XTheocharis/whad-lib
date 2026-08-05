#include <board/read_sensor.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a ReadSensor message.
 *
 * @param[in]   message     Message to parse
 */

ReadSensor::ReadSensor(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new ReadSensor message.
 *
 * @param[in]   request_id  Request identifier
 */

ReadSensor::ReadSensor(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t ReadSensor::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_ReadSensorRequest& ReadSensor::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void ReadSensor::pack()
{
    if (whad_board_read_sensor(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void ReadSensor::unpack()
{
    if (whad_board_read_sensor_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
