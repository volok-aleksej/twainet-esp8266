#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "any_socket.h"

#include <WString.h>
#include <lwip/netif.h>
#include <lwip/tcp.h>

class TCPSocket : public AnySocket
{
public:
	TCPSocket();
	virtual ~TCPSocket();

	virtual bool Bind(const String& host, int port);
	virtual bool Connect(const String& host, int port);
	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
	virtual void GetIPPort(String& ip, int& port);
	virtual bool Close();
	virtual int GetSocket();
	virtual int GetMaxBufferSize();
protected:
	void Initialize();
    void Consume(size_t size);
protected:
    friend void onError(void* arg, err_t err);
    friend err_t onTcpConnect(void* arg, tcp_pcb* tpcb, err_t err);
    friend err_t onTCPRecv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err);
    friend err_t onTCPSent(void *arg, struct tcp_pcb *tpcb, uint16_t len);
    void OnError(err_t err);
    err_t OnConnect(tcp_pcb* tpcb, err_t err);
    err_t OnTCPRecv(tcp_pcb* tpcb, pbuf* pb, err_t err);
    err_t OnTCPSent(tcp_pcb* tpcb, uint16_t len);
private:
	tcp_pcb* m_socket;
    uint32_t m_sentSize;
    pbuf* m_buf;
    uint32_t m_buf_offset;
    int8_t m_lastError;
    
    int32_t m_suspendedThread;
    int32_t m_suspendedRecvThread;
    int32_t m_suspendedSendThread;
};

#endif/*TCP_SOCKET_H*/
