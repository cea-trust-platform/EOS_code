#ifndef _ARGUMENTS_HXX
#define _ARGUMENTS_HXX

#include <map>
#include <string>
#include <vector>

class Arguments {

public:

  Arguments(int argc, char ** argv);

  template <typename T>
  T Get(const char *name, const T defaultValue);

private:

  std::map<std::string, std::string> _arguments;

};


#endif
