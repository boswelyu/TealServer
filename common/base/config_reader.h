#pragma once

#include <string>
#include <map>

using namespace std;

class ConfigReader
{
public:
    ConfigReader();
    ~ConfigReader();

    bool LoadConfig(const char *file);

    std::string GetStringValue(const char* name, const char* def = "");
    bool GetBoolValue(const char* name, const bool def = false);
    int GetIntValue(const char* name, int def = 0);
    unsigned int GetUIntValue(const char *name, unsigned int def = 0);
    float GetFloatValue(const char* name, float def = 0);

private:
    bool ParseConfig(FILE * fp);
    bool IsEmptyLine(char * line, int len);
    void ParseLine(char * line, int len);

    const char * FindNode(const char * keyStr);

    void PrintLoadedConfig();

private:
    bool m_loaded;
    std::map<string, string> m_nodes;
};

