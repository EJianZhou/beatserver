#pragma once
#include <iostream>
#include <string>
#include <map>

#define GATE_PORT 2435
#define GAME_PORT 2436
#define DB_PORT 2437
#define CONTROL_PORT 2439

#define DGAME_PORT 2440


const int MAXEVENTS = 1024;
const int BUFFSIZE = 1024;
const int PKGSIZE =  4096;

const int LENBYTES = 3;
const int TYPEBYTES = 1;



#define LOGIN 1
#define RLOGIN 2
#define MATCH 3
#define RMATCH 4
#define RENTER 5
#define OPERATION 6
#define ROPERATION 7


#define CONNECTSSERVER 100
#define RCONNECTSSERVER 101
#define GATELOGOUT 102
#define CREATROOM 103

const std::map<uint8_t, std::string> map_typeid2name = {
    {1, "LOGIN"},
    {2, "RLOGIN"},
    {3, "MATCH"},
    {4, "RMATCH"},
    {5, "RENTER"},
    {6, "OPERATION"},
    {7, "ROPERATION"},
    {100, "CONNECTSSERVER"},
    {101, "RCONNECTSSERVER"}
};


inline std::string getTypeName(uint8_t type) {
    auto it = map_typeid2name.find(type);
    if (it != map_typeid2name.end())
        return it->second;
    else 
        return "unknow";
}
