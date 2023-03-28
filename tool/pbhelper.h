#pragma once

#include <string>
#include "../proto/project.pb.h"
#include "../proto/server.pb.h"


class RMessageBuilder {
public:
    static beatsgame::RLogin createRLoginMessage(int id, uint32_t success);
    static beatsgame::RMatch createRMatchMessage(uint32_t success);
    static beatsgame::REnter createREnterMessage(uint32_t gameid, uint32_t seed);
    static beatsgame::ROperation createROperationMessage(uint32_t frame, uint32_t deltatime, const std::vector<beatsgame::OP>& operations);
    static beatsgame::RExitgame createRExitgameMessage(int id, uint32_t success);
};

class ServerMessageBuilder {
public:
    static serverproto::ConnectServer  createConnectServerMessage(const std::string &srevername);
    static serverproto::RConnectServer createRConnectServerMessage(bool read);
    static serverproto::CreateRoom createCreateRoomMessage(int id1, int id2);
};
