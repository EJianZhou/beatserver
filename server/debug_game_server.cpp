#include "mysocket.h"
#include "tool/circularbuffer.h"
#include "tool/pbhelper.h"
#include <map>
#include <algorithm>
#include "config.h"


time_t getTimeMS() {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return now_ms.time_since_epoch().count();
}

void log() { std::cerr << std::endl; }
template<typename T, typename... Args>void log(T x , Args... args) {
	std::cerr << x << " ";
	log(args...);
}


void mergeOperations(beatsgame::ROperation &src,const beatsgame::Operation& ops) {
    for (const auto& clientOp : ops.client_operation()) {
        my_log(clientOp.movex(), clientOp.movey());
        *src.add_operations() = clientOp;
    }
}

class Header { 

public:

    static const size_t header_length = 9;
    Header(size_t length=0, uint8_t type=0) : length(length), type(type) {}
    void read(const char* buf);
    void write(char* buf);
    void init() {
        length = 0;
        type = 0;
    }
    int length;
    int id;
    uint8_t type;

};

class GameServer {

private:
    int listenfd, epollfd;
    epoll_event events[MAXEVENTS], ev;
    std::map<int, CircularBuffer*> connect_buf;
    std::map<int, Header> need_header;
    std::map<int, std::vector<beatsgame::Operation>> op_list; 
    std::map<int, int> op_id;
    char buf[BUFFSIZE], pkg_buf[PKGSIZE];
    sockaddr_in server_addr;
    int control_fd;

public:
    GameServer(){
        control_fd = -1;
        listenfd = -1;
        epollfd = -1;
        connect_buf.clear();
        need_header.clear();

    }
    ~GameServer() {
        close_all_client();
    }
    void close_all_client() {
        for (auto &p : connect_buf) {
            close(p.first);
        }
        need_header.clear();
        connect_buf.clear();
    }
    void close_one_client(int fd) {
        my_log("close", get_ip_from_fd(fd));
        close(fd);
        need_header.erase(fd);
        connect_buf.erase(fd);
        op_id.erase(fd);
    }
    void start_listen(int port) {
        listenfd = Socket(AF_INET, SOCK_STREAM, 0);
        setnonblocking(listenfd);
        int flag = 1;
        Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(port);
        Bind(listenfd, (sockaddr *)&server_addr, sizeof(server_addr));
        my_log("bind success");
        Listen(listenfd, LISTENQ);
        my_log("listen success");
    }
    void solve_pkg(int fd, uint8_t type, int len) {
        // buf 里的数据可能还没处理完，不能使用
        if (type == OPERATION) {
            beatsgame::Operation op;

            if (!op.ParseFromArray(pkg_buf, len)) {
                my_log("parse error");
                for (int i = 0; i < len; i++) {
                    std::cout << (int)pkg_buf[i] << " ";
                }
                std::cout << std::endl;
            }
            my_log("OP", len, op.frame(), op.client_operation_size());
            // for (auto x : op.client_operation()) {
            //     my_log("recv op", x.movex(), x.movey());
            // }
            op_list[0].push_back(op);
        } else if (type == LOGIN) {
            my_log("try login");
            auto rlogin = RMessageBuilder::createRLoginMessage(0, 1);
            Header hd(rlogin.ByteSizeLong(), RLOGIN);
            hd.write(pkg_buf);
            rlogin.SerializePartialToArray(pkg_buf + 4, rlogin.ByteSizeLong());
            Send(fd, pkg_buf, 4 + hd.length, 0);

            my_log("try enter");
            int id = connect_buf.size();
            auto renter = RMessageBuilder::createREnterMessage(id, 633);
            
            Header hd2(renter.ByteSizeLong() ,RENTER);
            hd2.write(pkg_buf);
            renter.SerializeToArray(pkg_buf + 4, renter.ByteSizeLong());
            Send(fd, pkg_buf, renter.ByteSizeLong() + 4, 0);
            my_log("end");
            op_id[fd] = 0;
        }
    }
    void try_solve(int fd, CircularBuffer* cbuf) {
        auto &hd = need_header[fd];
        while (1) {
            int sz = hd.length;
            my_log("cbuf->getCapacity()", cbuf->getCapacity(), "sz", sz);
            if (cbuf->getCapacity() >= (sz ? sz : 4)) {
                if (sz == 0) {
                    cbuf->getBuffer(pkg_buf, 4);
                    hd.read(pkg_buf); // 读取下个包的header
                    cbuf->popBuffer(4);
                } else {
                    my_log("solve_pkg", hd.type);
                    cbuf->getBuffer(pkg_buf, sz);
                    solve_pkg(fd, hd.type, sz); // +4 连包头一起发
                    cbuf->popBuffer(sz);
                    hd.set_value(0); // header设置位等待读取状态
                }
            } else {
                break;
            }
        }
    }
    void push_buf(int fd, uint32_t len) {
        auto cbuf = connect_buf[fd];
        uint32_t count = 0;
        while (count < len) { 
            auto take = std::min(len - count, cbuf->getRemain());
            my_log(take, count);
            cbuf->addBuffer(buf + count, take);
            try_solve(fd, cbuf); // 将buf不断压入cbuf，如果cbuf的容量满足处理条件就处理
            count += take;
        }
    }
    int connect_one() {
        sockaddr_in client_addr;
        socklen_t len;
        my_log("try connect one");
        int connfd = Accept(listenfd, (sockaddr *)&client_addr, &len);
        connect_buf[connfd] = new CircularBuffer(PKGSIZE);
        my_log("connect", inet_ntoa(client_addr.sin_addr) , ":", ntohs(client_addr.sin_port) , "success");
        return connfd;
    }
    void do_list(int delta) {
       // my_log("do_list", delta, op_id.size());
        for (auto &p : op_id) {
            int fd = p.first;
            int &x = p.second;
          //  my_log("try send", x, delta);
    
            beatsgame::ROperation rop;
            bool flag = (x < op_list[0].size());
            while (x < op_list[0].size()) {
                mergeOperations(rop, op_list[0][x++]);
            }
            if (flag) {
                my_log("try send", rop.ByteSizeLong());
            }
            rop.set_deltatime(20);
            Header hd(rop.ByteSizeLong(), ROPERATION);
            hd.write(pkg_buf);
            rop.SerializeToArray(pkg_buf + 4, rop.ByteSizeLong());
            Send(fd, pkg_buf, rop.ByteSizeLong() + 4, 0);
            my_log("try send end", x, rop.ByteSizeLong() + 4);
        }
    }
    void run(int step) {
        epollfd = Epoll_create(1);
        epoll_event ev;
        ev.data.fd = listenfd;
        ev.events = EPOLLIN;
        Epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
        my_log("epoll create success");
        auto last = getTimeMS();
        while (1) {
            auto now = getTimeMS();
            if (now - last >= step) {
               // my_log("delta", now - last);
                do_list(now -last);
                last = now;
                
            }

            int nfds = Epoll_wait(epollfd, events, MAXEVENTS, 0);
         //   my_log(nfds, "events");
            for (int i = 0; i < nfds; ++i) {
                if (events[i].data.fd == listenfd) {
                    int connfd = connect_one();
                    op_id[connfd] = 0;
                    my_log(connfd, "connect cnt:", connect_buf.size());
                    // int id = connect_buf.size();
                    // auto renter = RMessageBuilder::createREnterMessage(id, 633);
                    
                    // Header hd(renter.ByteSizeLong() ,RENTER);
                    // hd.write(pkg_buf);
                    // renter.SerializeToArray(pkg_buf + 4, renter.ByteSizeLong());
                    // Send(connfd, pkg_buf, renter.ByteSizeLong() + 4, 0);
                    ev.events = EPOLLIN | EPOLLHUP;
                    ev.data.fd = connfd;
                    Epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev);
                } else if (events[i].events & EPOLLHUP) {
                    int fd = events[i].data.fd;
                    ev.events = EPOLLIN | EPOLLHUP;
                    ev.data.fd = fd;
                    
                    Epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
                    close_one_client(fd);
                    if (connect_buf.size() == 0) {
                        op_list[0].clear();
                    }
                } else {
                    int fd = events[i].data.fd;
                    int n = Recv(fd, buf, BUFFSIZE, 0);
                    my_log("read", n);	
                    if (n > 0) {
                        push_buf(fd, n);
                    } else {
                        ev.events = EPOLLIN | EPOLLHUP;
                        ev.data.fd = fd;
                        op_id[fd] = 0;
                        Epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
                        close_one_client(fd);
                        if (connect_buf.size() == 0) {
                            op_list[0].clear();
                        }
                    }
                }
            }
        }
    }
};

int main (int argc, char *argv[]) {
	if (argc != 2) {
		my_log("example: ./server <TIME_STEP>");
		exit(1);	
	}
	int step;
	try {
		step = std::stoi(argv[1]);
	}  catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
		exit(1);
    }
    GameServer gameserver;
    gameserver.start_listen(DGAME_PORT);
    gameserver.run(step);
}