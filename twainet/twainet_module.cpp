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

    Ipc__IPCName msgpath;
    Ipc__IPCName* ppath = &msgpath;
    msgpath.host_name = (char*)path.GetHostName().c_str();
    msgpath.module_name = (char*)path.GetModuleName().c_str();
    msgpath.conn_id = (char*)path.GetConnId().c_str();
    
    IPCProtoMessage sendMsg;
    sendMsg.GetMessage()->message_name = (char*)message.GetName();
    sendMsg.GetMessage()->ipc_path = &ppath;
    sendMsg.GetMessage()->n_ipc_path = 1;
    sendMsg.GetMessage()->message.data = (uint8_t*)data;
    sendMsg.GetMessage()->message.len = len;

    IPCSignalMessage msgSignal(sendMsg);
    SendMsg(msgSignal);
    
    free(data);
}

void TwainetModule::OnMessage(const String& messageName, const twnstd::vector<String>& path, const char* data, unsigned int lenData)
{
    if(m_terminal) {
        m_terminal->onData(messageName, data, lenData);
    }
}
