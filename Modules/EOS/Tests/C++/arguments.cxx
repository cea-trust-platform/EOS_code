#include "arguments.hxx"
#include <cstring>
#include <iostream>

Arguments::Arguments(const int argc, char **argv)
{
  const char * optionName = NULL;

  for (int iarg = 1; iarg < argc; iarg++) {
    const char *s = argv[iarg];

    if (strlen(s) > 1 && s[0] == '-') {
        s++;
        if (s[0] == '-') s++;
        _arguments[s] = "1";
        optionName = s;
    }
    else {
        if (!optionName) continue;
        _arguments[optionName] = s;
        optionName = NULL;
    }
  }
}

#include <algorithm>

template<>
bool Arguments::Get(const char * name, const bool defaultValue)
{
  std::map<std::string, std::string>::iterator t = _arguments.find(name);
  if (t == _arguments.end())
     return defaultValue;
  std::string tt = t->second;
  std::transform(tt.begin(), tt.end(), tt.begin(), ::toupper);
  return !((tt == "0") || (tt == "N") || (tt == "NO") || (tt == "OFF"));
}

template<>
int Arguments::Get(const char * name, const int defaultValue)
{
  std::map<std::string, std::string>::iterator t = _arguments.find(name);
  if (t == _arguments.end())
     return defaultValue;
  return strtol(t->second.c_str(), NULL, 10);
}

template<>
unsigned long Arguments::Get(const char * name, const unsigned long defaultValue)
{
  std::map<std::string, std::string>::iterator t = _arguments.find(name);
  if (t == _arguments.end())
     return defaultValue;
  return strtol(t->second.c_str(), NULL, 10);
}

template<>
double Arguments::Get(const char *name, const double defaultValue)
{
  std::map<std::string, std::string>::iterator t = _arguments.find(name);
  if (t == _arguments.end())
     return defaultValue;
  return strtod(t->second.c_str(), NULL);
}
  
template<>
const char * Arguments::Get(const char *name, const char * defaultValue)
{
  std::map<std::string, std::string>::iterator t = _arguments.find(name);
  if (t == _arguments.end())
     return defaultValue;
  return t->second.c_str();
}
