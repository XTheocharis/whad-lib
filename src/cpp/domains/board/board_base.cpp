#include <board/base.hpp>

namespace whad::board {

BoardMsg::BoardMsg() : NanoPbMsg()
{
}

BoardMsg::BoardMsg(NanoPbMsg &pMessage) : NanoPbMsg(pMessage.getRaw())
{
}

BoardMsg::~BoardMsg()
{
}

MessageType BoardMsg::getType(void)
{
    MessageType msgType = (MessageType)whad_board_get_message_type(
        this->getRaw()
    );

    return msgType;
}

}
