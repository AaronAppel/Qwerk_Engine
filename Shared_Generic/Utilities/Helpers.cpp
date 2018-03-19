#include "Helpers.h"
#include <assert.h>

#ifdef _WIN32
#include <Windows.h>
double helpers_Time()
{
	unsigned __int64 freq;
	unsigned __int64 time;

	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&time);

	double timeseconds = (double)time / freq;

	return timeseconds;
}
#else
double helpers_Time() { return 0.0f; }
#endif // _WIN32

char* helpers_DeepCopyString(const char* original)
{
	return _strdup(original);
	//char* copy = new char[strlen(original) + 1]; // memory allocation
	//memcpy(copy, original, strlen(original) + 1);

	//return copy;
}

static unsigned int g_UniqueID = 0; // static counter to increment for unique value
int helpers_GetUniqueID()
{
	return g_UniqueID++;
}

void helpers_Toggle(bool* value)
{
	*value = !value; // flip from on/off
}

void* VoidCheck(void* check)
{
	assert(check == nullptr);
	return check;
}