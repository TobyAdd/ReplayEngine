#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

namespace Console
{
	extern bool console_inited;
	void Write(string text);
	void WriteLine(string text);
	string Input();
	void Clear();
	void Init();
}