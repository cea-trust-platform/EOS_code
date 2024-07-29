#ifndef _TIMER_HXX
#define _TIMER_HXX

#include <string>
#include <iostream>

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#elif __cplusplus <= 199711L
#include <sys/time.h>
#else
#include <chrono>
#endif

class Timer
{
public:
  Timer(const char *s = 0L) : m_elapsed(0.0), m_running(false)
  {
    m_name = s ? s : "";
#if defined(WIN32) || defined(_WIN32)
    QueryPerformanceFrequency(&frequency);
#endif
  }

  inline void reinit()
  {
    m_elapsed = 0.0;
    m_running = false;
  }

  const std::string &name() const { return m_name; }
  std::string &name() { return m_name; }

  void start()
  {
    if (m_running == false)
    {
#if defined(WIN32) || defined(_WIN32)
      QueryPerformanceCounter(&startCount);
      m_start = startCount.QuadPart * (1.0 / frequency.QuadPart);
#elif __cplusplus <= 199711L
      gettimeofday(&m_start, NULL);
#else
      m_start = std::chrono::high_resolution_clock::now();
#endif
      m_running = true;
    }
  }

  void restart()
  {
    if (m_running == false)
    {
      reinit();
      start();
    }
  }

  void stop()
  {
    if (m_running)
    {
#if defined(WIN32) || defined(_WIN32)
      QueryPerformanceCounter(&endCount);
      m_end = endCount.QuadPart * (1.0 / frequency.QuadPart);
      m_elapsed += m_end - m_start;
#elif __cplusplus <= 199711L
      gettimeofday(&m_end, NULL);
      m_elapsed += (m_end.tv_sec - m_start.tv_sec) + 1e-6 * (m_end.tv_usec - m_start.tv_usec);
#else
      m_end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> diff = m_end - m_start;
      m_elapsed += diff.count();
#endif
      m_running = false;
    }
  }

  inline double elapsed() const { return m_elapsed; }

protected:
#if defined(WIN32) || defined(_WIN32)
  LARGE_INTEGER frequency;
  LARGE_INTEGER startCount;
  LARGE_INTEGER endCount;
  double m_start, m_end;
#elif __cplusplus <= 199711L
  struct timeval m_start, m_end;
#else
  std::chrono::time_point<std::chrono::high_resolution_clock> m_start, m_end;
#endif
  double m_elapsed;
  bool m_running;
  std::string m_name;
};

inline std::ostream & operator<< (std::ostream & f, const Timer & T)
{
  std::cout << "XXXXXX" << std::endl;
  f << T.name() << std::setw(12) << std::setprecision(6) << T.elapsed() << " s" << std::endl;
  return f;
}

#endif
