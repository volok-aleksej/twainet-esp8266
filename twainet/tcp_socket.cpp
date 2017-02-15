#include "tcp_socket.h"
#include "thread_manager.h"

#include <IPAddress.h>
#include <debug.h>
#include <Arduino.h>
#include <lwip/dns.h>
#include <lwip/err.h>

void onError(void* arg, int8_t err)
{
    Serial.print("tcp error");
    Serial.println(err);
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        socket->OnError(err);
    }
}

void dnsFoundCallback(const char *name, ip_addr_t *ipaddr, void *callback_arg)
{
    uint32_t threadId = (uint32_t)(*reinterpret_cast<IPAddress*>(callback_arg));
    if(ipaddr) {
        (*reinterpret_cast<IPAddress*>(callback_arg)) = ipaddr->addr;
    }
    ThreadManager::GetInstance().ResumeThread(threadId);
}

int8_t onTcpConnect(void* arg, tcp_pcb* tpcb, int8_t err)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnConnect(tpcb, err);
    }
    
    return ERR_VAL;
}

int8_t onTCPRecv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnTCPRecv(tpcb, pb, err);
    }
    
    return ERR_VAL;
}

int8_t onTCPSent(void *arg, struct tcp_pcb *tpcb, uint16_t len)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnTCPSent(tpcb, len);
    }
    
    return ERR_VAL;
}

TCPSocket::TCPSocket()
: m_buf_offset(0)
, m_buf(0)
, m_sentSize(0)
, m_suspendedThread(0)
{
	Initialize();
}

TCPSocket::~TCPSocket()
{
	Close();
}

bool TCPSocket::Bind(const String& host, int port)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

    ip_addr_t ipaddr;
	if(host.length() == 0)
	{
        ipaddr = ip_addr_any;
	}
	else
	{
       IPAddress ipaddress(ThreadManager::GetInstance().GetCurrentThreadId());
       if(!ipaddress.fromString(host)) {
          err_t err = dns_gethostbyname(host.c_str(), &ipaddr, &dnsFoundCallback, &ipaddress);
          if(err == ERR_INPROGRESS) {
               ThreadManager::GetInstance().SuspendThread(ThreadManager::GetInstance().GetCurrentThreadId());
               // will return here when dns_found_callback fires
          } else if(err != ERR_OK) {
              return false;
          }
       }
       
       ipaddr.addr = ipaddress;
	}
	
	return tcp_bind(m_socket, &ipaddr, port) == ERR_OK;
}

bool TCPSocket::Connect(const String& host, int port)
{
	if(m_socket == INVALID_SOCKET || host.length() == 0)
	{
		return false;
	}

    ip_addr_t ipaddr;
    if(host.length() == 0)
    {
        ipaddr = ip_addr_any;
    }
    else
    {
        IPAddress ipaddress;
        if(!ipaddress.fromString(host)) {
            err_t err = dns_gethostbyname(host.c_str(), &ipaddr, &dnsFoundCallback, &ipaddress);
            if(err == ERR_INPROGRESS) {
                ThreadManager::GetInstance().SuspendThread(m_suspendedThread);
                // will return here when dns_found_callback fires
            } else if(err != ERR_OK) {
                return false;
            }
        }
        
        ipaddr.addr = ipaddress;
    }
    
	tcp_connect(m_socket, &ipaddr, port, &onTcpConnect);
    m_suspendedThread = ThreadManager::GetInstance().GetCurrentThreadId();
    ThreadManager::GetInstance().SuspendThread(m_suspendedThread);
    return m_lastError == ERR_OK;
}

bool TCPSocket::Send(char* data, int len)
{
	if(m_socket == INVALID_SOCKET || m_sentSize)
	{
		return false;
	}
	
 	m_sentSize = len;
    while(m_sentSize) {
        size_t room = tcp_sndbuf(m_socket);
        size_t will_send = (room < m_sentSize) ? room : m_sentSize;
        err_t err = tcp_write(m_socket, data + len - m_sentSize, will_send, 0);
        if(err != ERR_OK) {
            DEBUGV("TCPSocket:Send !ERR_OK\r\n");
            return false;
        }
        
        tcp_output(m_socket);
        m_suspendedThread = ThreadManager::GetInstance().GetCurrentThreadId();
        ThreadManager::GetInstance().SuspendThread(m_suspendedThread);
    }
	
	return true;
}

bool TCPSocket::Recv(char* data, int len)
{        
    while(len) {
        if(m_socket == INVALID_SOCKET)
        {
            return false;
        }

        if(!m_buf) {
            m_suspendedThread = ThreadManager::GetInstance().GetCurrentThreadId();
            ThreadManager::GetInstance().SuspendThread(m_suspendedThread);
        }
        
        if(m_buf) {
            Serial.printf(":rd %d, %d, %d\r\n", len, m_buf->tot_len, m_buf_offset);
            size_t max_size = m_buf->tot_len - m_buf_offset;
            size_t size = (len < max_size) ? len : max_size;
            size_t buf_size = m_buf->len - m_buf_offset;
            size_t copy_size = (size < buf_size) ? size : buf_size;
            Serial.printf(":rdi %d, %d\r\n", buf_size, copy_size);
            os_memcpy(data, reinterpret_cast<char*>(m_buf->payload) + m_buf_offset, copy_size);
            data += copy_size;
            Consume(copy_size);
            len -= copy_size;
        }
    }
    
	return true;
}
void TCPSocket::Consume(size_t size)
{
    ptrdiff_t left = m_buf->len - m_buf_offset - size;
    if(left > 0) {
        m_buf_offset += size;
    } else if(!m_buf->next) {
        Serial.printf(":c0 %d, %d\r\n", size, m_buf->tot_len);
        if(m_socket) tcp_recved(m_socket, m_buf->len);
        pbuf_free(m_buf);
        m_buf = 0;
        m_buf_offset = 0;
    } else {
        Serial.printf(":c %d, %d, %d\r\n", size, m_buf->len, m_buf->tot_len);
        auto head = m_buf;
        m_buf = m_buf->next;
        m_buf_offset = 0;
        pbuf_ref(m_buf);
        if(m_socket) tcp_recved(m_socket, head->len);
        pbuf_free(head);
    }
}

bool TCPSocket::Close()
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}
	
    tcp_arg(m_socket, NULL);
    tcp_sent(m_socket, NULL);
    tcp_recv(m_socket, NULL);
    tcp_err(m_socket, NULL);
    tcp_close(m_socket);	
    return true;
}

void TCPSocket::GetIPPort(String& ip, int& port)
{
    IPAddress address(m_socket->remote_ip.addr);
    ip = address.toString();
    port = m_socket->remote_port;
}

int TCPSocket::GetSocket()
{
	return *(int*)m_socket;
}

void TCPSocket::Initialize()
{
	m_socket = tcp_new();
    tcp_arg(m_socket, this);
    tcp_err(m_socket, &onError);
}

int TCPSocket::GetMaxBufferSize()
{
	if(m_socket == INVALID_SOCKET)
	{
		return 0;
	}
	
	return m_socket->snd_buf;
}

void TCPSocket::OnError(uint8_t err)
{
    m_lastError = err;
    m_socket = INVALID_SOCKET;
    ThreadManager::GetInstance().ResumeThread(m_suspendedThread);
}

int8_t TCPSocket::OnConnect(tcp_pcb* tpcb, int8_t err)
{
    m_lastError = ERR_OK;
    m_socket = tpcb;
    tcp_setprio(m_socket, TCP_PRIO_MIN);
    tcp_arg(m_socket, this);
    tcp_recv(m_socket, (tcp_recv_fn)&onTCPRecv);
    tcp_sent(m_socket, &onTCPSent);
    tcp_err(m_socket, &onError);
    ThreadManager::GetInstance().ResumeThread(m_suspendedThread);
    return ERR_OK;
}

int8_t TCPSocket::OnTCPRecv(tcp_pcb* tpcb, pbuf* pb, err_t err)
{
    int8_t ret;
    if(!pb) {
        tcp_arg(m_socket, NULL);
        tcp_sent(m_socket, NULL);
        tcp_recv(m_socket, NULL);
        tcp_err(m_socket, NULL);
        tcp_abort(m_socket);
        m_socket = INVALID_SOCKET;
        ret = ERR_ABRT;
    } else {
        m_lastError = err;
        if(m_buf) {
            pbuf_cat(m_buf, pb);
        } else {
            m_buf = pb;
        }
        ret = ERR_OK;
    }
    ThreadManager::GetInstance().ResumeThread(m_suspendedThread);
    return ret;
}

int8_t TCPSocket::OnTCPSent(tcp_pcb* tpcb, uint16_t len)
{
    Serial.print("tcp_s: ");
    Serial.println(len);
    m_lastError = ERR_OK;
    m_sentSize -= len;
    ThreadManager::GetInstance().ResumeThread(m_suspendedThread);
    return ERR_OK;
}