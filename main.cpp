#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
#include <time.h>
#include <limits>
#include <mutex>
#include <fstream>

std::mutex mtx;
std::condition_variable cv;
bool canFreeStore = false;

struct data_t
{
	size_t* ptr;
	double size;
	bool isError;
};

void createData(std::queue <data_t> &dataQueue, double & sizeOfFile, const size_t frequencyRate)
{
	const size_t sizeOfBufferInMgb = 256; // Макс. - 256 МБ
	const size_t bytesInMgb = 1048576; // Количество байт в 1ом МБ 
	size_t* buffer = (size_t*)malloc(sizeOfBufferInMgb * bytesInMgb);
	data_t packet = { nullptr, 0, true };

	if (buffer == NULL)
	{
		// отсановка всех процессов
		std::cerr << "Memory allocation error\n";
		dataQueue.push(packet);
		return;
	}

	size_t index = 0;
	size_t num = 0; // "пустое" значение, которым будет заполнять массив
	size_t max_size = std::numeric_limits< size_t >::max();

	std::unique_lock<std::mutex> lck(mtx); // Блокируем доступ другим потокам во избежании ошибок

	while (sizeOfFile > 0)
	{
		if (!dataQueue.empty() && dataQueue.back().isError)
		{
			break;
		}

		if (--sizeOfFile > 1)
		{
			// "Создаем" 1 Мб 
			for (size_t i = 0; i < bytesInMgb / sizeof(size_t); ++i)
			{
				if (num == max_size)
				{
					num = 0;
				}

				if (index == (sizeOfBufferInMgb * bytesInMgb / sizeof(size_t) - 1))
				{
					index = 0;
				}

				buffer[index++] = num++;
			}

			packet.ptr = &buffer[index - bytesInMgb / sizeof(size_t)];
			packet.size = 1;
		}
		else
		{
			// "Создаем" остаточные данные 
			for (size_t i = 0; i < (size_t)(sizeOfFile * bytesInMgb / sizeof(size_t)) + 1; ++i)
			{
				if (num == max_size)
				{
					num = 0;
				}

				if (index == (sizeOfBufferInMgb * bytesInMgb / sizeof(size_t) - 1))
				{
					index = 0;
				}

				buffer[index++] = num++;
			}

			packet.ptr = &buffer[index - bytesInMgb / sizeof(size_t)];
			packet.size = sizeOfFile;
			dataQueue.push({ nullptr, 0, true });
		}
		
		if (dataQueue.size() < 256)
		{
			packet.isError = false;
			dataQueue.push(packet);
		}
		else
		{
			packet.isError = true;
			dataQueue.push(packet);

			std::cerr << "\n !!! The queue is full, we are losing data !!! \n";
			while (!canFreeStore) cv.wait(lck); // блокировка процесса, пока не закончится запись в файл
			free(buffer);
			return;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(frequencyRate));
	}

	while (!canFreeStore) cv.wait(lck); // блокировка процесса, пока не закончится запись в файл
	free(buffer);
}

void writeData(std::queue <data_t>& dataQueue, std::vector <double>& executioSpeed, double & sizeOfFile)
{
	std::ofstream file;
	file.open("output.txt");
	if (!file.is_open())
	{
		dataQueue.push({ nullptr, 0, true });
		std::cerr << "Couldn't open the file\n";
		return;
	}

	const size_t bytesInMgb = 1048576;
	
	while (true)
	{
		canFreeStore = false;
		cv.notify_one();

		while (!dataQueue.empty())
		{

			if (dataQueue.back().isError)
			{
				canFreeStore = true;
				cv.notify_one();
				return;
			}

			clock_t start = clock();

			for (size_t i = 0; i < dataQueue.front().size * bytesInMgb / sizeof(size_t); ++i)
			{
				file << *(dataQueue.front().ptr + i);
			}

			clock_t end = clock();
			double milliseconds = end - start;
			executioSpeed.push_back(milliseconds);

			dataQueue.pop();
		}

		canFreeStore = true;
		cv.notify_one();

		if (dataQueue.empty() && sizeOfFile < 1)
		{
			break;
		}
	}

	file.close();
}

void sendInformation(std::queue <data_t>& dataQueue, std::vector <double>& executioSpeed, double & sizeOfFile)
{
	while (sizeOfFile > 1 || !dataQueue.empty())
	{
		if (!dataQueue.empty() && dataQueue.back().isError)
		{
			return;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));

		double averagValue = 0;
		for (size_t i = 0; i != executioSpeed.size(); ++i)
		{
			averagValue += executioSpeed[i];
		}

		averagValue /= executioSpeed.size();

		std::cout << "\n";
		std::cout << "Queue size at the moment: " << dataQueue.size() << "\n";
		std::cout << "Average execution speed: " << averagValue << "\n";
		std::cout << "Another " << sizeOfFile << " Mgb will be recorded" << "\n";
	}
}