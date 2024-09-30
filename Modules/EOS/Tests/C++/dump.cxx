
#ifdef _OPENMP
#include <omp.h>
#endif

#include "dump.hxx"
#include <fstream>
#include <string>
#include <cstring>

cDump::cDump() : _active(false)
{
}

cDump::~cDump()
{
  for (auto & f : _fDump)
    if (f.is_open())
      f.close();
}

void cDump::activate(bool withAddress)
{

#ifdef _OPENMP

  int iTh, nTh = 1;
  nTh = omp_get_max_threads();
  _fDump.resize(nTh);
  _withAddress.resize(nTh);

#pragma omp parallel for
  for (iTh = 0; iTh < nTh; iTh++)
  {
#pragma omp critical
    {
      _withAddress[iTh] = withAddress;
    }
  }
#else
  _fDump.resize(1);
  _withAddress.resize(1);
  _withAddress[0] = withAddress;

#endif

  _active = true;
}

cDump Dump;

void basename(char *b, const char *d)
{
  const char *p = strrchr(d, '/');
  if (p)
  {
    strcpy(b, p + 1);
  }
  else
  {
    strcpy(b, d);
  }
}

#include <iomanip>

int cDump::_dump(const char *fileName, int line, const char *fct)
{
  if (_active)
  {
    char b[200];

#ifdef _OPENMP
    int iTh = omp_get_thread_num();
#else
    int iTh = 0;
#endif

    if (! _fDump[iTh].is_open())
    {
      std::string nameDump = "dump_" + std::to_string(iTh) + ".txt";
      _fDump[iTh].open(nameDump.c_str());

    }
    basename(b, fileName);
    _fDump[iTh] << std::setw(20) << b << " " << std::setw(4) << line << " : " << std::setw(35) << fct;
    return iTh;
  }
  return 0;
}

void cDump::dumpPos(const char *fileName, int line, const char *fct)
{
  if (_active)
  {
    int iTh = _dump(fileName, line, fct);
    _fDump[iTh] << std::endl;
  }
}

void dumppos_(const char *fileName, long *line, const char *fct, long l1, long l2)
{
  char s1[500], s2[500];
  memcpy(s1, fileName, l1);
  s1[l1] = '\0';
  memcpy(s2, fct, l2);
  s2[l2] = '\0';
  Dump.dumpPos(s1, *line, s2);
}

  void dumpdouble_(const char *fileName, int *line, const char *fct,
                   const char *varName, double *value, long l1, long l2, long l3)
  {
    if (Dump.isActive())
    {
      char s1[500], s2[500], s3[500];
      memcpy(s1, fileName, l1);
      s1[l1] = '\0';
      memcpy(s2, fct, l2);
      s2[l2] = '\0';
      memcpy(s3, varName, l3);
      s3[l3] = '\0';

      Dump.dump(s1, *line, s2, s3, value);
    }
  }

  void dumpint_(const char *fileName, int *line, const char *fct,
                const char *varName, int *value, long l1, long l2, long l3)
  {
    if (Dump.isActive())
    {
      char s1[500], s2[500], s3[500];
      memcpy(s1, fileName, l1);
      s1[l1] = '\0';
      memcpy(s2, fct, l2);
      s2[l2] = '\0';
      memcpy(s3, varName, l3);
      s3[l3] = '\0';

      Dump.dump(s1, *line, s2, s3, value);
    }
  }

  void dumplong_(const char *fileName, int *line, const char *fct,
                 const char *varName, long *value, long l1, long l2, long l3)
  {
    if (Dump.isActive())
    {
      char s1[500], s2[500], s3[500];
      memcpy(s1, fileName, l1);
      s1[l1] = '\0';
      memcpy(s2, fct, l2);
      s2[l2] = '\0';
      memcpy(s3, varName, l3);
      s3[l3] = '\0';

      Dump.dump(s1, *line, s2, s3, value);
    }
  }
