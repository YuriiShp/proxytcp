#ifndef PROXYTCP_CONVERSATIONPIPELINE_HPP
#define PROXYTCP_CONVERSATIONPIPELINE_HPP

#include <Utilities/SYNACKData.hpp>
#include "src/ConversationFlow/ConversationFlow.hpp"
#include "src/ConversationFlow/SOCKS5Flow/ClientHelloTransmission.hpp"
#include "src/ConversationPipeline/PayloadBuffer/PayloadBuffer.hpp"
#include "src/Connection/SocketConnection.hpp"
#include "src/Connection/SocketCapturingConnection.hpp"
#include "src/PCAP/PCAPCapturingFile.hpp"

namespace Proxy
{
    class ConversationManager;

    class ConversationPipeline : public std::enable_shared_from_this<ConversationPipeline>
    {
    public:
        ~ConversationPipeline() = default;

        ConversationPipeline(int32_t epollfd, ConversationManager& conversationManager) noexcept;
        ConversationPipeline(int32_t epollfd, std::unique_ptr<ConversationFlow> flow, ConversationManager& conversationManager) noexcept;

        ConversationPipeline(ConversationPipeline&& rhs) noexcept = delete;
        ConversationPipeline& operator=(ConversationPipeline&& rhs) noexcept = delete;

    public:
        template<typename PayloadType>
        PayloadType& InitPayloadAs()
        {
            m_payload = std::make_unique<PayloadType>();
            return *static_cast<PayloadType*>(m_payload.get());
        }

        template<typename PayloadType>
        PayloadType& PayloadAs()
        {
            return *static_cast<PayloadType*>(m_payload.get());
        }

        void PerformTransaction(int32_t sockfdWithEvent) noexcept;
        void InitServerConnection(int32_t sockfd) noexcept;
        void InitClientConnection(int32_t sockfd) noexcept;
        void OpenPCAPFile(const std::string& filename) noexcept;

        int32_t GetEpollfd()      const noexcept;
        int32_t GetClientSockfd() const noexcept;
        int32_t GetServerSockfd() const noexcept;
        ConversationFlow::FlowState GetConversationFlowState() const noexcept;

        bool IsClientConnectionInitialized() const noexcept;
        bool IsServerConnectionInitialized() const noexcept;

        ConversationManager& PipelineManager() noexcept;
        PCAP::PCAPCapturingFile& PCAPFile() noexcept;
        bool PayloadIsInitialized() const noexcept;
        SYNACKData& ClientSYNACK() noexcept { return m_clientSYNACKData; };
        SYNACKData& ServerSYNACK() noexcept { return m_serverSYNACKData; };

    private:
        std::unique_ptr<SocketConnection> m_clientConnection;
        std::unique_ptr<SocketConnection> m_serverConnection;
        std::unique_ptr<ConversationFlow> m_conversationFlow;
        std::unique_ptr<PayloadBuffer> m_payload;
        ConversationManager& m_conversationManager;
        int32_t m_epollfd;

        std::unique_ptr<PCAP::PCAPCapturingFile> m_pcapfile;
        SYNACKData m_clientSYNACKData { 1, 1 };
        SYNACKData m_serverSYNACKData { 1, 1 };

    };

}




#endif //PROXYTCP_CONVERSATIONPIPELINE_HPP
