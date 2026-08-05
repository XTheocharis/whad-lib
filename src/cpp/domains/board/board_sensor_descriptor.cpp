#include <board/sensor_descriptor.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a SensorDescriptor message.
 *
 * @param[in]   message     Message to parse
 */

SensorDescriptor::SensorDescriptor(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new SensorDescriptor message.
 *
 * @param[in]   request_id  Request identifier
 */

SensorDescriptor::SensorDescriptor(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t SensorDescriptor::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_ListSensorsResponse& SensorDescriptor::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void SensorDescriptor::pack()
{
    if (whad_board_sensor_descriptor(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void SensorDescriptor::unpack()
{
    if (whad_board_sensor_descriptor_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
