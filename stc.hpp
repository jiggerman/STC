#ifndef STC_HPP
#define STC_HPP

#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <fstream>

namespace chistyakov
{
  const int BITES_IN_MGB = 1048576;

  class Stc
  {
    public:
      Stc();
      ~Stc();

      void producer(uint64_t frequencyRate);
      void consumer();
      std::vector <double> getExecutionSpeed();

    private:
      std::mutex mtx_;
      std::condition_variable cv_;
      std::ofstream file_;
      std::vector <double> executionSpeed;
      bool data_ready_;
      uint64_t * buffer;
  };
}

#endif
