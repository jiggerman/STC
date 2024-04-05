#include "stc.hpp"
#include <iostream>

chistyakov::Stc::Stc()
{
  data_ready_ = false;
  buffer = new uint64_t[BITES_IN_MGB / sizeof(uint64_t)]{};
  file_.open("data.txt");

  if (!file_.is_open())
  {
    throw std::logic_error("File is close...");
  }
}

chistyakov::Stc::~Stc()
{
  delete [] buffer;
  file_.close();
}

void chistyakov::Stc::producer(uint64_t frequencyRate)
{
  std::lock_guard<std::mutex> lock(mtx_);

  auto start = std::chrono::system_clock::now();

  for (uint64_t i = 0; i < BITES_IN_MGB / sizeof(uint64_t); ++i)
  {
    buffer[i] = i;
  }

  auto end = std::chrono::system_clock::now();
  auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  data_ready_ = true;
  cv_.notify_one();
  std::this_thread::sleep_for(std::chrono::milliseconds(frequencyRate - duration_ms.count()));
}

void chistyakov::Stc::consumer()
{
  std::unique_lock<std::mutex> lock(mtx_);
  cv_.wait(lock, [this] { return this->data_ready_; });
  auto start = std::chrono::system_clock::now();

  for (size_t i = 0; i < BITES_IN_MGB / sizeof(uint64_t); ++i)
  {
    file_ << buffer[i];
  }

  auto end = std::chrono::system_clock::now();
  auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  executionSpeed.push_back(duration_ms.count());
}

std::vector <double> chistyakov::Stc::getExecutionSpeed()
{
  return executionSpeed;
}
