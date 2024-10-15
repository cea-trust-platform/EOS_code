#ifndef _ARGUMENTS_HXX
#define _ARGUMENTS_HXX

#include <map>
#include <string>
#include <vector>
#include <iostream>

class Arguments {

public:

  Arguments(int argc, char ** argv);

  template <typename T>
  T Get(const char *name, const T defaultValue);

  void print(std::ostream & f);

private:

  std::map<std::string, std::string> _arguments;
  std::map<std::string, std::pair<std::string, std::string> > _options;

};

inline std::ostream & operator<< (std::ostream & f, Arguments & a)
{
  a.print(f);
  return f;
}

#endif
