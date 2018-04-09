#pragma once

constexpr int GAME_PROTO_VERSION = 3;
constexpr size_t XGAME_FPS = 10;

constexpr size_t REGION_WIDTH = 32;
constexpr size_t REGION_HEIGHT = 32;
constexpr size_t CELL_WIDTH = 32;
constexpr size_t CELL_HEIGHT = 32;
constexpr size_t MAP_MPU = 64; // 1米=64个逻辑单位=2个CELL

#define RECONNECT_TIME 3
#define LOGIN_TIMEOUT_TIME 10
#define ROLE_ID_START 5184190

enum class XPLAYER_KICKOUT_REASON
{
    SystemError,
    ProtocolError,
    Maintain,
    DisconnectTimeout,
    AnotherLogin,
    ForbidLogin,
};

enum class XMoneyType
{
    Gold,
    Diamond,
    FrozenDiamond,
    MoneyKind,
};

#pragma warning(push, 3)
#include "XWorld/xworld.pb.h"
#include "XWorld/xconfig.pb.h"
#include "XWorld/xtabfile.pb.h"
#pragma warning(pop)

typedef ::google::protobuf::RepeatedPtrField<std::string> RepeatedString;
typedef ::google::protobuf::RepeatedPtrField<int> RepeatedInt;

#include "XWorld/XWorldTool.h"
#include "XWorld/XWorldApp.h"

struct XPoint
{
public:
    XPoint() { }
    XPoint(int x, int y) : X(x), Y(y) { }
    bool operator<(const XPoint& rhl) const { return ((uint64_t)X << 32 | Y) < ((uint64_t)rhl.X << 32 | rhl.Y); }
    double DistanceTo(int x, int y);
    double DistanceTo(const XPoint& point);

    int X = 0;
    int Y = 0;
};

double GetDistance(int x1, int y1, int x2, int y2);
double GetDistanceSquare(int x1, int y1, int x2, int y2);
double GetDistance(const XPoint& point1, const XPoint& point2);
void Normalize(float& x, float& y);

void CloseStatLog();
void InternalStatLog(const std::string& logType, const std::string& log);

inline void FormatStatLog(std::stringstream& ss)
{
}

template <typename KeyType, typename ValueType, typename ... Tails>
inline void FormatStatLog(std::stringstream& ss, KeyType key, ValueType value, Tails ... tails)
{
    ss << "," << key << "=" << value;
    FormatStatLog(ss, tails...);
}

template <typename ... ArgsType>
inline void StatLog(const std::string& logType, ArgsType ... args)
{
    std::stringstream ss;
    FormatStatLog(ss, args...);
    InternalStatLog(logType, ss.str());
}

