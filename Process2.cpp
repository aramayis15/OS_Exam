#include "stdafx.h"
#include <iostream>
#include <vector>
#include <random>
#include <ctime>

using std::cout;
using std::endl;

std::vector<int> vector(100, 1);
CRITICAL_SECTION crForVector;


DWORD WINAPI KillEnemy(void*)
{
	TCHAR buf[10];
	if (GetEnvironmentVariable(_T("2to1"), buf, 10) == 0)
	{
		cout << "Process2 is unable to get environment variable: " << GetLastError() << endl;
		exit(1);
	}

	int i = _ttoi(buf);
	HANDLE h = (HANDLE)i;

	while (true)
	{
		Sleep(2);
		SetEvent(h);
	}

}


DWORD WINAPI Delete(void*)
{
	TCHAR buf[10];
	if (GetEnvironmentVariable(_T("1to2"), buf, 10) == 0)
	{
		cout << "Process1 is unable to get environment variable: " << GetLastError() << endl;
		exit(1);
	}

	int i = _ttoi(buf);
	HANDLE WaitForDead = (HANDLE)i;

	TCHAR m[10];
	if (GetEnvironmentVariable(_T("myymutex"), m, 10) == 0)
	{
		cout << "Process2 is unable to get environment variable: " << GetLastError() << endl;
		exit(1);
	}

	HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, m);
	if (mutex == INVALID_HANDLE_VALUE)
	{
		cout << "Process2 is unable to open mutex: " << GetLastError() << endl;
		exit(1);
	}

	while (true)
	{
		srand(time(0));
		int r = 5+(rand() % 10);

		WaitForSingleObject(WaitForDead, INFINITE);
		EnterCriticalSection(&crForVector);
		for (int i = 0; i < r; i++)
		{
			if (vector.size() > 0)
			{
				vector.pop_back();
				WaitForSingleObject(mutex, INFINITE);
				std::cout << "Group_2      --1      " << vector.size() << std::endl;
				ReleaseMutex(mutex);
			}
			else
			{
				ExitThread(1);
			}
		}
		LeaveCriticalSection(&crForVector);
		ResetEvent(WaitForDead);
	}
}


DWORD WINAPI Add(void*)
{
	TCHAR buf[10];
	if (GetEnvironmentVariable(_T("myymutex"), buf, 10) == 0)
	{
		cout << "Process2 is unable to get environment variable: " << GetLastError() << endl;
		exit(1);
	}

	HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, buf);
	if (mutex == INVALID_HANDLE_VALUE)
	{
		cout << "Process2 is unable to open mutex: " << GetLastError() << endl;
		exit(1);
	}

	while (true)
	{
		srand(time(0));
		int r = rand() % 15;

		for (int i = 0; i < r; ++i)
		{
			EnterCriticalSection(&crForVector);
			if (vector.size() < 1)
			{
				ExitThread(1);
			}
			vector.push_back(1);
			WaitForSingleObject(mutex, INFINITE);
			std::cout << "Group_2      ++1      " << vector.size() << std::endl;
			ReleaseMutex(mutex);
			LeaveCriticalSection(&crForVector);
		}
	}
}


int main()
{

	// Critical section for shared vector
	InitializeCriticalSection(&crForVector);


	/////////////////////////////////// Opening mutex //////////////////////////////////////////

#pragma region MyRegion

	TCHAR buf[10];
	if (GetEnvironmentVariable(_T("myymutex"), buf, 10) == 0)
	{
		cout << "Process2 is unable to get environment variable: " << GetLastError() << endl;
		exit(1);
	}

	HANDLE mutex = OpenMutex(SYNCHRONIZE, FALSE, buf);
	if (mutex == INVALID_HANDLE_VALUE)
	{
		cout << "Process2 is unable to open mutex: " << GetLastError() << endl;
		exit(1);
	}

#pragma endregion

	////////////////////////////////////////////////////////////////////////////////////////////

	
	// Create threads for increasing and decreasing the count of army (vector)
	DWORD ID1, ID2, ID3;
	HANDLE Th1 = CreateThread(NULL, 0, KillEnemy, NULL, 0, &ID1);
	HANDLE Th2 = CreateThread(NULL, 0, Add, NULL, 0, &ID2);
	HANDLE Th3 = CreateThread(NULL, 0, Delete, NULL, 0, &ID3);

	HANDLE arr[] = { Th1,Th2,Th3 };
	WaitForMultipleObjects(3, arr, FALSE, INFINITE);


	// Kill all threads
	TerminateThread(Th1, 2);
	TerminateThread(Th2, 2);
	TerminateThread(Th3, 2);


	// Group_1 will lost if one of threads ends
	WaitForSingleObject(mutex, INFINITE);
	std::cout << "Group_2 Lost :( \n";
	ReleaseMutex(mutex);


	// Close all handles
	CloseHandle(mutex);
	CloseHandle(Th1);
	CloseHandle(Th2);
	CloseHandle(Th3);


	return 0;
}

