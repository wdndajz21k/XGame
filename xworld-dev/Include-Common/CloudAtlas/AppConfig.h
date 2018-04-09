#pragma once
#include <regex>

bool XSetProtobufMessageValue(google::protobuf::Message *msg, const google::protobuf::FieldDescriptor *field, const std::string& value);
int XReadColumnHeader(std::vector<const google::protobuf::FieldDescriptor*>& fields, const google::protobuf::Descriptor* msgDesc, const char *content);
bool XReadTabFileLine(google::protobuf::Message *msg, std::vector<const google::protobuf::FieldDescriptor*>& fields, const char *content, size_t size, int *usedSize);
int XGetLineCount(const char *s);
int XGetLineFieldCount(const char *s);
bool ReadConfigFile(const char *file, google::protobuf::Message *msg);
bool XReadCmdConfig(int argc, char* argv[], google::protobuf::Message *msg);

template <typename MsgT>
inline bool ReadTableFile(const std::string& file, std::vector<MsgT>& msgList)
{
    bool result = false;
    std::string s;
    if (!XTool::ReadFile(file, s))
        return false;

    int lineCount = XGetLineCount(s.c_str()) - 1;
    if (lineCount <= 0)
        return true;

    const google::protobuf::Descriptor* msgDesc = MsgT::descriptor();
    std::vector<const google::protobuf::FieldDescriptor*> fields;
    int totalUsedSize = XReadColumnHeader(fields, msgDesc, s.c_str());
    XLOG_FAILED_JUMP(totalUsedSize >= 0);

    msgList.resize(lineCount);
    for (auto& msg : msgList)
    {
        int usedSize = 0;
        result = XReadTabFileLine(&msg, fields, s.c_str() + totalUsedSize, s.size() - totalUsedSize, &usedSize);
        XLOG_FAILED_JUMP(result);
        totalUsedSize += usedSize;
    }

    result = true;

Exit0:
    if (!result)
        LogError("Read TabFile Failed: $", file);
    return result;
}

template <typename MsgT>
inline bool ReadTableFile(const std::string& file, std::unordered_map<int, MsgT>& msgMap, const char *idKey = "id")
{
    std::vector<MsgT> msgList;
    if (!ReadTableFile(file, msgList))
        return false;

    const google::protobuf::Descriptor* msgDesc = MsgT::descriptor();
    const google::protobuf::FieldDescriptor *fieldDesc = msgDesc->FindFieldByName(idKey);
    for (MsgT& msg : msgList)
    {
        int id = msg.GetReflection()->GetInt32(msg, fieldDesc);
        if (msgMap.find(id) != msgMap.end())
            LogWarning("$ ID: $ Duplicity!!!", file, id);
        else
            msgMap.insert(std::make_pair(id, std::move(msg)));
    }
    return true;
}

template <typename MsgT>
inline bool ReadTableFile(const std::string& file, std::unordered_map<std::string, MsgT>& msgMap, const char *indexKey = "index")
{
    std::vector<MsgT> msgList;
    if (!ReadTableFile(file, msgList))
        return false;

    const google::protobuf::Descriptor* msgDesc = MsgT::descriptor();
    const google::protobuf::FieldDescriptor *fieldDesc = msgDesc->FindFieldByName(indexKey);
    for (MsgT& msg : msgList)
    {
        const std::string& keyStr = msg.GetReflection()->GetString(msg, fieldDesc);
        if (msgMap.find(keyStr.c_str()) != msgMap.end())
            LogError("$ Index: $ Duplicity!!!", file, keyStr);
        else
            msgMap[keyStr] = std::move(msg);
    }
    return true;
}

template <typename MsgKey1,typename MsgT1, typename MsgKey2 ,typename MsgT2>
inline bool CheckValidTableData(std::unordered_map<MsgKey1, MsgT1>& msgMap1, std::unordered_map<MsgKey2, MsgT2>& msgMap2, const char *key)
{
	bool result = false;
	assert(key);
	const google::protobuf::Descriptor* msgDesc = MsgT1::descriptor();
	const google::protobuf::FieldDescriptor *field = msgDesc->FindFieldByName(key);

	XLOG_FAILED_JUMP(msgDesc);
	XLOG_FAILED_JUMP(field);

	for (auto it1 = msgMap1.begin(); it1 != msgMap1.end(); it1++)
	{
		if (field->is_repeated())
		{
			switch (field->type())
			{
			case google::protobuf::FieldDescriptor::TYPE_INT32:


				for (int i = 0; i < it1->second.GetReflection()->FieldSize(it1->second, field);++i)
				{
					if (it1->second.GetReflection()->GetRepeatedInt32(it1->second, field, i) > 0)
					{
						if (msgMap2.find(it1->second.GetReflection()->GetRepeatedInt32(it1->second, field, i)) == msgMap2.end())
						{
							LogError("$ id $ not exist in $ at $:$ in $", field->name(), it1->second.GetReflection()->GetRepeatedInt32(it1->second, field, i), MsgT2::descriptor()->name(), __FILE__, __LINE__, __THIS_FUNCTION__);
							goto Exit0;
						}
					}
				}



				break;
			case google::protobuf::FieldDescriptor::TYPE_STRING:

				break;
			case google::protobuf::FieldDescriptor::TYPE_DOUBLE:

				break;
			case google::protobuf::FieldDescriptor::TYPE_FLOAT:

				break;
			case google::protobuf::FieldDescriptor::TYPE_UINT32:

				break;
			case google::protobuf::FieldDescriptor::TYPE_INT64:

				break;
			case google::protobuf::FieldDescriptor::TYPE_UINT64:

				break;
			case google::protobuf::FieldDescriptor::TYPE_BOOL:

				break;
			default:
				assert(false);
			}
		}
		else 
		{
			switch (field->type())
			{
			case google::protobuf::FieldDescriptor::TYPE_INT32:

				if (it1->second.GetReflection()->GetInt32(it1->second, field) > 0)
				{
					if (msgMap2.find(it1->second.GetReflection()->GetInt32(it1->second, field)) == msgMap2.end())
					{
						LogError("$ id $ not exist in $ at $:$ in $", field->name(), it1->second.GetReflection()->GetInt32(it1->second, field), MsgT2::descriptor()->name(), __FILE__, __LINE__, __THIS_FUNCTION__);
						goto Exit0;
					}
				}

				break;
			case google::protobuf::FieldDescriptor::TYPE_STRING:

				break;
			case google::protobuf::FieldDescriptor::TYPE_DOUBLE:

				break;
			case google::protobuf::FieldDescriptor::TYPE_FLOAT:

				break;
			case google::protobuf::FieldDescriptor::TYPE_UINT32:

				break;
			case google::protobuf::FieldDescriptor::TYPE_INT64:

				break;
			case google::protobuf::FieldDescriptor::TYPE_UINT64:

				break;
			case google::protobuf::FieldDescriptor::TYPE_BOOL:

				break;
			default:
				assert(false);
			}
		}




	}


	result = true;
Exit0:
	return result;
}
