#pragma once

class XWorldTool
{
public:
    static bool PackLuaTable(xrpc_param *p, lua_State *L);
    static void PushLuaParams(lua_State *L, xrpc_param *p);
    static std::string Protobuf2Json(const google::protobuf::Message *msg);
    static void ReplaceString(std::string & s, const std::string& src, const std::string& dst);
    static int GetPid();
    static int GetDate();
    static int GetChannelId(const std::string& channel);
    static bool IsValidIP(const std::string& ip);

    template <typename T>
    static T* AutoExtendArray(T* arr, size_t oldSize, size_t newSize)
    {
        T* newArr = new T[newSize];
        memset(newArr, 0, sizeof(T) * newSize);
        memcpy(newArr, arr, sizeof(T) * oldSize);
        delete[] arr;
        return newArr;
    }

    template <typename MsgT>
    static void GbkToUtf8(std::vector<MsgT>& container, const char *key)
    {
        const google::protobuf::Descriptor* msgDesc = MsgT::descriptor();
        const google::protobuf::FieldDescriptor *fieldDesc = msgDesc->FindFieldByName(key);
        for (auto& it : container)
        {
            google::protobuf::Message *msg = &it;
            const std::string& gbkStr = msg->GetReflection()->GetString(*msg, fieldDesc);
            std::string utf8Str = XTool::GbkToUtf8(gbkStr);
            msg->GetReflection()->SetString(msg, fieldDesc, utf8Str);
        }
    }
};

