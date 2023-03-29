#include "mysocket.h"
#include "serverbase.h"
#include "tool/circularbuffer.h"
#include "tool/pbhelper.h"
#include "tool/objectpool.h"
#include <list>
#include <random>
#include <unordered_map>
#include <unordered_set>


std::mt19937 rng(std::random_device{}());



enum PlayerState {
    login,
    match,
    game,
    exitgame
};
struct PlayerInfo {
    PlayerState state;
    std::string username;
    std::list<int>::iterator listit_match;
    uint32_t seed;
    int gameid;
};


class IDManager {
private:
    std::unordered_map<std::string, int> umap_username2id;
    std::unordered_multiset<int> uset_id;
    uint32_t generateID() {
        std::uniform_int_distribution<uint32_t> dist(1);
        return dist(rng);
    }
public:
    int getID(const std::string& username) {
        auto it = umap_username2id.find(username);
        if (it != umap_username2id.end()) {
            return it->second;
        }
        int id;
        do {
            id = generateID() % INT32_MAX;
        } while(uset_id.count(id));
        umap_username2id[username] = id;
        return id;
    }
    void erase(int id, const std::string& username) {
        auto it_umap = umap_username2id.find(username);
        auto it_uset = uset_id.find(id);
        if (it_umap != umap_username2id.end() && it_uset != uset_id.end() && it_umap->second == id) {
            umap_username2id.erase(it_umap);
            uset_id.erase(it_uset);
        } else {
            my_log("IDMgr erase fail", "username:", username, "id:", id);
        }
    }
};





class ControlServer: public ServerBase {

public:
    // void on_close(int fd) override {
    //     ServerBase::on_close(fd);
    // }
    ControlServer() : ServerBase(ServerType::gateserver) {
        pool_playerinfo = new ObjectPool<PlayerInfo>(256);
        IDMgr = new IDManager();
    }
    ~ControlServer() {
        delete pool_playerinfo;
        delete IDMgr;
    }
    void try_match() {
        if (list_match.size() >= 2) {
            int player1_id = list_match.front();
            list_match.pop_front();
            int player2_id = list_match.front();
            list_match.pop_front();
            auto seed = rng();
            //向两个玩家发送REnter包
            auto rm1 = RMessageBuilder::createREnterMessage(1, seed);
            auto rm2 = RMessageBuilder::createREnterMessage(2, seed);
            Header header1(rm1.ByteSizeLong(), player1_id, RENTER);
            Header header2(rm2.ByteSizeLong(), player2_id, RENTER);
            if (SerializeWithHeader(&header1, rm1, pkg_buf, PKGSIZE)) {
                send_to_server(ServerType::gateserver, &header1, pkg_buf);
            } else {
                my_log("RMatch SerializeWithHeader fail");
            }
            if (SerializeWithHeader(&header2, rm2, pkg_buf, PKGSIZE)) {
                send_to_server(ServerType::gateserver, &header2, pkg_buf);
            } else {
                my_log("RMatch SerializeWithHeader fail");
            }
            //player 状态更新
            PlayerInfo *info = umap_id2playerinfo[player1_id];
            info->gameid = 1;
            info->seed = seed;
            info->state = PlayerState::game;
            info = umap_id2playerinfo[player2_id];
            info->gameid = 2;
            info->seed = seed;
            info->state = PlayerState::game;
            //创建房间
            auto cr = ServerMessageBuilder::createCreateRoomMessage(player1_id, player2_id);
            Header hd(cr.ByteSizeLong(), 0, CREATROOM);
            if (SerializeWithHeader(&hd, cr, pkg_buf, PKGSIZE)) {
                send_to_server(ServerType::gameserver, &hd, pkg_buf);
            } else {
                my_log("CreateRoom SerializeWithHeader fail");
            }
        }
    }
    void try_logout(int id) {
        my_log("logout", id);
        auto it = umap_id2playerinfo.find(id);
        if (it != umap_id2playerinfo.end()) {
            PlayerInfo* info = it->second;
            if (info->state == PlayerState::login) {
                my_log("已登录用户登出");
                pool_playerinfo->release(info);

                IDMgr->erase(id, info->username);
                umap_id2playerinfo.erase(it);
            } else if (info->state == PlayerState::match) {
                my_log("匹配队列中用户退出");
                pool_playerinfo->release(info);

                list_match.erase(info->listit_match);

                IDMgr->erase(id, info->username);
                umap_id2playerinfo.erase(it);
            } else if (info->state == PlayerState::game) {
                my_log("游戏中用户退出");
                info->state = PlayerState::exitgame;

            } else if (info->state == PlayerState::exitgame) {

            }
        }
    }
    bool check_password(const std::string& username, const std::string& password) {
        if (true) {
            return 1;
        } else {
            return 0;
        }
    }
    void try_login(int fd, Header *header) {
        int gate_fd = header->value;
        beatsgame::Login login;
        if (login.ParseFromArray(pkg_buf + Header::header_length, header->length)) {
            auto username = login.username();
            auto password = login.password();
            my_log("login", username, password, !check_password(username, password));
            if (!check_password(username, password))  //校验登录
                return;
            int playerid = IDMgr->getID(username); // 获取ID
            my_log("playerid", playerid);
            if (umap_id2playerinfo.count(playerid)) {  //已登录 或者 游戏中途退出的用户
                my_log("已登录 或者 游戏中途退出的用户");
                PlayerInfo *info = umap_id2playerinfo[playerid];           
                if (info->state == PlayerState::exitgame) { // 断线重连
                    auto re = RMessageBuilder::createREnterMessage(info->gameid, info->seed);
                    Header hd(re.ByteSizeLong(), playerid, RENTER);
                    if (SerializeWithHeader(&hd, re, pkg_buf, PKGSIZE)) {
                        my_log("player reconnect", info->username);
                        send_to_server(ServerType::gateserver, &hd, pkg_buf);
                    } else {
                        my_log("RLogin SerializeWithHeader fail");
                    }
                } else { //账户已登录 ，登录失败
                    my_log("账户已登录 ，登录失败");
                    auto rl = RMessageBuilder::createRLoginMessage(0, 0); 
                    Header hd(rl.ByteSizeLong(), gate_fd, RLOGIN);
                    if (SerializeWithHeader(&hd, rl, pkg_buf, PKGSIZE)) {
                        my_log("send RLogin fail");
                        send_to_server(ServerType::gateserver, &hd, pkg_buf);
                    } else {
                        my_log("RLogin SerializeWithHeader fail");
                    }
                }
            } else { //新登录的用户
                my_log("新登录的用户");
                PlayerInfo* info = pool_playerinfo->acquire();
                info->state = PlayerState::login;
                info->username = username;
                umap_id2playerinfo[playerid] = info;
                auto rl = RMessageBuilder::createRLoginMessage(playerid, 1);
                Header hd(rl.ByteSizeLong(), gate_fd, RLOGIN);
                if (SerializeWithHeader(&hd, rl, pkg_buf, PKGSIZE)) {
                    my_log("send RLogin");
                    send_to_server(ServerType::gateserver, &hd, pkg_buf);
                } else {
                    my_log("RLogin SerializeWithHeader fail");
                }
            }
        } else {
            my_log("login parse error");
        }    
    }
    void solve_pkg(int fd, Header *header) override {
        ServerBase::solve_pkg(fd, header);
        auto type = header->type;
        if (type == LOGIN) { // 登录
            try_login(fd, header);
        } else if (type == MATCH) { //匹配
            int id = header->value;
            list_match.push_back(id);
            PlayerInfo *info = umap_id2playerinfo[id];
            info->listit_match = --list_match.end();
            info->state = PlayerState::match;
            my_log(info->username, "开始匹配");
            auto rm = RMessageBuilder::createRMatchMessage(1);
            Header hd(rm.ByteSizeLong(), id, RMATCH);
            if (SerializeWithHeader(&hd, rm, pkg_buf, PKGSIZE)) {
                send_to_server(ServerType::gateserver, &hd, pkg_buf);
            }
            try_match();   
        } else if (type == GATELOGOUT) {
            int id = header->value;
            try_logout(id);
        } else {
            my_log("recv pkg type", getTypeName(type));
        }
    }
    void run() override {
        while (1) {
            this->epoll_step(0);
            this->try_reconnect();
        }
    }
private:
    ObjectPool<PlayerInfo> *pool_playerinfo;
    IDManager* IDMgr;
    std::list<int> list_match;
    std::unordered_map<int, PlayerInfo*> umap_id2playerinfo;
};



int main (int argc, char *argv[])  {
    ControlServer controlserver;
    controlserver.open_as_server(CONTROL_PORT);
    controlserver.run();
}