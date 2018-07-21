#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ConfigReader.h"


ConfigReader::ConfigReader() : m_loaded(false)
{
}

ConfigReader::~ConfigReader()
{
    m_loaded = false;
}


bool ConfigReader::LoadConfig(const char *file)
{
    FILE * fp = fopen(file, "r");
    if(fp == NULL)
    {
        printf("Open configure file failed: %s, error: %s\n", file, strerror(errno));
        return false;
    }

    m_loaded = ParseConfig(fp);
    return m_loaded;
}

std::string ConfigReader::GetStringValue(const char* name, const char* def /* =  */)
{
    if(!m_loaded) return std::string(def);

    const char * str = FindNode(name);
    if(str == NULL) return std::string(def);

    return std::string(str);
}


bool ConfigReader::GetBoolValue(const char* name, const bool def /* = false */)
{
    if(!m_loaded) return false;

    const char * str = FindNode(name);

    if(str == NULL) return def;
    
    if(strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0 ||
        strcmp(str, "yes") == 0 || strcmp(str, "YES") == 0 ||
        strcmp(str, "1") == 0)
    {
        return true;
    }

    return false;
}

int ConfigReader::GetIntValue(const char* name, const int def /* = 0 */)
{
    if(!m_loaded)  return def;

    const char * str = FindNode(name);
    if(str == NULL)  return def;

	int value = 0;
	if (strncmp(str, "0x", 2) == 0 || strncmp(str, "0X", 2) == 0)
	{
		value = strtol(str, NULL, 16);
	}
	else
	{
		value = strtol(str, NULL, 10);
	}

    return value;
}

unsigned int ConfigReader::GetUIntValue(const char *name, unsigned int def /* = 0 */)
{
	if(!m_loaded) return def;

    const char * str = FindNode(name);
    if(str == NULL) return def;

	unsigned int value = 0;

	if (strncmp(str, "0x", 2) == 0 || strncmp(str, "0X", 2) == 0)
	{
		value = strtoul(str, NULL, 16);
	}
	else
	{
		value = strtoul(str, NULL, 10);
	}

	return value;
}


float ConfigReader::GetFloatValue(const char* name, float def /* = 0 */)
{
    if(!m_loaded)  return def;

    const char * str = FindNode(name);
    if(str == NULL) return def;

    float value = (float)atof(str);
    return value;
}

// Private Functions
bool ConfigReader::ParseConfig(FILE * fp)
{
    char line[512];

    while(fgets(line, sizeof(line), fp))
    {
        int len = strlen(line);
        if(len >= sizeof(line) - 1)
        {
            printf("Line too long in config file\n %s\n", line);
            return false;
        }

        if(IsEmptyLine(line, len)) continue;
        ParseLine(line, len);
    }

    PrintLoadedConfig();
    return true;
}

bool ConfigReader::IsEmptyLine(char * line, int len)
{
    if(line[0] == '#') return true;

    bool hasEqualMark = false;
    bool hasAlpha = false;
    for(int i = 0; i < len; i++)
    {
        if(line[i] == '=') 
        {
            hasEqualMark = true;
        }
        else if( isalpha(line[i]) )
        {
            hasAlpha = true;
        }

        if(hasEqualMark && hasAlpha) return false;
    }
    return true;
}

void ConfigReader::ParseLine(char * line, int len)
{
    char * mark = strstr(line, "=");
    if(mark == NULL) return;

    char key[512], value[512];
    int keyLen = mark - line;
    
    int from = 0, to = 0;
    char c;
    while(line[from] == ' ' || line[from] == '\t') from++;
    for( ; from < keyLen; from++, to++)
    {
        c = line[from];
        if(c == ' ' || c == '\t')
        {
            key[to] = 0;
            break;
        }
        key[to] = line[from];
    }
    key[to] = 0;

    to = 0;
    from = keyLen + 1;
    while(line[from] == ' ' || line[from] == '\t') from++;
    for( ; from < len; from++, to++)
    {
        c = line[from];
        if(c == ' ' || c == '\t' || c == '\n')
        {
            value[to] = 0;
            break;
        }
        value[to] = line[from];
    }
    value[to] = 0;

    if(strlen(key) == 0 || strlen(value) == 0)
    {
        printf("Empty Key or Value in line: %s\n", line);
        return;
    }

    m_nodes.insert(make_pair(std::string(key), std::string(value)));
}

const char * ConfigReader::FindNode(const char * key)
{
    std::string keyStr(key);
    std::map<std::string, std::string>::iterator iter = m_nodes.find(keyStr);
    if(iter == m_nodes.end())
    {
        return NULL;
    }

    return iter->second.c_str();
}