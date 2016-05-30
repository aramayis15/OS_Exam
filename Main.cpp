#include "stdafx.h"
#include <iostream>

using std::cout;
using std::endl;


int main()
{

	//////////////////////////// Mutex Creation ///////////////////////////////////

#pragma region MutexCreation

	HANDLE mutexHandle = CreateMutex(NULL, FALSE, _T("exammutex"));
	if (mutexHandle == INVALID_HANDLE_VALUE)
	{
		cout << "Unable to create mutex: " << GetLastError() << endl;
		exit(1);
	}

	if (!SetEnvironmentVariable(_T("myymutex"), _T("exammutex")))
	{
		cout << "Unable to set environment variable: " << GetLastError() << endl;
		exit(1);
	}

#pragma endregion

	///////////////////////////////////////////////////////////////////////////////



	//////////////////////////// Event Creation ///////////////////////////////////

#pragma region EventCreation

	SECURITY_ATTRIBUTES sa1To2;
	sa1To2.nLength = sizeof(sa1To2);
	sa1To2.bInheritHandle = TRUE;
	sa1To2.lpSecurityDescriptor = NULL;

	HANDLE event1To2 = CreateEvent(&sa1To2, FALSE, FALSE, _T("FirstTo"));
	if (event1To2 == INVALID_HANDLE_VALUE)
	{
		cout << "Unable to create event : " << GetLastError() << endl;
		exit(1);
	}

	int e = (int)event1To2;
	TCHAR buf[10];
	_itot_s(e, buf, 10);
	SetEnvironmentVariable(_T("1to2"), buf);


	SECURITY_ATTRIBUTES sa2To1;
	sa2To1.nLength = sizeof(sa2To1);
	sa2To1.bInheritHandle = TRUE;
	sa2To1.lpSecurityDescriptor = NULL;

	HANDLE event2To1 = CreateEvent(&sa2To1, FALSE, FALSE, _T("SecondTo"));
	if (event2To1 == INVALID_HANDLE_VALUE)
	{
		cout << "Unable to create event : " << GetLastError() << endl;
		exit(1);
	}

	e = (int)event2To1;
	buf[10];
	_itot_s(e, buf, 10);
	SetEnvironmentVariable(_T("2to1"), buf);

#pragma endregion

	///////////////////////////////////////////////////////////////////////////////



	////////////////////////////  Process Creation ////////////////////////////////

#pragma region ProcessCreation

	PROCESS_INFORMATION p1, p2;
	STARTUPINFO s1, s2;

	ZeroMemory(&s1, sizeof(s1));
	s1.cb = sizeof(s1);
	ZeroMemory(&s2, sizeof(s2));
	s2.cb = sizeof(s2);

	ZeroMemory(&p1, sizeof(p1));
	ZeroMemory(&p2, sizeof(p2));

	TCHAR process1Arg[] = _T("Process1.exe");
	TCHAR process2Arg[] = _T("Process2.exe");


	if (!CreateProcess(NULL, process1Arg, NULL, NULL, TRUE, 0, NULL, NULL, &s1, &p1))
	{
		cout << "Unable to start Process1: " << GetLastError() << endl;
		exit(1);
	}
	if (!CreateProcess(NULL, process2Arg, NULL, NULL, TRUE, 0, NULL, NULL, &s2, &p2))
	{
		cout << "Unable to start Process2: " << GetLastError() << endl;
		exit(1);
	}

#pragma endregion

	///////////////////////////////////////////////////////////////////////////////

	
	// Wait for war to end
	HANDLE arr[] = { p1.hProcess, p2.hProcess };
	WaitForMultipleObjects(2, arr, FALSE, INFINITE);

	// Close all Proccesses
	TerminateProcess(p1.hProcess, 2);
	TerminateProcess(p2.hProcess, 2);

	// Close all handles
	CloseHandle(mutexHandle);
	CloseHandle(event1To2);
	CloseHandle(event2To1);
	CloseHandle(p1.hProcess);
	CloseHandle(p2.hProcess);

	return 0;
}

