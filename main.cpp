#include <iostream>
#include "stc.hpp"

int main()
{
  using namespace chistyakov;

  double frequencyRate = 0;
	std::cout << "Input frequency rate in milliseconds: ";
	std::cin >> frequencyRate;

  size_t repNum = 0;
  std::cout << "Input number of repetitions: ";
  std::cin >> repNum;
  std::cout << "\n";

	if (!std::cin)
	{
		std::cerr << "Bad input! Try again\n";
		return 1;
	}

	Stc test;

	for (size_t i = 0; i < repNum; ++i)
	{
	  std::thread consumer_thread([&test] () { test.consumer(); });
    std::thread producer_thread([&test] (double frequencyRate) { test.producer(frequencyRate); }, std::ref(frequencyRate));

    if (i % 50 == 0)
    {
      std::cout << "number of repetitions: " << repNum - i << "\n";
    }

    consumer_thread.join();
    producer_thread.join();
  }

  std::vector <double> executionSpeed = test.getExecutionSpeed();

	double averagValue = 0;
	double maxS = 0;
	double minS = executionSpeed[0];

	for (size_t i = 0; i != executionSpeed.size(); ++i)
	{
	  maxS = std::max(maxS, executionSpeed[i]);
	  minS = std::min(minS, executionSpeed[i]);
		averagValue += executionSpeed[i];
	}

	averagValue /= executionSpeed.size();

	std::cout << "---RESULT---\n";
	std::cout << "Average execution speed: " << averagValue << " ms\n";
	std::cout << "Average execution speed __MAX__: " << maxS << " ms\n";
  std::cout << "Average execution speed __MIN__: " << minS << " ms\n";

  return 0;
}
