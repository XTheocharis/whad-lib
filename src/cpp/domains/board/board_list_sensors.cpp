#include <board/list_sensors.hpp>

using namespace whad::board;

/**
 * @brief   Constructor, parse a BoardMsg as a ListSensors message.
 *
 * @param[in]   message     Message to parse
 */

ListSensors::ListSensors(BoardMsg &message) : BoardMsg(message)
{
    this->unpack();
}


/**
 * @brief   Constructor, create a new ListSensors message.
 *
 * @param[in]   request_id  Request identifier
 */

ListSensors::ListSensors(uint32_t request_id) : BoardMsg()
{
    m_requestId = request_id;
    memset(&m_value, 0, sizeof(m_value));
}


/**
 * @brief   Get the request identifier.
 *
 * @retval  Request identifier
 */

uint32_t ListSensors::getRequestId(void)
{
    return m_requestId;
}


/**
 * @brief   Get the underlying value struct.
 *
 * @retval  Reference to the value struct
 */

board_ListSensorsRequest& ListSensors::getValue(void)
{
    return m_value;
}


/**
 * @brief   Pack parameters into a BoardMsg.
 */

void ListSensors::pack()
{
    if (whad_board_list_sensors(this->getMessage(), m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}


/**
 * @brief   Extract parameters from a BoardMsg.
 */

void ListSensors::unpack()
{
    if (whad_board_list_sensors_parse(this->getMessage(), &m_requestId, &m_value) == WHAD_ERROR)
    {
        throw WhadMessageParsingError();
    }
}
