#include <cstdlib>
#include <string.h>
#include "cpp/message.hpp"
#include <whad.h>

#ifdef BOARD_CLUE
#include "messagePool.h"
#endif

/**
 * @brief   Nanopb message wrapper constructor.
 **/

whad::NanoPbMsg::NanoPbMsg(void) : m_ownsMessage(true)
{
#ifdef BOARD_CLUE
    this->p_nanopbMessage = messagePoolAllocateMessage(NULL);
#else
    this->p_nanopbMessage = (Message*)calloc(1u, sizeof(Message));
#endif
}


whad::NanoPbMsg::NanoPbMsg(Message *message) : m_ownsMessage(false)
{
    this->p_nanopbMessage = message;
}


/**
 * @brief   NanoPbMsg destructor.
 **/

whad::NanoPbMsg::~NanoPbMsg(void)
{
    if (m_ownsMessage && this->p_nanopbMessage != NULL)
    {
#ifdef BOARD_CLUE
        messagePoolReleaseMessage(this->p_nanopbMessage);
#else
        free(this->p_nanopbMessage);
#endif
    }
}


/**
 * @brief   Convert the message into its raw representation.
 * 
 * @return  Pointer to a Nanopb Message structure 
 **/

Message *whad::NanoPbMsg::getRaw(void)
{
    /* Call the pack() callback method. */
    this->pack();

    /* Return our NanoPb message. */
    return this->getMessage();
}


/**
 * @brief   Retrieve a pointer on the underlying (wrapped) Nanopb message.
 * 
 * @return  Pointer to a Nanopb Message structure 
 **/

Message *whad::NanoPbMsg::getMessage(void)
{
    return this->p_nanopbMessage;
}


/**
 * @brief   Get message type.
 * 
 * @return  Message type.
 **/

whad::MessageType whad::NanoPbMsg::getType(void)
{
    whad::MessageType msgType = UnknownMsg;

    switch (whad_get_message_type(this->p_nanopbMessage))
    {
        case WHAD_MSGTYPE_GENERIC:
            msgType = GenericMsg;
            break;

        case WHAD_MSGTYPE_DISCOVERY:
            msgType = DiscoveryMsg;
            break;

        case WHAD_MSGTYPE_DOMAIN:
            msgType = DomainMsg;
            break;

        default:
            break;
    }

    /* Return message type. */
    return msgType;
}


/**
 * @brief   Get message domain.
 * 
 * @return  Message domain.
 **/

whad::MessageDomain whad::NanoPbMsg::getDomain(void)
{
    whad::MessageDomain domain = DomainNone;

    if (this->getType() == whad::DomainMsg)
    {
        switch (whad_get_message_domain(this->p_nanopbMessage))
        {
            case DOMAIN_BTLE:
                domain = DomainBle;
                break;

            case DOMAIN_ESB:
                domain = DomainEsb;
                break;

            case DOMAIN_PHY:
                domain = DomainPhy;
                break;

            case DOMAIN_LOGITECH_UNIFYING:
                domain = DomainUnifying;
                break;

            case DOMAIN_DOT15D4:
                domain = DomainDot15d4;
                break;

            case DOMAIN_BOARD:
                domain = DomainBoard;
                break;

            default:
                break;
        }
    }

    /* Return domain. */
    return domain;
}

void whad::NanoPbMsg::unpack()
{
}

void whad::NanoPbMsg::pack()
{
}
