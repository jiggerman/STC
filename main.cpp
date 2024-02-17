#include <iostream>
#include <cstddef>

struct data_t
{
	size_t* ptr;
	double size;
	bool isError;
};

int main()
{
  std::cout << "Hello, world!\n";
  std::cout << sizeof(size_t);
  return 0;
}
