#include "pbhelper.h"

beatsgame::RLogin RMessageBuilder::createRLoginMessage(int id, uint32_t success)
{
    beatsgame::RLogin rLogin;
    rLogin.set_id(id);
    rLogin.set_success(success);
    return rLogin;
}

beatsgame::RMatch RMessageBuilder::createRMatchMessage(uint32_t success)
{
    beatsgame::RMatch rMatch;
    rMatch.set_success(success);
    return rMatch;
}

beatsgame::REnter RMessageBuilder::createREnterMessage(uint32_t gameid, uint32_t seed)
{
    beatsgame::REnter rEnter;
    rEnter.set_gameid(gameid);
    rEnter.set_seed(seed);
    return rEnter;
}

beatsgame::ROperation RMessageBuilder::createROperationMessage(uint32_t frame, uint32_t deltatime, const std::vector<beatsgame::OP> &operations)
{
    beatsgame::ROperation rOperation;
    rOperation.set_frame(frame);
    rOperation.set_deltatime(deltatime);
    for (const auto &op : operations)
    {
        *rOperation.add_operations() = op;
    }
    return rOperation;
}

beatsgame::RExitgame RMessageBuilder::createRExitgameMessage(int id, uint32_t success)
{
    beatsgame::RExitgame rExitgame;
    rExitgame.set_id(id);
    rExitgame.set_success(success);
    return rExitgame;
}

serverproto::ConnectServer ServerMessageBuilder::createConnectServerMessage(const std::string &servername)
{
    serverproto::ConnectServer connectserver;
    connectserver.set_servername(servername);
    return connectserver;
}

serverproto::RConnectServer ServerMessageBuilder::createRConnectServerMessage(bool read)
{
    serverproto::RConnectServer rConnectserver;
    rConnectserver.set_read(read);
    return rConnectserver;
}
serverproto::CreateRoom ServerMessageBuilder::createCreateRoomMessage(int id1, int id2)
{
    serverproto::CreateRoom createroom;
    createroom.set_id1(id1);
    createroom.set_id2(id2);
    return createroom;
}