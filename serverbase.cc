#include "serverbase.h"

ServerType get_server_typebyname(const std::string &servername)
{
    static std::unordered_map<std::string, ServerType> umap_ServerNameToType = {
        {"gateserver", ServerType::gateserver},
        {"gameserver", ServerType::gameserver},
        {"controlserver", ServerType::controlserver},
        {"dbserver", ServerType::dbserver}};
    if (umap_ServerNameToType.count(servername))
    {
        return umap_ServerNameToType[servername];
    }
    else
    {
        return ServerType::none;
    }
}
std::string get_server_namebytype(ServerType type)
{
    size_t idx = static_cast<int>(type);
    if (idx < 0 || idx >= ServerTypeNames.size())
    {
        return "unknown";
    }
    return ServerTypeNames[idx];
}

void Header::read(const char *buf)
{
    this->init();
    length = (int)((buf[0] & 0xFF) | ((buf[1] & 0xFF) << 8) | ((buf[2] & 0xFF) << 16) | ((buf[3] & 0xFF) << 24));
    value = (int)((buf[4] & 0xFF) | ((buf[5] & 0xFF) << 8) | ((buf[6] & 0xFF) << 16) | ((buf[7] & 0xFF) << 24));
    type = buf[8];
}

void Header::write(char *buf)
{
    for (int i = 0; i < 4; i++)
    {
        buf[i] = (length >> (i * 8)) & 0xFF;
    }
    for (int i = 0; i < 4; i++)
    {
        buf[4 + i] = (value >> (i * 8)) & 0xFF;
    }
    buf[8] = type;
}
void Header::init()
{
    length = 0;
    value = 0;
    type = 0;
}

int NetworkManager::open_as_server(int port)
{
    sockaddr_in server_addr;
    int listenfd = Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    int flag = 1;
    Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    Bind(listenfd, (sockaddr *)&server_addr, sizeof(server_addr));
    my_log("bind success");
    Listen(listenfd, SOMAXCONN);
    my_log("listen success");
    return listenfd;
}
int NetworkManager::connect_as_client(const ServerInfo &serverinfo)
{
    int fd = Socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(serverinfo.port);
    Inet_pton(AF_INET, serverinfo.ip.c_str(), &addr.sin_addr);
    if (Connect(fd, (sockaddr *)&addr, sizeof(addr)))
    {
        fd_type[fd] = serverinfo.type;
        serverfd[serverinfo.type] = fd;
        // my_log()
        return fd;
    }
    else
    {
        Close(fd);
        return -1;
    }
}
void NetworkManager::set_fdtype(int fd, const ServerType &type)
{
    if (static_cast<int>(type) >= serverfd.size())
    {
        return;
    }
    serverfd[type] = fd;
    fd_type[fd] = type;
}
int NetworkManager::accpect_client(int listenfd)
{
    sockaddr_in client_addr;
    socklen_t len;
    my_log("try accpect one");
    int connfd = Accept(listenfd, (sockaddr *)&client_addr, &len);
    if (connfd != -1)
    {
        my_log("accpect", inet_ntoa(client_addr.sin_addr), ":", ntohs(client_addr.sin_port), "success");
    }
    else
    {
        my_log("accpect client error");
    }
    return connfd;
}

void NetworkManager::close_fd(int fd)
{
    if (fd_type.count(fd))
    {
        auto type = fd_type[fd];
        serverfd[type] = -1;
        fd_type.erase(fd);
    }
    Close(fd);
}
int NetworkManager::request_serverfd(const ServerType &type)
{
    if (static_cast<int>(type) >= serverfd.size())
    {
        return -1;
    }
    return serverfd[type];
}
ServerType NetworkManager::request_fdtype(int fd)
{
    if (fd_type.count(fd))
    {
        return fd_type[fd];
    }
    else
    {
        return ServerType::none;
    }
}

void ServerBase::on_accept()
{
    my_log("on_accept");
    int connfd = net_mgr->accpect_client(listenfd);
    if (connfd != -1)
    {
        uset_clientfd.insert(connfd);
        epoll_mgr->add(connfd, EPOLLIN);
        map_fd_cbufheader[connfd] = {new CircularBuffer(PKGSIZE), new Header(0)};
    }
}
void ServerBase::on_message(int fd, const char *buf, int len)
{
    my_log("on_message", fd, len);
    if (map_fd_cbufheader.count(fd))
    {
        auto cbufheader = map_fd_cbufheader[fd];
        auto cbuf = cbufheader.first;
        auto header = cbufheader.second;
        if (!cbuf || !header)
        {
            my_log("cbuf or header is null");
            return;
        }
        int count = 0;
        while (count < len)
        {
            auto take = std::min(len - count, (int)cbuf->getRemain());
            ///   my_log(take, count);
            cbuf->addBuffer(buf + count, take);
            try_solve(fd, cbuf, header); // 将buf不断压入cbuf，如果cbuf的容量满足处理条件就处理
            count += take;
        }
    }
    else
    {
        my_log("solve message no cbuf");
    }
}
void ServerBase::solve_pkg(int fd, Header *header)
{
    if (header->type == CONNECTSSERVER)
    {
        my_log(header->length, header->type);
        serverproto::ConnectServer cs;
        if (cs.ParseFromArray(pkg_buf + Header::header_length, header->length))
        {
            ServerType servertype = get_server_typebyname(cs.servername());
            if (servertype != ServerType::none)
            {
                net_mgr->set_fdtype(fd, servertype);
                my_log(cs.servername(), "connect");
            }
            else
            {
                my_log("servername is unknow", cs.servername());
            }
        }
        else
        {
            my_log("ConnectServer Parser error");
        }
        return;
    }
}
void ServerBase::on_server_close(ServerType type)
{
}
void ServerBase::on_close(int fd)
{
    my_log("base close");
    epoll_mgr->del(fd);
    auto it = map_fd_cbufheader.find(fd);
    if (it != map_fd_cbufheader.end())
    { // cbuf header 删除
        delete it->second.first;
        delete it->second.second;
        map_fd_cbufheader.erase(it);
    }
    if (uset_clientfd.count(fd))
    {
        uset_clientfd.erase(fd);
    }
    ServerType type = net_mgr->request_fdtype(fd);
    my_log("on close", fd, get_server_namebytype(type));
    if (type != ServerType::none)
    {
        on_server_close(type); // 其他server断开时间处理
    }
    if (type != ServerType::none && map_type_info.count(type))
    { // 作为客户端连接的断开需要重连
        list_reconnect.push_back(type);
    }
    net_mgr->close_fd(fd);
}
void ServerBase::try_solve(int fd, CircularBuffer *cbuf, Header *header)
{
    static uint32_t count = 0; // 需要空读的长度
    while (1)
    {
        auto len = header->length;
        auto type = header->type;
        if (count > 0 && cbuf->getCapacity() > 0)
        { // buf中有数据，能空读
            uint32_t read_size = std::min(count, cbuf->getCapacity());
            count -= read_size;
            cbuf->popBuffer(read_size);
            if (count == 0)
            { // 空读完成， header设置位等待读取状态
                header->init();
            }
        }
        else if (cbuf->getCapacity() >= (type ? len : Header::header_length))
        { // 正常处理包头 包体
            if (type == 0)
            {
                //    my_log("read header");
                cbuf->getBuffer(pkg_buf, Header::header_length);
                header->read(pkg_buf); // 读取下个包的header
                my_log("read header", header->length, header->value, (int)header->type);
                cbuf->popBuffer(Header::header_length);
                if (header->length + Header::header_length > PKGSIZE)
                { // 包体长度超过上限空读，不处理
                    my_log("包体的长度过长,空读", getTypeName(header->type), header->length);
                    count = header->length;
                }
            }
            else
            {
                my_log("read pkg", "type:", getTypeName(type), "len:", len);
                cbuf->getBuffer(pkg_buf + Header::header_length, len);
                solve_pkg(fd, header); // 处理pb协议包
                cbuf->popBuffer(len);
                header->init(); // header设置位等待读取状态
            }
        }
        else
        { // 无能做动作
            break;
        }
    }
}
void ServerBase::send_to_server(ServerType type, const char *buf, int len)
{
    int serverfd = net_mgr->request_serverfd(type);
    if (serverfd != -1)
    {
        Send(serverfd, buf, len, 0);
    }
    else
    {
        my_log("request error:", ServerTypeNames[type], "close");
    }
}
void ServerBase::send_to_server(ServerType type, const Header *header, char *buf)
{
    int serverfd = net_mgr->request_serverfd(type);
    if (serverfd != -1)
    {
        Send(serverfd, buf, header->length + Header::header_length, 0);
    }
    else
    {
        my_log("request error:", ServerTypeNames[type], "close");
    }
}
bool ServerBase::connect_as_client(ServerInfo &info, bool reconnect)
{
    int fd = net_mgr->connect_as_client(info);
    if (!reconnect)
    {
        map_type_info[info.type] = info;
        if (fd == -1)
            list_reconnect.push_back(info.type);
    }
    if (fd != -1)
    {
        map_fd_cbufheader[fd] = {new CircularBuffer(PKGSIZE), new Header(0)};
        auto connmsg = ServerMessageBuilder::createConnectServerMessage(get_server_namebytype(selftype));
        Header hd(connmsg.ByteSizeLong(), 0, CONNECTSSERVER);
        if (SerializeWithHeader(&hd, connmsg, pkg_buf, PKGSIZE))
        {
            my_log("send to ", hd.length, getTypeName(hd.type));
            Send(fd, pkg_buf, hd.length + Header::header_length, 0);
            epoll_mgr->add(fd, EPOLLIN);
        }
        else
        {
            my_log("connect success but send Serialize Error");
        }
    }
    my_log(reconnect ? "reconnect" : "connect", get_server_namebytype(info.type), fd != -1 ? "success" : "fail");
    return fd != -1;
}
void ServerBase::open_as_server(int port)
{
    listenfd = net_mgr->open_as_server(port);
    epoll_mgr->add(listenfd, EPOLLIN);
}
void ServerBase::try_reconnect()
{
    if (list_reconnect.empty())
        return;
    static std::chrono::steady_clock::time_point timePoint = std::chrono::steady_clock::now();
    if (std::chrono::steady_clock::now() < timePoint)
        return;
    timePoint = std::chrono::steady_clock::now() + std::chrono::seconds(30);
    my_log("try reconnect");
    for (auto it = list_reconnect.begin(); it != list_reconnect.end();)
    {
        if (map_type_info.count(*it))
        {
            auto info = map_type_info[*it];
            if (connect_as_client(info, true))
            {
                auto del = it++;
                list_reconnect.erase(del);
                continue;
            }
        }
        ++it;
    }
    my_log("list_reconnect.size() :", list_reconnect.size());
}
void ServerBase::epoll_step(int timeout)
{
    static char buf[BUFFSIZE];
    static epoll_event events[MAXEVENTS];
    int nfds = epoll_mgr->wait(events, MAXEVENTS, timeout);
    if (nfds > 0)
    {
        my_log("nfds:", nfds);
    }
    for (int i = 0; i < nfds; ++i)
    {
        int fd = events[i].data.fd;
        ///  my_log(fd, listenfd);
        auto ets = events[i].events;
        if (fd == listenfd)
        {
            on_accept();
        }
        else if (ets & (EPOLLHUP | EPOLLERR))
        {
            my_log("EPOLLHUP | EPOLLERR", ets & EPOLLHUP, ets & EPOLLERR);
            on_close(events[i].data.fd);
        }
        else if (ets & EPOLLIN)
        {
            int n = Recv(fd, buf, BUFFSIZE, 0);
            if (n > 0)
            {
                on_message(fd, buf, n);
            }
            else
            {
                on_close(fd);
            }
        }
        else
        {
            my_log("epoll", fd, ets);
        }
    }
}
void ServerBase::run()
{
    while (1)
    {
        epoll_step(-1);
    }
}
