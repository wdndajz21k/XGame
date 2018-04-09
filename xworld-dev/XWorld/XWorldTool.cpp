#include "stdafx.h"
#include "XWorldTool.h"
#include "XWorldApp.h"

#ifdef _WIN32
#include <process.h>
#endif

using namespace google::protobuf;

#ifdef _WIN32
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")
#endif

#ifdef _WDEBUG
#define new DEBUG_NEW
#endif

enum class RpcType
{
    None = -1,
    Nil = 0,
    Boolean = 1,
    LightUserdata = 2,
    Double = 3,
    String = 4,
    Table = 5,
    Function = 6,
    Userdata = 7,
    Thread = 8,
    Int,
    Int64,
    Float,
};

bool XWorldTool::PackLuaTable(xrpc_param *p, lua_State *L)
{
    static bool ACCEPT_KEY_TYPE[LUA_NUMTAGS] = { false, true, false, true, true, false, false, false, false };
    static bool ACCEPT_VALUE_TYPE[LUA_NUMTAGS] = { true, true, false, true, true, true, false, false, false };
    auto valueType = (RpcType)lua_type(L, -1);
    auto keyType = (RpcType)lua_type(L, -2);
    if (!ACCEPT_KEY_TYPE[(int)keyType] || !ACCEPT_VALUE_TYPE[(int)valueType])
        return true;

    p->add_key_type((int)keyType);
    switch (keyType)
    {
    case RpcType::Boolean:
        p->add_key_bool(!!lua_toboolean(L, -2));
        break;
    case RpcType::Double:
        p->add_key_double(lua_tonumber(L, -2));
        break;
    case RpcType::String:
        p->add_key_str(lua_tostring(L, -2));
        break;
    default:
        assert(false);
        break;
    }

    p->add_value_type((int)valueType);
    switch (valueType)
    {
    case RpcType::Nil:
        break;
    case RpcType::Boolean:
        p->add_value_bool(!!lua_toboolean(L, -1));
        break;
    case RpcType::Double:
        p->add_value_double(lua_tonumber(L, -1));
        break;
    case RpcType::String:
        p->add_value_str(lua_tostring(L, -1));
        break;
    case RpcType::Table:
        mLua::M_Traversal(L, -1, std::bind(PackLuaTable, p->add_value_table(), std::placeholders::_1));
        break;
    default:
        assert(false);
        break;
    }
    return true;
}

void XWorldTool::PushLuaParams(lua_State *L, xrpc_param *p)
{
    int keyBoolIndex = 0, keyDoubleIndex = 0, keyStrIndex = 0;
    int valueBoolIndex = 0, valueDoubleIndex = 0, valueStrIndex = 0, valueTableIndex = 0;
    lua_newtable(L);
    for (int i = 0; i < p->key_type_size(); ++i)
    {
        switch (p->key_type(i))
        {
        case LUA_TNUMBER:
            lua_pushnumber(L, p->key_double(keyDoubleIndex++));
            break;
        case LUA_TBOOLEAN:
            lua_pushboolean(L, p->key_bool(keyBoolIndex++));
            break;
        case LUA_TSTRING:
            lua_pushstring(L, p->key_str(keyStrIndex++).c_str());
            break;
        default:
            LogError("Unknown Key Type: $", p->key_type(i));
            return;
        }

        switch (p->value_type(i))
        {
        case LUA_TNUMBER:
            lua_pushnumber(L, p->value_double(valueDoubleIndex++));
            break;
        case LUA_TBOOLEAN:
            lua_pushboolean(L, p->value_bool(valueBoolIndex++));
            break;
        case LUA_TSTRING:
            lua_pushstring(L, p->value_str(valueStrIndex++).c_str());
            break;
        case LUA_TTABLE:
            PushLuaParams(L, p->mutable_value_table(valueTableIndex++));
            break;
        default:
            LogError("Unknown Value Type: $", p->value_type(i));
            lua_pushnil(L);
            continue;
        }
        lua_settable(L, -3);
    }
}

bool XWorldTool::CreateItemData(const std::string& itemStr, xitem_data *itemDataOut)
{
    assert(itemDataOut);
    bool result = false;
    xitem_data itemData;
    std::string lua = XTool::Format("return {$}", itemStr);
    mLua::luafunc f(WorldAppHandle->LuaState, "loadstring");
    mLua::luafunc f1 = f();
    mLua::luatable tbItem = f1();
    itemData.set_template_id((int)tbItem[1]);
    itemData.set_count((int)tbItem[2]);

    if (tbItem.containsKey(3))
    {
        mLua::luatable tbIntValue = tbItem[3];
        XLOG_FAILED_JUMP(tbIntValue.length() % 2 == 0);
        for (int i = 1; i <= tbIntValue.length(); ++i)
        {
            if (i % 2 == 1)
                itemData.mutable_user_data()->add_int_key((int)tbIntValue[i]);
            else
                itemData.mutable_user_data()->add_int_value((int)tbIntValue[i]);
        }
    }

    if (tbItem.containsKey(4))
    {
        mLua::luatable tbStrValue = tbItem[4];
        XLOG_FAILED_JUMP(tbStrValue.length() % 2 == 0);
        for (int i = 1; i <= tbStrValue.length(); ++i)
        {
            if (i % 2 == 1)
                itemData.mutable_user_data()->add_str_key((int)tbStrValue[i]);
            else
                itemData.mutable_user_data()->add_str_value((const char *)tbStrValue[i]);
        }
    }

    *itemDataOut = std::move(itemData);
    result = true;

Exit0:
    return result;
}

std::string XWorldTool::ItemData2String(const xitem_data& itemData)
{
    std::stringstream ss;
    const xuser_data* ud = &itemData.user_data();
    ss << itemData.template_id() << "," << itemData.count() << ",{";
    for (int i = 0; i < ud->int_key_size(); ++i)
    {
        ss << ud->int_key(i) << "," << ud->int_value(i) << ",";
    }
    ss << "},{";
    for (int i = 0; i < ud->str_key_size(); ++i)
    {
        ss << ud->str_key(i) << ",\"" << ud->str_value(i) << "\",";
    }
    ss << "}";
    return ss.str();
}

static void ProtoGetRepeatedValue(std::stringstream& ss, const google::protobuf::Message *msg, const FieldDescriptor *field);
static void ProtoGetValue(std::stringstream& ss, const google::protobuf::Message *msg, const FieldDescriptor *field);

static std::string& EscapeJSONString(std::string& s)
{
    char buf[1024];
    std::string src = " ";
    for (int i = 0; i < s.size(); ++i)
    {
        if (s[i] > 0 && s[i] < 32)
        {
            sprintf(buf, "\\\\x0%x", s[i]);
            src[0] = s[i];
            XWorldTool::ReplaceString(s, src, buf);
        }
    }

    std::string ns = " ";
    ns[0] = '\0';
    XWorldTool::ReplaceString(s, ns, "\\\\0");
    XWorldTool::ReplaceString(s, "\"", "\\\"");
    return s;
}

static void ProtoGetMessage(std::stringstream& ss, const google::protobuf::Message *msg)
{
    const Descriptor *desc = msg->GetDescriptor();
    ss << "{";
    for (int i = 0; i < desc->field_count(); ++i)
    {
        auto field = desc->field(i);
        if (field->is_repeated())
            ProtoGetRepeatedValue(ss, msg, field);
        else
            ProtoGetValue(ss, msg, field);
        if (i < desc->field_count() - 1)
            ss << ",\n";
    }
    ss << "\n}";
}

static void ProtoGetValue(std::stringstream& ss, const google::protobuf::Message *msg, const FieldDescriptor *field)
{
    const Reflection* refl = msg->GetReflection();
    ss << "\"" << field->name() << "\": ";
    switch (field->cpp_type())
    {
    case FieldDescriptor::CPPTYPE_INT32:
        ss << std::to_string(refl->GetInt32(*msg, field));
        break;
    case FieldDescriptor::CPPTYPE_INT64:
        ss << std::to_string(refl->GetInt64(*msg, field));
        break;
    case FieldDescriptor::CPPTYPE_UINT32:
        ss << std::to_string(refl->GetUInt32(*msg, field));
        break;
    case FieldDescriptor::CPPTYPE_UINT64:
        ss << std::to_string(refl->GetUInt64(*msg, field));
        break;
    case FieldDescriptor::CPPTYPE_DOUBLE:
        ss << std::to_string(refl->GetDouble(*msg, field));
        break;
    case FieldDescriptor::CPPTYPE_FLOAT:
        ss << std::to_string(refl->GetFloat(*msg, field));
        break;
    case FieldDescriptor::CPPTYPE_BOOL:
        ss << std::to_string(refl->GetBool(*msg, field));
        break;
    case FieldDescriptor::CPPTYPE_ENUM:
        ss << std::to_string(refl->GetEnum(*msg, field)->number());
        break;
    case FieldDescriptor::CPPTYPE_STRING:
    {
        std::string s = refl->GetString(*msg, field);
        EscapeJSONString(s);
        ss << "\"" << s << "\"";
        break;
    }
    case FieldDescriptor::CPPTYPE_MESSAGE:
        ProtoGetMessage(ss, &refl->GetMessage(*msg, field));
        break;
    }
}

static void ProtoGetRepeatedValue(std::stringstream& ss, const google::protobuf::Message *msg, const FieldDescriptor *field)
{
    const Reflection* refl = msg->GetReflection();
    int count = msg->GetReflection()->FieldSize(*msg, field);
    ss << "\"" << field->name() << "\": [";
    for (int i = 0; i < count; ++i)
    {
        switch (field->cpp_type())
        {
        case FieldDescriptor::CPPTYPE_INT32:
            ss << std::to_string(refl->GetRepeatedInt32(*msg, field, i));
            break;
        case FieldDescriptor::CPPTYPE_INT64:
            ss << std::to_string(refl->GetRepeatedInt64(*msg, field, i));
            break;
        case FieldDescriptor::CPPTYPE_UINT32:
            ss << std::to_string(refl->GetRepeatedUInt32(*msg, field, i));
            break;
        case FieldDescriptor::CPPTYPE_UINT64:
            ss << std::to_string(refl->GetRepeatedUInt64(*msg, field, i));
            break;
        case FieldDescriptor::CPPTYPE_DOUBLE:
            ss << std::to_string(refl->GetRepeatedDouble(*msg, field, i));
            break;
        case FieldDescriptor::CPPTYPE_FLOAT:
            ss << std::to_string(refl->GetRepeatedFloat(*msg, field, i));
            break;
        case FieldDescriptor::CPPTYPE_BOOL:
            ss << std::to_string(refl->GetRepeatedBool(*msg, field, i));
            break;
        case FieldDescriptor::CPPTYPE_ENUM:
            ss << std::to_string(refl->GetEnum(*msg, field)->number());
            break;
        case FieldDescriptor::CPPTYPE_STRING:
        {
            std::string s = refl->GetRepeatedString(*msg, field, i);
            EscapeJSONString(s);
            ss << "\"" << s << "\"";
        }
        break;
        case FieldDescriptor::CPPTYPE_MESSAGE:
            ProtoGetMessage(ss, &refl->GetRepeatedMessage(*msg, field, i));
            break;
        }
        if (i < count - 1)
            ss << ", \n";
        else
            ss << "\n";
    }
    ss << "]";
}

std::string XWorldTool::Protobuf2Json(const google::protobuf::Message * msg)
{
    std::stringstream ss;
    ProtoGetMessage(ss, msg);
    return ss.str();
}

void XWorldTool::ReplaceString(std::string& s, const std::string& src, const std::string& dst)
{
    if (src.size() == 0)
        return;

    int pos = 0;
    while ((pos = s.find(src, pos)) != s.npos)
    {
        s.replace(pos, src.length(), dst);
        pos += dst.length();
    }
}

int XWorldTool::GetPid()
{
    return getpid();
}

int XWorldTool::GetDate()
{
    time_t timeNow = time(nullptr);
    tm* localTime = localtime(&timeNow);

    return (localTime->tm_year + 1900) * 10000 +
        (localTime->tm_mon + 1) * 100 +
        localTime->tm_mday;
}

std::unordered_map<std::string, int> Channel2Id = {
    { "tencent", 1 },
    { "qq",      1 },
    { "weixin",  1 },
    { "xiaomi",  2 },
    { "teamtop", 3 },
    { "uc",      4 },
};

int XWorldTool::GetChannelId(const std::string & channel)
{
    if (channel.empty())
        return 0;

    auto it = Channel2Id.find(channel);
    if (it != Channel2Id.end())
    {
        return it->second;
    }
    else
    {
        LogWarning("Invalid Channel $", channel);
        return 0;
    }
}

bool XWorldTool::IsValidIP(const std::string& ip)
{
    unsigned int n1 = 0, n2 = 0, n3 = 0, n4 = 0;
    if (sscanf(ip.c_str(), "%u.%u.%u.%u", &n1, &n2, &n3, &n4) != 4)
    {
        return false;
    }
    if ((n1 != 0) && (n1 <= 255) && (n2 <= 255) && (n3 <= 255) && (n4 <= 255))
    {
        char buf[64];
        sprintf(buf, "%u.%u.%u.%u", n1, n2, n3, n4);
        return strcmp(buf, ip.c_str()) == 0;
    }
    return false;
}
