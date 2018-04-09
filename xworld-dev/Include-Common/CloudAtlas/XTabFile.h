#pragma once

class XTabFile
{
public:
    bool Load(const char *fileName);
    int GetInt(size_t row, const char *columnName);
    const char* GetStr(size_t row, const char *columnName);
    int RowCount();
    int ColumnCount();

protected:
    int ReadColumnHeader(const char *fileContent);
    int ReadTabFileLine(int row, const char *fileContent);

    std::map<std::string, int> ColumnName;
    std::vector<std::string> Content;
};

