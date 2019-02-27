#include "tcp_socket.h"
#include "thread_manager.h"

#include <IPAddress.h>
#include <debug.h>
#include <Arduino.h>
#include <lwip/dns.h>
#include <lwip/err.h>

#define ONCE 0

void onError(void* arg, err_t err)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        socket->OnError(err);
    }
}

void dnsFoundCallback(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    uint32_t threadId = (uint32_t)(*reinterpret_cast<IPAddress*>(callback_arg));
    if(ipaddr) {
        (*reinterpret_cast<IPAddress*>(callback_arg)) = ipaddr->addr;
    }
    ThreadManager::GetInstance().ResumeThread(threadId);
}

err_t onTcpConnect(void* arg, tcp_pcb* tpcb,  err_t err)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnConnect(tpcb, err);
    }
    
    return ERR_VAL;
}

err_t onTCPRecv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnTCPRecv(tpcb, pb, err);
    }
    
    return ERR_VAL;
}

err_t onTCPSent(void *arg, struct tcp_pcb *tpcb, uint16_t len)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnTCPSent(tpcb, len);
    }
    
    return ERR_VAL;
}

void onSendError(void *arg)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnError(ERR_ABRT);
    }
}

TCPSocket::TCPSocket()
: m_buf_offset(0)
, m_buf(0)
, m_sentSize(0)
, m_suspendedThread(-1)
, m_suspendedRecvThread(-1)
, m_suspendedSendThread(-1)
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
                m_suspendedThread = ThreadManager::GetInstance().GetCurrentThreadId();
                ThreadManager::GetInstance().SuspendThread(m_suspendedThread);
            } else if(err != ERR_OK) {
                return false;
            }
        }
        
        ipaddr.addr = ipaddress;
    }
 
    err_t err = tcp_connect(m_socket, &ipaddr, port, &onTcpConnect);
    if(err == ERR_OK || err == ERR_INPROGRESS) {
        m_suspendedThread = ThreadManager::GetInstance().GetCurrentThreadId();
        ThreadManager::GetInstance().SuspendThread(m_suspendedThread);
    }
    return m_lastError == ERR_OK;
}

bool TCPSocket::Send(char* data, int len)
{
    //wait until other thread will send data
    while(m_suspendedSendThread > -1 && m_socket != INVALID_SOCKET) {
        ThreadManager::GetInstance().DelayThread(ThreadManager::GetInstance().GetCurrentThreadId(), 100);
    }
    
    while(len) {
        if(m_socket == INVALID_SOCKET)
        {
            return false;
        }
        
        size_t room = tcp_sndbuf(m_socket);
        size_t will_send = (room < len) ? room : len;
        
        err_t err = tcp_write(m_socket, data, will_send, 0);
        if(err != ERR_OK) {
            return false;
        }
        
        data += will_send;
        len -= will_send;
        m_sentSize = 0;
        tcp_output(m_socket);
        
        os_timer_disarm(&m_sendErrorTimer);
        os_timer_setfn(&m_sendErrorTimer, &onSendError, this);
        os_timer_arm(&m_sendErrorTimer, 1000, ONCE);
        
        // waiting to send tcp data
        while(m_sentSize != will_send && m_socket != INVALID_SOCKET) {
            m_suspendedSendThread = ThreadManager::GetInstance().GetCurrentThreadId();
            ThreadManager::GetInstance().SuspendThread(m_suspendedSendThread);
        }
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
            m_suspendedRecvThread = ThreadManager::GetInstance().GetCurrentThreadId();
            ThreadManager::GetInstance().SuspendThread(m_suspendedRecvThread);
        }
        
        if(m_buf) {
            size_t max_size = m_buf->tot_len - m_buf_offset;
            size_t size = (len < max_size) ? len : max_size;
            size_t buf_size = m_buf->len - m_buf_offset;
            size_t copy_size = (size < buf_size) ? size : buf_size;
            os_memcpy(data, reinterpret_cast<char*>(m_buf->payload) + m_buf_offset, copy_size);
            data += copy_size;
            len -= copy_size;
            Consume(copy_size);
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
        if(m_socket) tcp_recved(m_socket, m_buf->len);
        pbuf_free(m_buf);
        m_buf = 0;
        m_buf_offset = 0;
    } else {
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
    m_socket = INVALID_SOCKET;
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
    if(m_socket) {
        tcp_arg(m_socket, this);
        tcp_err(m_socket, &onError);
    }
}

int TCPSocket::GetMaxBufferSize()
{
	if(m_socket == INVALID_SOCKET)
	{
		return 0;
	}
	
	return m_socket->snd_buf;
}

void TCPSocket::OnError(err_t err)
{
    m_lastError = err;
    tcp_arg(m_socket, NULL);
    tcp_sent(m_socket, NULL);
    tcp_recv(m_socket, NULL);
    tcp_err(m_socket, NULL);
    m_socket = INVALID_SOCKET;
    int thread_id;
    if(m_suspendedThread != -1) {
        thread_id = m_suspendedThread;
        m_suspendedThread = -1;
        ThreadManager::GetInstance().ResumeThread(thread_id);
    } else if(m_suspendedRecvThread != -1) {
        thread_id = m_suspendedRecvThread;
        m_suspendedRecvThread = -1;
        ThreadManager::GetInstance().ResumeThread(thread_id);
    } else if(m_suspendedSendThread != -1) {
        thread_id = m_suspendedSendThread;
        m_suspendedSendThread = -1;
        ThreadManager::GetInstance().ResumeThread(thread_id);
    }
}

err_t TCPSocket::OnConnect(tcp_pcb* tpcb, err_t err)
{
    m_lastError = ERR_OK;
    m_socket = tpcb;
    tcp_setprio(m_socket, TCP_PRIO_MIN);
    tcp_arg(m_socket, this);
    tcp_recv(m_socket, (tcp_recv_fn)&onTCPRecv);
    tcp_sent(m_socket, &onTCPSent);
    tcp_err(m_socket, &onError);
    if(m_suspendedThread != -1) {
        int thread_id = m_suspendedThread;
        m_suspendedThread = -1;
        ThreadManager::GetInstance().ResumeThread(thread_id);
    }
    return ERR_OK;
}

err_t TCPSocket::OnTCPRecv(tcp_pcb* tpcb, pbuf* pb, err_t err)
{
    err_t ret;
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
    if(m_suspendedRecvThread != -1) {
        int thread_id = m_suspendedRecvThread;
        m_suspendedRecvThread = -1;
        ThreadManager::GetInstance().ResumeThread(thread_id);
    }
    return ret;
}

err_t TCPSocket::OnTCPSent(tcp_pcb* tpcb, uint16_t len)
{
    m_lastError = ERR_OK;
    m_sentSize = len;
    os_timer_disarm(&m_sendErrorTimer);
    if(m_suspendedSendThread != -1) {
        int thread_id = m_suspendedSendThread;
        m_suspendedSendThread = -1;
        ThreadManager::GetInstance().ResumeThread(thread_id);
    }
    return ERR_OK;
}
