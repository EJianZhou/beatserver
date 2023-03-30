#include "mysocket.h"
#include "serverbase.h"
#include "tool/circularbuffer.h"
#include "tool/pbhelper.h"
#include "tool/objectpool.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>

#define NOW std::chrono::steady_clock::now()

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
	int frame;
	RoomInfo(int playersize = 2) : playersize(playersize), playercount(0)
	{
		vect_op.reserve(1 << 10);
		vect_playerid.reserve(playersize);
		vect_opid.reserve(playersize);
		vect_state.reserve(playersize);
		frame = 0;
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
			vect_state.push_back(1);
			return 1;
		}
		else
		{
			return 0;
		}
	}
	void player_in(int playerid)
	{
		for (int i = 0; i < playercount; i++)
		{
			if (vect_playerid[i] == playerid)
			{
				vect_state[i] = 1;
				return;
			}
		}
	}
	void player_out(int playerid)
	{
		for (int i = 0; i < playercount; i++)
		{
			if (vect_playerid[i] == playerid)
			{
				vect_opid[i] = 0;
				vect_state[i] = 0;
				return;
			}
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
	int get_state(int gameid)
	{
		if (gameid > 0 && gameid <= playercount)
		{
			return vect_state[gameid - 1];
		}
		return 0;
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
	std::vector<int> vect_state;
};

struct time_event
{
	std::chrono::steady_clock::time_point point;
	std::list<RoomInfo *>::iterator listit_room;
};
class GameServer : public ServerBase
{
	// void on_close(int fd) override {
	//     ServerBase::on_close(fd);
	// }
public:
	GameServer() : ServerBase(ServerType::gameserver)
	{
		pool_room = new ObjectPool<RoomInfo>(256);
	}
	~GameServer()
	{
		delete pool_room;
	}
	void on_server_close(ServerType type) override
	{
		if (type == ServerType::controlserver)
		{
		}
		else if (type == ServerType::gateserver)
		{
		}
	}
	void solve_pkg(int fd, Header *header) override
	{
		ServerBase::solve_pkg(fd, header);
		if (header->type == CREATROOM)
		{
			my_log("create room");
			serverproto::CreateRoom cr;
			if (cr.ParseFromArray(pkg_buf + Header::header_length, header->length))
			{
				RoomInfo *rm = pool_room->acquire();
				list_room.push_back(rm);
				auto itor = --list_room.end();
				umap_id2itor[cr.id1()] = itor;
				umap_id2itor[cr.id2()] = itor;
				rm->add_player(cr.id1());
				rm->add_player(cr.id2());
				qu_event.push({NOW + std::chrono::milliseconds(step), itor});
			}
			else
			{
				my_log("CreatRoom Parser Error");
			}
		}
		else if (header->type == OPERATION)
		{ // 客户端发来操作
			int id = header->value;
			my_log("operation");
			if (umap_id2itor.count(id))
			{
				my_log("id", id);
				auto itor = umap_id2itor[id];
				RoomInfo *rm = *itor;
				beatsgame::Operation op;
				if (op.ParseFromArray(pkg_buf + Header::header_length, header->length))
				{
					my_log("add op");
					if (rm) rm->add_op(op);
				}
				else
				{
					my_log("op Parser Error");
				}
			}
			else
			{
				my_log("player is not exist");
			}
		}
		else if (header->type == PLAYEROUT)
		{ // 玩家掉线断线
			my_log("处理玩家掉线");
			int playerid = header->value;
			if (umap_id2itor.count(playerid))
			{
				auto itor = umap_id2itor[playerid];
				RoomInfo *rm = *itor;
				if (rm) rm->player_out(playerid);
			}
			else
			{
				my_log("player is not exist");
			}
		}
		else if (header->type == PLAYERIN)
		{
			my_log("玩家断线重连");
			int playerid = header->value;
			if (umap_id2itor.count(playerid))
			{
				auto itor = umap_id2itor[playerid];
				RoomInfo *rm = *itor;
				if (rm) rm->player_in(playerid);
			}
			else
			{
				my_log("玩家断线重连", "player is not exist");
			}
		}
		else
		{
			my_log("gameserver recv pkg_type", getTypeName(header->type));
		}
	}
	void do_room(RoomInfo *rm)
	{
		rm->frame++;
		for (int gameid = 1; gameid <= rm->playercount; gameid++)
		{
			beatsgame::ROperation rop;
			int playerid = rm->get_playerid(gameid);
			if (playerid == -1)
				continue;
			if (!rm->get_state(gameid))
				continue;
			rm->update_state(gameid, rop);
			rop.set_deltatime(step);
			rop.set_frame(rm->frame);
			Header hd(rop.ByteSizeLong(), playerid, ROPERATION);
			if (SerializeWithHeader(&hd, rop, pkg_buf, PKGSIZE))
			{
				send_to_server(ServerType::gateserver, &hd, pkg_buf);
			}
			else
			{
				my_log("ROP SerializeWithHeader Error");
			}
		}
	}
	void run() override
	{
		while (1)
		{
			while (!qu_event.empty() && NOW >= qu_event.front().point)
			{
				auto itor = qu_event.front().listit_room;
				if ((*itor)->playercount == 0)
					continue;
				qu_event.push({NOW + std::chrono::milliseconds(step), itor});
				do_room(*itor);
				qu_event.pop();
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
	ObjectPool<RoomInfo> *pool_room;
	std::list<RoomInfo *> list_room;
	std::queue<time_event> qu_event;
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
	GameServer gameserver;
	gameserver.open_as_server(GAME_PORT);
	std::vector<ServerInfo> info_list = {
		ServerInfo("127.0.0.1", CONTROL_PORT, ServerType::controlserver),
	};
	gameserver.set_step(step);
	for (auto &info : info_list)
	{
		gameserver.connect_as_client(info, false);
	}
	gameserver.run();
}