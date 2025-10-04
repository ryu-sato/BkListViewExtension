#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <time.h>

using namespace std;

/**
 * ログ出力機能
 */
class Logger
{
public:
	static Logger& getInstance();
	void print(LPCTSTR strMsg);
	void print(std::basic_string<TCHAR> strMsg);
	static void invalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);
protected:
	std::string strPath;
	std::string strPluginName;
private:
	Logger();
	~Logger();
};

