#include "twainet_module.h"
#include "terminal.h"

TwainetModule::TwainetModule(const IPCObjectName& ipcName, ConnectorFactory* factory)
: ClientModule(ipcName, factory), m_terminal(0)
{
}

TwainetModule::~TwainetModule()
{
}

void TwainetModule::SetTerminal(Terminal* terminal)
{
    m_terminal = terminal;
}

void TwainetModule::toMessage(const DataMessage& message, const IPCObjectName& path)
{
    int len = 0;
    message.deserialize(0, len);
    char* data = (char*)malloc(len);
    message.deserialize(data, len);

    IPCNameMessage msgpath;
    Ipc__IPCName* ppath = msgpath.GetMessage();
    msgpath.GetMessage()->host_name = (char*)path.GetHostName().c_str();
    msgpath.GetMessage()->module_name = (char*)path.GetModuleName().c_str();
    msgpath.GetMessage()->conn_id = (char*)path.GetConnId().c_str();
    
    IPCProtoMessage sendMsg;
    sendMsg.GetMessage()->message_name = (char*)message.GetName();
    sendMsg.GetMessage()->ipc_path = &ppath;
    sendMsg.GetMessage()->n_ipc_path = 1;
    sendMsg.GetMessage()->ipc_sender = 0;
    sendMsg.GetMessage()->n_ipc_sender = 0;
    sendMsg.GetMessage()->has_message = true;
    sendMsg.GetMessage()->message.data = (uint8_t*)data;
    sendMsg.GetMessage()->message.len = len;

    IPCSignalMessage msgSignal(sendMsg);
    SendMsg(msgSignal);
    
    free(data);
}

void TwainetModule::OnServerConnected()
{
    ClientModule::OnServerConnected();
    m_terminal->onConnected();
}

void TwainetModule::OnMessage(const String& messageName, const twnstd::vector<String>& path, const char* data, unsigned int lenData)
{
//    LOG_INFO("message recv %s, %d", messageName.c_str(), lenData);
    if(m_terminal) {
        m_terminal->onData(messageName, data, lenData);
    }
}
