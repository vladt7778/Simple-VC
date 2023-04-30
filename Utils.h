#pragma once
#include <string>

namespace Utils
{
	template<class T>
	void Print(T val)
	{
		std::string temp = to_string(val);
		MessageBox(0, temp, "Value", MB_OK);
	}

	void Print(const char* val)
	{
		MessageBox(0, val, "Value", MB_OK);
	}

	int random(int min, int max)
	{
		return min + (rand() % (max - min + 1));
	}
}