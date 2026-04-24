#include "board_communicate.h"
#include "udp/udp_async_client.h"
#include "tcp/tcp_async_client.h"

std::shared_ptr<BoardCommunicate> BoardCommunicate::Create(NetChannel channel, std::shared_ptr<io_context> ioContext, int localPort)
{
    switch (channel)
    {
        case NetChannel::UDP:
            return std::make_shared<UdpASyncClient>(ioContext, localPort);
        case NetChannel::TCP:
            return std::make_shared<TcpASyncClient>(ioContext, localPort);
        default:
            return nullptr;
    }
}

int64_t BoardCommunicate::GetCurrentTimeMs() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}
