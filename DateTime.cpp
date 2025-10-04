#include "DateTime.h"

/**
 * コンストラクタ
 */
DateTime::DateTime(void)
{
}

/**
 * デストラクタ
 */
DateTime::~DateTime(void)
{
}

/**
 * 文字列から日付データを解析
 */
void DateTime::Parse(LPCTSTR szField, LPCTSTR szDateFormat, struct tm *tmDate, long int& tm_gmtoff)
{
	// not implemented
}

/**
 * 文字列から日付データを解析
 */
void DateTime::Parse(LPCTSTR szField, DTFORMAT format, struct tm *tmDate, long int& tm_gmtoff)
{
	switch (format) {
		case DTF_RFC2822:  // メールヘッダに記載される日付フォーマット(RFC2822)
			parseDate_RFC2822(szField, tmDate, tm_gmtoff);
			break;

		case DTF_RFC5822:  // メールヘッダに記載される日付フォーマット(RFC5822)
			parseDate_RFC5822(szField, tmDate, tm_gmtoff);
			break;

		default: // unknown format
			throw "DateTime::Parse: Unknown date time format is specified.";
			return;
	}
}

/**
 * 文字列から日付の日を解析する
 */
basic_istream<char>::_Iter DateTime::parseDate_mday(basic_istream<char>::_Iter in,
													basic_istream<char>::_Iter end,
													ios_base& str, 
													ios_base::iostate& err, int& mday)
{
	basic_istream<char>::_Iter i;
	unsigned int mdaybuf = 0;
	locale loc;
	const num_get<char>& numget = use_facet<num_get<char>>(loc);
	char buf[256] = "";

	i = numget.get(in, end, str, err, mdaybuf);
	if (!(err & ios_base::failbit)) {  // 数値データの解析成功
		if (!(1 <= mdaybuf && mdaybuf <= 31)) {
			err |= std::ios_base::failbit;
		} else {
			mday = mdaybuf;
		}
	}
	return i;
}

/**
 * 文字列から日付の月を解析する
 */
basic_istream<char>::_Iter DateTime::parseDate_month(basic_istream<char>::_Iter in,
													 basic_istream<char>::_Iter end,
													 ios_base& str,
													 ios_base::iostate& err, int& month)
{
	basic_istream<char>::_Iter i;
	unsigned int monthbuf = 0;
	locale loc;
	const num_get<char>& numget = use_facet<num_get<char>>(loc);

	i = numget.get(in, end, str, err, monthbuf);
	if (!(err & ios_base::failbit)) {  // 数値データの解析成功
		if (!(1 <= monthbuf && monthbuf <= 12)) {
			err |= std::ios_base::failbit;
		} else {
			month = monthbuf;
		}
	}
	return i;
}

/**
 * 文字列から日付の年を解析する
 */
basic_istream<char>::_Iter DateTime::parseDate_year(basic_istream<char>::_Iter in,
													basic_istream<char>::_Iter end,
													ios_base& str,
													ios_base::iostate& err, int& year)
{
	basic_istream<char>::_Iter i;
	unsigned int yearbuf = 0;
	locale loc;
	const num_get<char>& numget = use_facet<num_get<char>>(loc);

	i = numget.get(in, end, str, err, yearbuf);
	if (!(err & ios_base::failbit)) {  // 数値データの解析成功
		year = yearbuf - 1900;
	}
	return i;
}

/**
 * 文字列から時刻の時/分/秒を解析する
 */
basic_istream<char>::_Iter DateTime::parseTime_elem(basic_istream<char>::_Iter in,
													basic_istream<char>::_Iter end,
													ios_base& str,
													ios_base::iostate& err, int& elem)
{
	basic_istream<char>::_Iter i;
	unsigned int elembuf = 0;
	locale loc;
	const num_get<char>& numget = use_facet<num_get<char>>(loc);

	i = numget.get(in, end, str, err, elembuf);
	if (!(err & ios_base::failbit)) {  // 数値データの解析成功
		if (!(0 <= elembuf && elembuf <= 59)) {
			err |= std::ios_base::failbit;
		} else {
			elem = elembuf;
		}
	}
	return i;
}

/**
 * 文字列から時刻の時を解析する
 */
basic_istream<char>::_Iter DateTime::parseTime_hour(basic_istream<char>::_Iter in,
													basic_istream<char>::_Iter end,
													ios_base& str,
													ios_base::iostate& err, int& hour)
{
	return parseTime_elem(in, end, str, err, hour);
}

/**
 * 文字列から時刻の分を解析する
 */
basic_istream<char>::_Iter DateTime::parseTime_minute(basic_istream<char>::_Iter in,
													  basic_istream<char>::_Iter end,
													  ios_base& str,
													  ios_base::iostate& err, int& minute)
{
	return parseTime_elem(in, end, str, err, minute);
}

/**
 * 文字列から時刻の秒を解析する
 */
basic_istream<char>::_Iter DateTime::parseTime_second(basic_istream<char>::_Iter in,
													  basic_istream<char>::_Iter end,
													  ios_base& str,
													  ios_base::iostate& err, int& second)
{
	return parseTime_elem(in, end, str, err, second);
}

/**
 * 文字列からタイムゾーンを解析する
 */
basic_istream<char>::_Iter DateTime::parseTime_tmzone(basic_istream<char>::_Iter in,
													  basic_istream<char>::_Iter end,
													  ios_base& str,
													  ios_base::iostate& err, long int& tmzone)
{
	basic_istream<char>::_Iter i = in;
	basic_stringstream<char> strTMZone;
	int tmzonebuf = 0;
	int len = 0;

	if (!(*i == '+' || *i == '-')) {
		err |= ios_base::failbit;
		return i;
	}
	strTMZone << *i;
	i++;

	for (; i != end; i++) {
		if (len >= 4)
			break;
		if (!isdigit(*i))
			break;
		strTMZone << *i;
		len++;
	}
	if (len != 4) {
		err |= ios_base::failbit;
		return i;
	}

	strTMZone >> tmzonebuf;
	tmzonebuf = ((long int)(tmzonebuf / 100)) * 60 + (tmzonebuf % 100);
	tmzone = tmzonebuf;
	return i;
}

/**
 * parseDate_RFC2822
 *
 * RFC2822
 *     date-time       =       [ day-of-week "," ] date time [CFWS]
 *     day-of-week     =       ([FWS] day-name) / obs-day-of-week
 *     day-name        =       "Mon" / "Tue" / "Wed" / "Thu" /
 *                             "Fri" / "Sat" / "Sun"
 *     date            =       day month year
 *     year            =       4*DIGIT / obs-year
 *     month           =       (FWS month-name FWS) / obs-month
 *     month-name      =       "Jan" / "Feb" / "Mar" / "Apr" /
 *                             "May" / "Jun" / "Jul" / "Aug" /
 *                             "Sep" / "Oct" / "Nov" / "Dec"
 *     day             =       ([FWS] 1*2DIGIT) / obs-day
 *     time            =       time-of-day FWS zone
 *     time-of-day     =       hour ":" minute [ ":" second ]
 *     hour            =       2DIGIT / obs-hour
 *     minute          =       2DIGIT / obs-minute
 *     second          =       2DIGIT / obs-second
 *     zone            =       (( "+" / "-" ) 4DIGIT) / obs-zone
 */
void DateTime::parseDate_RFC2822(LPCTSTR szField, struct tm *tmDate, long int& tm_gmtoff)
{
	locale loc("English");
	basic_stringstream<char> pszGetF;
	const time_get<char>& tmget = use_facet<time_get<char>>(loc);
	const num_get<char>& numget = use_facet<num_get<char>>(loc);
	basic_istream<char>::_Iter i = pszGetF.rdbuf();
	basic_istream<char>::_Iter eofi;
	ios_base::iostate err = 0;

	pszGetF << szField;

	/* 曜日を解析（オプショナル） */
	i = tmget.get_weekday(i, eofi, pszGetF, err, tmDate);
	if (!(err & ios_base::failbit)) {  // 曜日の解析成功
		if (*i++ != ',') {
			// [TODO] 解析エラー処理
			return;
		}
		while (i != eofi && isspace(*i)) i++;
	}
	/* 日を解析 */
	err = 0;
	i = parseDate_mday(i, eofi, pszGetF, err, tmDate->tm_mday);
	if (err & ios_base::failbit) {
		// [TODO] 解析エラー処理
	}
	while (i != eofi && isspace(*i)) i++;
	/* 月（英語名）を解析 */
	err = 0;
	i = tmget.get_monthname(i, eofi, pszGetF, err, tmDate);
	if (err & ios_base::failbit) {
		// [TODO] 解析エラー処理
	}
	while (i != eofi && isspace(*i)) i++;
	/* 年（西暦）を解析 */
	err = 0;
	i = parseDate_year(i, eofi, pszGetF, err, tmDate->tm_year);
	if (err & ios_base::failbit) {
		// [TODO] 解析エラー処理
	}
	while (i != eofi && isspace(*i)) i++;

	/* 時を解析 */
	err = 0;
	i = parseTime_hour(i, eofi, pszGetF, err, tmDate->tm_hour);
	if (err & ios_base::failbit) {
		// [TODO] 解析エラー処理
	}
	if (*i++ != ':') return;
	/* 分を解析 */
	err = 0;
	i = parseTime_minute(i, eofi, pszGetF, err, tmDate->tm_min);
	if (err & ios_base::failbit) {
		// [TODO] 解析エラー処理
	}
	/* 秒を解析（オプショナル） */
	err = 0;
	if (*i++ == ':') {
		i = parseTime_second(i, eofi, pszGetF, err, tmDate->tm_sec);
		if (err & ios_base::failbit) {
			// [TODO] 解析エラー処理
		}
	}
	while (i != eofi && isspace(*i)) i++;

	/* タイムゾーンを解析 */
	i = parseTime_tmzone(i, eofi, pszGetF, err, tm_gmtoff);
	if (err & ios_base::failbit) {
		// [TODO] 解析エラー処理
	}
}





/**
 * WSP
 * SP / HTAB
 */
bool DateTime::parseWSP(std::istringstream& iss, std::string& strField)
{
	const char *szWSP = " \t";
	char c = iss.peek();
	const char *test = strchr(szWSP, iss.peek());
	return (strchr(szWSP, iss.peek()) != NULL && iss.seekg(1, std::ios::cur));
}

/**
 * CRLF
 */
bool DateTime::parseCRLF(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "\r\n") != 0) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	return true;
}

/**
 * DIGIT
 */
bool DateTime::parseDIGIT(std::istringstream& iss, std::string& strField)
{
	return (isdigit(iss.peek()) != 0 && iss.seekg(1, std::ios::cur));
}

/**
 * VCHAR
 * %x21-7E  ; 可視 (印字) 文字  = %d33-126
 */
bool DateTime::parseVCHAR(std::istringstream& iss, std::string& strField)
{
	int n = iss.peek();
	return (0x21 <= n && n <= 0x7E && iss.seekg(1, std::ios::cur));
}

/**
 * NO-WS-CTL
 * %d1-8 / %d11 / %d12 / %d14-31 / %d127
 */
bool DateTime::parseNOWSCTL(std::istringstream& iss, std::string& strField)
{
	const char *szWS = "\t\n\r";
	int n = iss.peek();
	if ((1 <= n && n <= 31 && strchr(szWS, n) == NULL) || n == 127) {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * quoted-pair
 * ("\" (VCHAR / WSP)) / obs-qp
 */
bool DateTime::parseQuotedPair(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (iss.peek() == '\\' && iss.seekg(1, std::ios::cur)
	   && (parseVCHAR(iss, strField) || parseWSP(iss, strField))) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	if (parseObsQp(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * FWS
 * ([*WSP CRLF] 1*WSP) / obs-FWS
 */
bool DateTime::parseFWS(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (parseObsFWS(iss, strField)) {
		return true;
	} else {
		if (parseWSP(iss, strField)) {
			while (parseWSP(iss, strField));
			if (! parseCRLF(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			if (! parseWSP(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			while (parseWSP(iss, strField));
			return true;
		} else if (parseCRLF(iss, strField)) {
			if (! parseWSP(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			while (parseWSP(iss, strField));
			return true;
		}
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	return false;
}

/**
 * ctext
 * %d33-39 / %d42-91 / %d93-126 / obs-ctext
 */
bool DateTime::parseCtext(std::istringstream& iss, std::string& strField)
{
	int n = iss.peek();
	if (33 <= n && n <= 126 && strchr("()\\", n) == NULL) {
		iss.seekg(1, std::ios::cur);
		return true;
	} else if (parseObsCtext(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * ccontent
 * ctext / quoted-pair / comment
 */
bool DateTime::parseCcontent(std::istringstream& iss, std::string& strField)
{
	if (parseCtext(iss, strField) 
	   && parseQuotedPair(iss, strField)
	   && parseComment(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * comment
 * "(" *([FWS] ccontent) [FWS] ")"
 */
bool DateTime::parseComment(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (iss.peek() == '(' && iss.seekg(1, std::ios::cur)) {
		while (! iss.eof()) {
			parseFWS(iss, strField);
			if (iss.peek() == ')') {
				iss.seekg(1, std::ios::cur);
				return true;
			} else if (! parseCcontent(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * CFWS
 * (1*([FWS] comment) [FWS]) / FWS
 */
bool DateTime::parseCFWS(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (parseFWS(iss, strField)) {
		if (! parseComment(iss, strField)) {
			return true;
		}
		while (true) {
			parseFWS(iss, strField);
			if (! parseComment(iss, strField)) {
				break;
			}
		}
		parseFWS(iss, strField);
		return true;
	} else {
		if (! parseComment(iss, strField)) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		while (true) {
			parseFWS(iss, strField);
			if (! parseComment(iss, strField)) {
				break;
			}
		}
		parseFWS(iss, strField);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * date-time
 * [ day-of-week "," ] date time [CFWS]
 */
bool DateTime::parseDateTime(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	if (parseDayOfWeek(iss, strField, tmDate, tm_gmtoff)) {
		if (iss.peek() != ',') {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		iss.seekg(1, std::ios::cur);
	}
	if (parseDate(iss, strField, tmDate, tm_gmtoff) && parseTime(iss, strField, tmDate, tm_gmtoff)) {
		parseCFWS(iss, strField);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * day-of-week
 * ([FWS] day-name) / obs-day-of-week
 */
bool DateTime::parseDayOfWeek(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	if (parseFWS(iss, strField)) {
		if (parseDayName(iss, strField, tmDate, tm_gmtoff)) {
			return true;
		}
	} else {
		if (parseDayName(iss, strField, tmDate, tm_gmtoff)) {
			return true;
		} else if (parseObsDayOfWeek(iss, strField, tmDate, tm_gmtoff)) {
			return true;
		}
	}
	return false;
}

/**
 * day-name
 * "Mon" / "Tue" / "Wed" / "Thu" / "Fri" / "Sat" / "Sun"
 */
bool DateTime::parseDayName(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	const char* szDayName[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
	for (int i = 0; i < sizeof(szDayName) / sizeof(szDayName[0]); i++) {
		if (strncmp(szDayName[i], strField.c_str(), strlen(szDayName[i])) == 0) {
			iss.seekg(strlen(szDayName[i]), std::ios::cur);
			if (tmDate) {
				tmDate->tm_wday = i;
			}
			return true;
		}
	}
	return false;
}

/**
 * date
 * day month year
 */
bool DateTime::parseDate(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	if (parseDay(iss, strField, tmDate, tm_gmtoff)
		&& parseMonth(iss, strField, tmDate, tm_gmtoff) 
		&& parseYear(iss, strField, tmDate, tm_gmtoff)) {
		return true;
	}
	return false;
}

/**
 * day
 * ([FWS] 1*2DIGIT FWS) / obs-day
 */
bool DateTime::parseDay(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	if (parseFWS(iss, strField)) {
		std::ios::pos_type begDay = iss.tellg();
		if (parseDIGIT(iss, strField) && parseDIGIT(iss, strField)) {
			if (! parseFWS(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			if (tmDate) {
				char szDigit[3];
				memset(szDigit, 0, sizeof(szDigit));
				iss.read(szDigit, iss.tellg() - begDay);
				tmDate->tm_mday = atoi(szDigit);
			}
			return true;
		}
	} else if (parseObsDay(iss, strField, tmDate, tm_gmtoff)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * month
 * "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", 
 * "Sep", "Oct", "Nov", "Dec"
 */
bool DateTime::parseMonth(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	const char* szMonthName[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	std::ios::pos_type pos = iss.tellg();
	char buf[4];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	for (int i = 0; i < sizeof(szMonthName) / sizeof(szMonthName[0]); i++) {
		if (strncmp(szMonthName[i], buf, strlen(szMonthName[i])) == 0) {
			if (tmDate) {
				tmDate->tm_mon = i;
			}
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * year
 * (FWS 4*DIGIT FWS) / obs-year
 */
bool DateTime::parseYear(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	if (parseFWS(iss, strField)) {
		char buf[5];
		memset(buf, 0, sizeof(buf));
		iss.read(buf, sizeof(buf) - 1);
		int year = 0;
		int nMatched = sscanf_s(buf, "%04d", &year);
		if (nMatched == 1) {
			if (parseFWS(iss, strField)) {
				if (tmDate) {
					tmDate->tm_year = atoi(buf) - 1900;
				}
				return true;
			}
		}
		iss.seekg(pos, std::ios::beg);
	}

	if (parseObsYear(iss, strField, tmDate, tm_gmtoff)) {
		return true;
	}
	return false;
}

/**
 * time
 * time-of-day zone
 */
bool DateTime::parseTime(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	if (parseTimeOfDay(iss, strField, tmDate, tm_gmtoff) 
		&& parseZone(iss, strField, tmDate, tm_gmtoff)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * time-of-day
 * hour ":" minute [ ":" second ]
 */
bool DateTime::parseTimeOfDay(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	if (parseHour(iss, strField, tmDate, tm_gmtoff) && iss.peek() == ':') {
		iss.seekg(1, std::ios::cur);
		if (parseMinute(iss, strField, tmDate, tm_gmtoff)) {
			if (iss.peek() == ':') {
				iss.seekg(1, std::ios::cur);
				if (! parseSecond(iss, strField, tmDate, tm_gmtoff)) {
					iss.seekg(pos, std::ios::beg);
					return false;
				}
			}
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * hour
 * 2DIGIT / obs-hour
 */
bool DateTime::parseHour(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int hour = 0;
	int matched = sscanf_s(buf, "%02d", &hour);
	if (matched == 1) {
		if (tmDate) {
			tmDate->tm_hour = atoi(buf);
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	
	if (parseObsHour(iss, strField, tmDate, tm_gmtoff)) {
		return true;
	}
	return false;
}

/**
 * minute
 * 2DIGIT / obs-minute
 */
bool DateTime::parseMinute(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int minute = 0;
	int matched = sscanf_s(buf, "%02d", &minute);
	if (matched == 1) {
		if (tmDate) {
			tmDate->tm_min = atoi(buf);
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	
	if (parseObsMinute(iss, strField, tmDate, tm_gmtoff)) {
		return true;
	}
	return false;
}

/**
 * second
 * 2DIGIT / obs-second
 */
bool DateTime::parseSecond(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int second = 0;
	int matched = sscanf_s(buf, "%02d", &second);
	if (matched == 1) {
		if (tmDate) {
			tmDate->tm_sec = atoi(buf);
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	
	if (parseObsSecond(iss, strField, tmDate, tm_gmtoff)) {
		return true;
	}
	return false;
}

/**
 * zone
 * (FWS ( "+" / "-" ) 4DIGIT) / obs-zone
 */
bool DateTime::parseZone(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	if (parseFWS(iss, strField)) {
		char cPM = iss.peek();
		if (cPM == '+' || cPM == '-') {
			char buf[5];
			memset(buf, 0, sizeof(buf));
			iss.read(buf, sizeof(buf) - 1);
			int zone = 0;
			int matched = sscanf_s(buf, "%04d", &zone);
			if (matched == 1) {
				if (tm_gmtoff) {
					tm_gmtoff = atoi(buf);
					tm_gmtoff = ((long int)(tm_gmtoff / 100)) * 60 + (tm_gmtoff % 100);
					tm_gmtoff = (cPM == '+' ? tm_gmtoff : -1 * tm_gmtoff);
				}
				return true;
			}
		}
		iss.seekg(pos, std::ios::beg);
	}

	if (parseObsZone(iss, strField, tmDate, tm_gmtoff)) {
		return true;
	}
	return false;
}

/**
 * obs-NO-WS-CTL
 * %d1-8 / %d11 / %d12 / %d14-31 / %d127  ; 復帰、改行、空白を除くUS-ASCII 制御文字
 */
bool DateTime::parseObsNOWSCTL(std::istringstream& iss, std::string& strField)
{
	const char *szWS = "\t\n\r";
	int n = iss.peek();
	if ((1 <= n && n <= 31 && strchr(szWS, n) == NULL) || n == 127) {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * obs-ctext
 * obs-NO-WS-CTL
 */
bool DateTime::parseObsCtext(std::istringstream& iss, std::string& strField)
{
	return parseObsNOWSCTL(iss, strField);
}

/**
 * obs-qp
 * "\" (%d0 / obs-NO-WS-CTL / LF / CR)
 */
bool DateTime::parseObsQp(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (iss.peek() == '\\' && iss.seekg(1, std::ios::cur)) {
		if (iss.eof()
		   || parseNOWSCTL(iss, strField)
		   || (iss.peek() == '\n' && iss.seekg(1, std::ios::cur))
		   || (iss.peek() == '\r' && iss.seekg(1, std::ios::cur))) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-FWS
 * 1*WSP *(CRLF 1*WSP)
 */
bool DateTime::parseObsFWS(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (parseWSP(iss, strField)) {
		while (parseWSP(iss, strField));
		while (true) {
			if (! parseCRLF(iss, strField)) {
				break;
			}
			if (! parseWSP(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			while (parseWSP(iss, strField));
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-day-of-week
 * [CFWS] day-name [CFWS]
 */
bool DateTime::parseObsDayOfWeek(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	parseCFWS(iss, strField);
	if (! parseDayName(iss, strField, tmDate, tm_gmtoff)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	parseCFWS(iss, strField);
	return true;
}

/**
 * obs-day
 * [CFWS] 1*2DIGIT [CFWS]
 */
bool DateTime::parseObsDay(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	parseCFWS(iss, strField);
	std::ios::pos_type begDay = iss.tellg();
	if (parseDIGIT(iss, strField)) {
		parseDIGIT(iss, strField);
		if (tmDate) {
			char szBuf[3];
			memset(szBuf, 0, sizeof(szBuf));
			std::ios::pos_type endDay = iss.tellg();
			iss.seekg(begDay, std::ios::beg);
			iss.read(szBuf, endDay - begDay);
			tmDate->tm_mday = atoi(szBuf);
		}
		parseCFWS(iss, strField);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-year
 * [CFWS] 2*DIGIT [CFWS]
 */
bool DateTime::parseObsYear(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	parseCFWS(iss, strField);
	std::ios::pos_type begYear = iss.tellg();
	if (! (parseDIGIT(iss, strField) && parseDIGIT(iss, strField))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	while (parseDIGIT(iss, strField));
	if (tmDate) {
		char szBuf[32];
		memset(szBuf, 0, sizeof(szBuf));
		std::ios::pos_type endYear = iss.tellg();
		iss.seekg(begYear, std::ios::beg);
		iss.read(szBuf, endYear - begYear);
		int year = atoi(szBuf);
		if (year < 50) {
			tmDate->tm_year = year + 100;
		} else {
			tmDate->tm_year = year;
		}
	}
	parseCFWS(iss, strField);
	return true;
}

/**
 * obs-hour
 * [CFWS] 2DIGIT [CFWS]
 */
bool DateTime::parseObsHour(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	parseCFWS(iss, strField);
	std::ios::pos_type begHour = iss.tellg();
	if (! (parseDIGIT(iss, strField) && parseDIGIT(iss, strField))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	if (tmDate) {
		char szBuf[3];
		memset(szBuf, 0, sizeof(szBuf));
		std::ios::pos_type endHour = iss.tellg();
		iss.seekg(begHour, std::ios::beg);
		iss.read(szBuf, endHour - begHour);
		tmDate->tm_hour = atoi(szBuf);
	}
	parseCFWS(iss, strField);
	return true;
}

/**
 * obs-minute
 * [CFWS] 2DIGIT [CFWS]
 */
bool DateTime::parseObsMinute(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	parseCFWS(iss, strField);
	std::ios::pos_type begMinute = iss.tellg();
	if (! (parseDIGIT(iss, strField) && parseDIGIT(iss, strField))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	if (tmDate) {
		char szBuf[3];
		memset(szBuf, 0, sizeof(szBuf));
		std::ios::pos_type endMinute = iss.tellg();
		iss.seekg(begMinute, std::ios::beg);
		iss.read(szBuf, endMinute - begMinute);
		tmDate->tm_min = atoi(szBuf);
	}
	parseCFWS(iss, strField);
	return true;
}

/**
 * obs-second
 * [CFWS] 2DIGIT [CFWS]
 */
bool DateTime::parseObsSecond(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	parseCFWS(iss, strField);
	std::ios::pos_type begSecond = iss.tellg();
	if (! (parseDIGIT(iss, strField) && parseDIGIT(iss, strField))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	if (tmDate) {
		char szBuf[3];
		memset(szBuf, 0, sizeof(szBuf));
		std::ios::pos_type endSecond = iss.tellg();
		iss.seekg(begSecond, std::ios::beg);
		iss.read(szBuf, endSecond - begSecond);
		tmDate->tm_sec = atoi(szBuf);
	}
	parseCFWS(iss, strField);
	return true;
}

/**
 * obs-zone
 * "UT" / "GMT" / "EST" / "EDT" / "CST" /
 * "CDT" / "MST" / "MDT" / "PST" / "PDT" /
 * %d65-73 / %d75-90 / %d97-105 / %d107-122
 */
bool DateTime::parseObsZone(std::istringstream& iss, std::string& strField, struct tm *tmDate, long int& tm_gmtoff)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[4];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, 2);
	if (strcmp(buf, "UT") == 0) {
		return true;
	}

	const char *arrZone[] = { "GMT", "EST", "EDT", "CST", 
	                          "CDT", "MST", "MDT", "PST", "PDT" };
	iss.read(&buf[2], 1);
	for (int i = 0; i < sizeof(arrZone) / sizeof(arrZone[0]); i++) {
		if (strcmp(buf, arrZone[i]) == 0) {
			return true;
		}
	}
	if (isalpha(buf[0]) && buf[0] != 'j' && buf[0] != 'J') {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * parseDate_RFC5822
 *
 * RFC5822
 *     date-time       =   [ day-of-week "," ] date time [CFWS]
 *     day-of-week     =   ([FWS] day-name) / obs-day-of-week
 *     day-name        =   "Mon" / "Tue" / "Wed" / "Thu" /
 *                         "Fri" / "Sat" / "Sun"
 *     date            =   day month year
 *     day             =   ([FWS] 1*2DIGIT FWS) / obs-day
 *     month           =   "Jan" / "Feb" / "Mar" / "Apr" /
 *                         "May" / "Jun" / "Jul" / "Aug" /
 *                         "Sep" / "Oct" / "Nov" / "Dec"
 *     year            =   (FWS 4*DIGIT FWS) / obs-year
 *     time            =   time-of-day zone
 *     time-of-day     =   hour ":" minute [ ":" second ]
 *     hour            =   2DIGIT / obs-hour
 *     minute          =   2DIGIT / obs-minute
 *     second          =   2DIGIT / obs-second
 *     zone            =   (FWS ( "+" / "-" ) 4DIGIT) / obs-zone
 */
void DateTime::parseDate_RFC5822(LPCTSTR szField, struct tm *tmDate, long int& tm_gmtoff)
{
	struct tm tmDateBuf;
	long int tm_gmtoffBuf = 0;
	std::basic_string<TCHAR> strField(szField);
	std::istringstream iss(strField);
	memset(&tmDateBuf, 0, sizeof(tmDateBuf));
	bool isValidFormat = parseDateTime(iss, strField, &tmDateBuf, tm_gmtoffBuf);
	if (isValidFormat) {
		*tmDate = tmDateBuf;
		tm_gmtoff = tm_gmtoffBuf;
	}
}

