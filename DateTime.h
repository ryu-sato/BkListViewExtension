#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <locale>
#include <iostream>
#include <sstream>

using namespace std;

class DateTime
{
public:
	typedef enum {
		DTF_RFC2822, DTF_RFC5822
	} DTFORMAT;

	DateTime(void);
	~DateTime(void);

	void Parse(LPCTSTR szField, LPCTSTR szDateFormat, struct tm *tmDate, long int& tm_gmtoff);
	void Parse(LPCTSTR szField, DTFORMAT format, struct tm *tmDate, long int& tm_gmtoff);

private:
	/* 文字列から日付の日を解析する */
	basic_istream<char>::_Iter parseDate_mday(basic_istream<char>::_Iter in,
		basic_istream<char>::_Iter end, ios_base& str, 
		ios_base::iostate& err, int& mday);
	/* 文字列から日付の月を解析する */
	basic_istream<char>::_Iter parseDate_month(basic_istream<char>::_Iter in,
		basic_istream<char>::_Iter end, ios_base& str,
		ios_base::iostate& err, int& month);
	/* 文字列から日付の年を解析する */
	basic_istream<char>::_Iter parseDate_year(basic_istream<char>::_Iter in,
		basic_istream<char>::_Iter end,ios_base& str,
		ios_base::iostate& err, int& year);
	/* 文字列から時刻の時/分/秒を解析する */
	basic_istream<char>::_Iter parseTime_elem(basic_istream<char>::_Iter in,
		basic_istream<char>::_Iter end,ios_base& str,
		ios_base::iostate& err, int& elem);
	/* 文字列から時刻の時を解析する */
	basic_istream<char>::_Iter parseTime_hour(basic_istream<char>::_Iter in,
		basic_istream<char>::_Iter end,ios_base& str,
		ios_base::iostate& err, int& hour);
	/* 文字列から時刻の分を解析する */
	basic_istream<char>::_Iter parseTime_minute(basic_istream<char>::_Iter in,
		basic_istream<char>::_Iter end,ios_base& str,
		ios_base::iostate& err, int& minute);
	/* 文字列から時刻の秒を解析する */
	basic_istream<char>::_Iter parseTime_second(basic_istream<char>::_Iter in,
		basic_istream<char>::_Iter end,ios_base& str,
		ios_base::iostate& err, int& second);
	/* 文字列からタイムゾーンを解析する */
	basic_istream<char>::_Iter parseTime_tmzone(basic_istream<char>::_Iter in,
		basic_istream<char>::_Iter end,ios_base& str,
		ios_base::iostate& err, long int& tmzone);
	/* parseDate_RFC2822 */
	void parseDate_RFC2822(LPCTSTR szField, struct tm *tmDate, long int& tm_gmtoff);

	bool parseWSP(std::istringstream& iss, std::string& strField);
	bool parseDIGIT(std::istringstream& iss, std::string& strField);
	bool parseVCHAR(std::istringstream& iss, std::string& strField);
	bool parseNOWSCTL(std::istringstream& iss, std::string& strField);
	bool parseCRLF(std::istringstream& iss, std::string& strField);
	bool parseQuotedPair(std::istringstream& iss, std::string& strField);
	bool parseFWS(std::istringstream& iss, std::string& strField);
	bool parseCtext(std::istringstream& iss, std::string& strField);
	bool parseCcontent(std::istringstream& iss, std::string& strField);
	bool parseComment(std::istringstream& iss, std::string& strField);
	bool parseCFWS(std::istringstream& iss, std::string& strField);

	bool parseDateTime(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseDayOfWeek(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseDayName(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseDate(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseDay(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseMonth(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseYear(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseTime(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseTimeOfDay(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseHour(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseMinute(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseSecond(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseZone(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);

	bool parseObsNOWSCTL(std::istringstream& iss, std::string& strField);
	bool parseObsCtext(std::istringstream& iss, std::string& strField);
	bool parseObsQp(std::istringstream& iss, std::string& strField);
	bool parseObsFWS(std::istringstream& iss, std::string& strField);

	bool parseObsDayOfWeek(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseObsDay(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseObsYear(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseObsHour(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseObsMinute(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseObsSecond(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);
	bool parseObsZone(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff);

	void parseDate_RFC5822(LPCTSTR szField, struct tm *tmDate, long int& tm_gmtoff);
};
