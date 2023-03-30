#include "mysocket.h"
#include "serverbase.h"
#include "tool/circularbuffer.h"
#include "tool/pbhelper.h"
#include "tool/objectpool.h"
#include <list>
#include <map>
#include <unordered_map>


time_t getTimeMS()
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return now_ms.time_since_epoch().count();
}

void mergeOperations(beatsgame::ROperation &src, const beatsgame::Operation &ops)
{
    for (const auto &clientOp : ops.client_operation())
    {
        my_log(clientOp.movex(), clientOp.movey());
        *src.add_operations() = clientOp;
    }
}
class RoomInfo
{
public:
    int playersize, playercount;
    RoomInfo(int playersize = 2) : playersize(playersize), playercount(0)
    {
        vect_op.reserve(1 << 10);
        vect_playerid.reserve(playersize);
        vect_opid.reserve(playersize);
    }
    void init()
    {
        vect_op.clear();
        vect_opid.clear();
        vect_playerid.clear();
        playercount = 0;
    }
    bool add_player(int playerid)
    {
        if (playercount < playersize)
        {
            playercount++;
            vect_playerid.push_back(playerid);
            vect_opid.push_back(0);
            return 1;
        }
        else
        {
            return 0;
        }
    }
    void add_op(const beatsgame::Operation &op)
    {
        vect_op.push_back(op);
    }
    int get_playerid(int gameid)
    {
        if (gameid > 0 && gameid <= playercount)
        {
            return vect_playerid[gameid - 1];
        }
        return -1;
    }
    bool update_state(int gameid, beatsgame::ROperation &rop)
    {
        if (gameid <= playercount && gameid > 0)
        {
            int &opid = vect_opid[gameid - 1];
            while (opid < vect_op.size())
            {
                my_log("opid", opid);
                mergeOperations(rop, vect_op[opid++]);
            }
            return 1;
        }
        else
        {
            my_log("gameid > playercount or gameid <= 0, gameid:", gameid);
            return 0;
        }
    }

private:
    std::vector<beatsgame::Operation> vect_op;
    std::vector<int> vect_playerid;
    std::vector<int> vect_opid;
};

class DebugGameServer : public ServerBase
{
    // void on_close(int fd) override {
    //     ServerBase::on_close(fd);
    // }
public:
    DebugGameServer() : ServerBase(ServerType::none)
    {
        pool_room = new ObjectPool<RoomInfo>(256);
        frame = 0;
    }
    ~DebugGameServer()
    {
        delete pool_room;
    }
    void on_close(int fd) override
    {
        ServerBase::on_close(fd);
        if (uset_clientfd.empty())
        {
            list_room.front()->init();
        }
    }
    void solve_pkg(int fd, Header *header) override
    {
        ServerBase::solve_pkg(fd, header);
        if (header->type == LOGIN)
        {
            int id = fd;
            my_log("fd", fd);
            auto rlogin = RMessageBuilder::createRLoginMessage(fd, 1);
            // Header hd(rlogin.ByteSizeLong(), id, RLOGIN);
            // if (SerializeWithHeader(&hd, rlogin, pkg_buf, PKGSIZE)) {
            //     my_log("send rlogin", hd.length);
            //     Send(fd, pkg_buf, Header::header_length + hd.length, 0);
            // }
            my_log("try enter");

            auto renter = RMessageBuilder::createREnterMessage(uset_clientfd.size(), 633);
            Header hd2(renter.ByteSizeLong(), id, RENTER);
            if (SerializeWithHeader(&hd2, renter, pkg_buf, PKGSIZE))
            {
                Send(fd, pkg_buf, Header::header_length + hd2.length, 0);
            }
            my_log("try enter end");
            if (list_room.empty())
            {
                auto rm = pool_room->acquire();
                rm->init();
                list_room.push_back(rm);
            }
            list_room.front()->add_player(id);
        }
        else if (header->type == OPERATION)
        {
            int id = header->value;
            beatsgame::Operation op;
            if (op.ParseFromArray(pkg_buf + Header::header_length, header->length))
            {
                if (!list_room.empty())
                {
                    list_room.front()->add_op(op);
                }
            }
            else
            {
                my_log("op Parser Error");
            }
        }
        else
        {
            my_log("gameserver recv pkg_type error", header->type);
        }
    }
    void do_list()
    {
        //  my_log("do_list");
        for (auto rm : list_room)
        {
            //   my_log("room", rm->playercount);
            for (int gameid = 1; gameid <= rm->playercount; gameid++)
            {
                beatsgame::ROperation rop;
                int playerid = rm->get_playerid(gameid);
                //    my_log("playerid", playerid);
                if (playerid == -1)
                    continue;
                rm->update_state(gameid, rop);
                rop.set_deltatime(step);
                rop.set_frame(frame);
                Header hd(rop.ByteSizeLong(), playerid, ROPERATION);
                if (SerializeWithHeader(&hd, rop, pkg_buf, PKGSIZE))
                {
                    //   my_log("send", playerid);
                    Send(playerid, pkg_buf, Header::header_length + hd.length, 0);
                }
            }
        }
    }
    void run() override
    {
        auto last = getTimeMS();
        while (1)
        {
            auto now = getTimeMS();
            if (now - last >= step)
            {
                frame++;
                do_list();
                last = now;
            }
            this->epoll_step(0);
            this->try_reconnect();
        }
    }
    void set_step(int step)
    {
        this->step = step;
    }

private:
    int step;
    int frame;
    ObjectPool<RoomInfo> *pool_room;
    std::list<RoomInfo *> list_room;
    std::unordered_map<int, std::list<RoomInfo *>::iterator> umap_id2itor;
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        my_log("example: ./server <TIME_STEP>");
        exit(1);
    }
    int step;
    try
    {
        step = std::stoi(argv[1]);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        exit(1);
    }
    DebugGameServer server;
    server.open_as_server(DGAME_PORT);
    server.set_step(step);
    server.run();
}