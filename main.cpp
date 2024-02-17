#include <iostream>
#include <cstddef>

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

	size_t index = 0;
	size_t num = 0; // "пустое" значение, которым будет заполнять массив

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
			free(buffer);
			return;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(frequencyRate));
	}

	free(buffer);
}