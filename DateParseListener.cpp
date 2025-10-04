#include "DateParseListener.h"

/**
 * コンストラクタ
 */
DateParseListener::DateParseListener(void)
{
	memset(&this->tmDate, 0, sizeof(struct tm));
	this->tm_gmtoffset = 0;
}

/**
 * デストラクタ
 */
DateParseListener::~DateParseListener(void)
{
}

/**
 * orig-date
 * "Date:" date-time CRLF
 *    or
 * obs-orig-date
 * "Date" *WSP ":" date-time CRLF
 *
 * date-time
 * [ day-of-week "," ] date time [CFWS]
 */
bool DateParseListener::hookParse(std::string strHeader, MailHeaderField::MHFTYPE type,
								  std::ios::pos_type posBeg, std::ios::pos_type posEnd)
{
	static std::ios::pos_type begToken, endToken;
	if (posBeg == posEnd) {
		switch (type) {
		case MailHeaderField::T_DAY:
		case MailHeaderField::T_MONTH:
		case MailHeaderField::T_YEAR:
		case MailHeaderField::T_HOUR:
		case MailHeaderField::T_MINUTE:
		case MailHeaderField::T_SECOND:
		case MailHeaderField::T_ZONE:
			begToken = posBeg;
			break;
		}
		return true;
	} else {
		endToken = posEnd;
		std::string strToken = strHeader.substr(begToken, endToken - begToken);
		std::stringstream pszGetF;
		switch (type) {
		case MailHeaderField::T_DAY:
			tmDate.tm_mday = atoi(strToken.c_str());
			break;
		case MailHeaderField::T_MONTH: {
			std::locale loc("English");
			const std::time_get<char>& tmget = std::use_facet<std::time_get<char>>(loc);
			std::istream::_Iter i = pszGetF.rdbuf();
			std::istream::_Iter eofi;
			std::ios_base::iostate err = 0;
			pszGetF << strToken;
			tmget.get_monthname(i, eofi, pszGetF, err, &tmDate);
			break;
			}
		case MailHeaderField::T_YEAR: {
			int year = atoi(strToken.c_str());
			if (year >= 1900) {
				year -= 1900;
			} else if (year < 50) {
				year += 100;
			}
			tmDate.tm_year = year;
			break;
			}
		case MailHeaderField::T_HOUR:
			tmDate.tm_hour = atoi(strToken.c_str());
			break;
		case MailHeaderField::T_MINUTE:
			tmDate.tm_min = atoi(strToken.c_str());
			break;
		case MailHeaderField::T_SECOND:
			tmDate.tm_sec = atoi(strToken.c_str());
			break;
		case MailHeaderField::T_ZONE: {
			int offsetMin = 0;
			int zone = 0;
			int matched = sscanf_s(strToken.c_str(), "%05d", &zone);
			if (matched == 1) {  // オフセットが時刻で表わされている場合
				offsetMin = ((long int)(zone / 100) * 60) + (zone % 100);
			} else {             // オフセットがゾーンで表わされている場合
				struct _zone_info {
					const char *name;
					long int offsetMin;
				} zone_info[] = {
					{"UT",  0 },     {"GMT", 0},
					{"EST", 5 * 60}, {"EDT", 4 * 60},
					{"CST", 6 * 60}, {"CDT", 5 * 60},
					{"MST", 7 * 60}, {"MDT", 6 * 60},
					{"PST", 8 * 60}, {"PDT", 7 * 60},
				};
				for (int i = 0; i < sizeof(zone_info) / sizeof(zone_info[0]); i++) {
					if (strcmp(strToken.c_str(), zone_info[i].name) == 0) {
						offsetMin = zone_info[i].offsetMin;
						break;
					}
				}
				if (strToken.size() == 1) {
					char c = *strToken.c_str();
					if (isalpha(c) && tolower(c) <= 'k') {
						offsetMin = (-1  - (c - 'a')) * 60;
					} else if (isalpha(c) && tolower(c) <= 'm') {
						offsetMin = (-10 - (c - 'k')) * 60;
					} else if (isalpha(c) && tolower(c) <= 'y') {
						offsetMin = (1   + (c - 'n')) * 60;
					}
				}
			}
			tm_gmtoffset = offsetMin;
			break;
			}
		}
	}
	return true;
}

/**
 * 解析した時刻を UTC 時刻からの経過時間を返す
 */
time_t DateParseListener::mkTime(void)
{
	time_t t = mktime(&(this->tmDate));
	if (t < 0) {
		return t;
	}
	return t + this->tm_gmtoffset;
}
