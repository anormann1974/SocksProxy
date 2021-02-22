#include "InitialState.h"

#include <QtDebug>

#include "protocol/Socks5GreetingMessage.h"
#include "protocol/Socks5MethodSelectionMessage.h"

#include "Socks5AuthState.h"
#include "Socks4InitialState.h"


InitialState::InitialState(SocksConnection *parent)
    : SocksState(parent)
    , _timeoutTimer(new QTimer(this))
{
    connect(_timeoutTimer, &QTimer::timeout, parent, &SocksConnection::close);
    _timeoutTimer->start(10 * 1000);
}

void InitialState::handleIncomingBytes(QByteArray &bytes)
{
    if (bytes.isEmpty())
    {
        qWarning() << "handleIncomingBytes got empty byte array";
        return;
    }

    //Try to parse a Socks5GreetingMessage out of the bytes
    QSharedPointer<Socks5GreetingMessage> socks5Greeting(new Socks5GreetingMessage());
    QString errorString;

    if (const auto result = socks5Greeting->parse(bytes, &errorString); result == SocksProtocolMessage::Success)
    {
        //Tell parent that we're SOCKS5
        _parent->setSocksVersion(SocksProtocolMessage::SOCKS5);

        //Transition to Socks5AuthState
        auto state = new Socks5AuthState(socks5Greeting, _parent);
        _parent->setState(state);
    }
    //if that fails, see if Socks4 may be applicable
    else if (bytes.at(0) == 0x04)
    {
        //Tell parent that we're SOCKS4
        _parent->setSocksVersion(SocksProtocolMessage::SOCKS4);

        //Transition to Socks4InitialState
        auto state = new Socks4InitialState(_parent);
        _parent->setState(state);
    }
    //Otherwise, we might just need more bytes to make a SOCKS5 message
    else if (result == SocksProtocolMessage::NotEnoughData)
    {
        //Try again later
    }
    //Something bad has happened
    else
    {
        qWarning() << errorString;
        _parent->close();
    }
}
