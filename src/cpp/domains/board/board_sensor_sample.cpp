#include <board/sensor_sample.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a SensorSample message.
 *
 * @param[in]   message     Message to parse
 */

SensorSample::SensorSample(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new SensorSample message.
 *
 * @param[in]   request_id  Request identifier
 */

SensorSample::SensorSample(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t SensorSample::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_SensorSample& SensorSample::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void SensorSample::pack()
{
    if (whad_board_sensor_sample(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void SensorSample::unpack()
{
    if (whad_board_sensor_sample_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
