#ifndef __DUMP_HXX
#define __DUMP_HXX

#include <vector>
#include <fstream>

class cDump
{
public:
  cDump();
  ~cDump();

  void activate(bool withAddress = false);
  bool isActive() { return _active; }
  void dumpPos(const char *fileName, int line, const char *fct);

  template <typename T>
  void dump(const char *fileName, int line, const char *fct,
                   const char *varName, const T *pvalue)
  {
    if (_active)
    {
      int iTh = _dump(fileName, line, fct);

      _fDump[iTh] << ": " << varName << " " << *pvalue;
      if (_withAddress[iTh])
        _fDump[iTh] << " (" << pvalue << ")";
      _fDump[iTh] << std::endl;
    }
  }

private:
  bool _active;
  std::vector<std::ofstream> _fDump;
  std::vector<bool> _withAddress;
  int _dump(const char *fileName, int line, const char *fct);
};

extern cDump Dump;

#ifdef __cplusplus
extern "C"
{
#endif

  void dumppos_(const char *fileName, long *line, const char *fct, long l1, long l2);

  void dumpdouble_(const char *fileName, int *line, const char *fct,
                   const char *varName, double *value, long l1, long l2, long l3);

  void dumpint_(const char *fileName, int *line, const char *fct,
                const char *varName, int *value, long l1, long l2, long l3);

  void dumplong_(const char *fileName, int *line, const char *fct,
                 const char *varName, long *value, long l1, long l2, long l3);

#ifdef __cplusplus
}
#endif

#endif
