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

#include <iomanip>

void Arguments::print(std::ostream & f)
{
  f << "usage : \n";

  for (const auto &p : _options)
    std::cout << "  " << std::setw(10) << p.first 
              << "  (type : " << p.second.first 
              << ", default : " << p.second.second << ")\n";
  
  f << std::endl;
}

#include <algorithm>

template<>
bool Arguments::Get(const char * name, const bool defaultValue)
{
  _options[name] = std::make_pair("<bool>", std::to_string(defaultValue));

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
  _options[name] = std::make_pair("<int>", std::to_string(defaultValue));

  std::map<std::string, std::string>::iterator t = _arguments.find(name);
  if (t == _arguments.end())
     return defaultValue;
  return strtol(t->second.c_str(), NULL, 10);
}

template<>
unsigned long Arguments::Get(const char * name, const unsigned long defaultValue)
{
  _options[name] = std::make_pair("<long>", std::to_string(defaultValue));

  std::map<std::string, std::string>::iterator t = _arguments.find(name);
  if (t == _arguments.end())
     return defaultValue;
  return strtol(t->second.c_str(), NULL, 10);
}

template<>
double Arguments::Get(const char *name, const double defaultValue)
{
  _options[name] = std::make_pair("<double>", std::to_string(defaultValue));

  std::map<std::string, std::string>::iterator t = _arguments.find(name);
  if (t == _arguments.end())
     return defaultValue;
  return strtod(t->second.c_str(), NULL);
}
  
template<>
const char * Arguments::Get(const char *name, const char * defaultValue)
{
  _options[name] = std::make_pair("<string>", std::string(defaultValue));

  std::map<std::string, std::string>::iterator t = _arguments.find(name);
  if (t == _arguments.end())
     return defaultValue;
  return t->second.c_str();
}
