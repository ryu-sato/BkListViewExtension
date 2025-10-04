/* RFC2822準拠 */
#include <time.h>

#include "Mail.h"
#include "DateTime.h"

/**
 * メールヘッダクラス
 */
/**
 * コンストラクタ
 */
MailHeader::MailHeader(void)
{
}

/**
 * コンストラクタ
 */
MailHeader::MailHeader(std::string strHeader)
{
	this->setHeader(strHeader);
}

/**
 * デストラクタ
 */
MailHeader::~MailHeader(void)
{
}

/**
 * []演算子のオーバーロード
 */
std::list<std::pair<std::string, std::string>> MailHeader::operator[](const TCHAR* szFieldName)
{
	return this->getField(szFieldName);
}

/**
 * ヘッダを取得
 */
const std::string MailHeader::getHeader() const
{
	std::string strHeader;
	for (std::list<std::pair<std::string, std::string>>::const_iterator i = this->listField.begin(); i != this->listField.end(); i++) {
		strHeader += i->first;
		strHeader += ": ";
		strHeader += i->second;
		strHeader += "\r\n";
	}
	return strHeader;
}

/**
 * DQUOTE
 */
bool MailHeader::lexIsDQUOTE(std::istringstream& iss, std::string& strField)
{
	return (iss.peek() == '\"' && iss.seekg(1, std::ios::cur));
}

/**
 * WSP
 * SP / HTAB
 */
bool MailHeader::lexIsWSP(std::istringstream& iss, std::string& strField)
{
	const char *szWSP = " \t";
	return (strchr(szWSP, iss.peek()) != NULL && iss.seekg(1, std::ios::cur));
}

/**
 * CRLF
 */
bool MailHeader::lexIsCRLF(std::istringstream& iss, std::string& strField)
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
 * ALPHA
 */
bool MailHeader::lexIsALPHA(std::istringstream& iss, std::string& strField)
{
	return (isalpha(iss.peek()) != 0 && iss.seekg(1, std::ios::cur));
}

/**
 * DIGIT
 */
bool MailHeader::lexIsDIGIT(std::istringstream& iss, std::string& strField)
{
	return (isdigit(iss.peek()) != 0 && iss.seekg(1, std::ios::cur));
}

/**
 * NO-WS-CTL
 * %d1-8 / %d11 / %d12 / %d14-31 / %d127
 */
bool MailHeader::lexIsNOWSCTL(std::istringstream& iss, std::string& strField)
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
 * text
 * %d1-9 / %d11 / %d12 / %d14-127 / obs-text
 */
bool MailHeader::lexIsText(std::istringstream& iss, std::string& strField)
{
	int n = iss.peek();
	if ((1 <= n && n <= 127 && n != '\n' && n != '\r' && iss.seekg(1, std::ios::cur))
		|| lexIsObsText(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * specials
 * "(" / ")" / "<" / ">" / "[" / "]" /
 * ":" / ";" / "@" / "\" / "," / "." / DQUOTE
 */
bool MailHeader::lexIsSpecials(std::istringstream& iss, std::string& strField)
{
	const char *szSpecials = "()<>[]:;@\\,.";
	if ((strchr(szSpecials, iss.peek()) != NULL && iss.seekg(1, std::ios::cur))
		|| lexIsDQUOTE(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * quoted-pair
 * ("\" text) / obs-qp
 */
bool MailHeader::lexIsQuotedPair(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if ((iss.peek() == '\\' && iss.seekg(1, std::ios::cur))
	    || lexIsObsQp(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * FWS
 * ([*WSP CRLF] 1*WSP) / obs-FWS
 */
bool MailHeader::lexIsFWS(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsObsFWS(iss, strField)) {
		return true;
	} else {
		if (lexIsWSP(iss, strField)) {
			while (lexIsWSP(iss, strField));
			if (! lexIsCRLF(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			if (! lexIsWSP(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			while (lexIsWSP(iss, strField));
			return true;
		} else if (lexIsCRLF(iss, strField)) {
			if (! lexIsWSP(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			while (lexIsWSP(iss, strField));
			return true;
		}
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	return false;
}

/**
 * ctext
 * NO-WS-CTL / %d33-39 / %d42-91 / %d93-126
 */
bool MailHeader::lexIsCtext(std::istringstream& iss, std::string& strField)
{
	int n = iss.peek();
	if (lexIsNOWSCTL(iss, strField)) {
		return true;
	} else if (33 <= n && n <= 126 && strchr("()\\", n) == NULL) {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * ccontent
 * ctext / quoted-pair / comment
 */
bool MailHeader::lexIsCcontent(std::istringstream& iss, std::string& strField)
{
	if (lexIsCtext(iss, strField) 
	   && lexIsQuotedPair(iss, strField)
	   && lexIsComment(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * comment
 * "(" *([FWS] ccontent) [FWS] ")"
 */
bool MailHeader::lexIsComment(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (iss.peek() == '(' && iss.seekg(1, std::ios::cur)) {
		while (! iss.eof()) {
			lexIsFWS(iss, strField);
			if (iss.peek() == ')') {
				iss.seekg(1, std::ios::cur);
				return true;
			} else if (! lexIsCcontent(iss, strField)) {
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
 * *([FWS] comment) (([FWS] comment) / FWS)
 */
bool MailHeader::lexIsCFWS(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	while (true) {
		lexIsFWS(iss, strField);
		if (! lexIsComment(iss, strField)) {
			break;
		}
	}
	if (lexIsFWS(iss, strField)) {
		lexIsComment(iss, strField);
		return true;
	} else {
		if (lexIsComment(iss, strField)) {
			return true;
		}
	}
	return false;
}

/**
 * atext
 * ALPHA / DIGIT / "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" /
 * "/" / "=" / "?" / "^" / "_" / "`" / "{" / "|" / "}" / "~"
 */
bool MailHeader::lexIsAtext(std::istringstream& iss, std::string& strField)
{
	const char *szTextPart = "!#$%&'*+-/=?^_`{|}~";
	if (lexIsALPHA(iss, strField) || lexIsDIGIT(iss, strField)) {
		return true;
	} else if (strchr(szTextPart, iss.peek()) != NULL) {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * atom
 * [CFWS] 1*atext [CFWS]
 */
bool MailHeader::lexIsAtom(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	if (! lexIsAtext(iss, strField)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	while (lexIsAtext(iss, strField));
	lexIsCFWS(iss, strField);
	return true;
}

/**
 * dot-atom
 * [CFWS] dot-atom-text [CFWS]
 */
bool MailHeader::lexIsDotAtom(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	if (! lexIsDotAtomText(iss, strField)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	lexIsCFWS(iss, strField);
	return true;
}

/**
 * dot-atom-text
 * 1*atext *("." 1*atext)
 */
bool MailHeader::lexIsDotAtomText(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsAtext(iss, strField)) {
		while (lexIsAtext(iss, strField));
		while (true) {
			if (! (iss.peek() == '.' && iss.seekg(1, std::ios::cur))) {
				break;
			}
			if (! lexIsAtext(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			while (lexIsAtext(iss, strField));
		}
	}
	return true;
}

/**
 * qtext
 * NO-WS-CTL / %d33 / %d35-91 / %d93-126  ; %d34 = '"', %d92 = '\'
 */
bool MailHeader::lexIsQtext(std::istringstream& iss, std::string& strField)
{
	int n = iss.peek();
	if (lexIsNOWSCTL(iss, strField)) {
		return true;
	} else if (33 <= n && n <= 126 && n != '\"' && n != '\\') {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * qcontent
 * qtext / quoted-pair
 */
bool MailHeader::lexIsQcontent(std::istringstream& iss, std::string& strField)
{
	if (lexIsQtext(iss, strField) || lexIsQuotedPair(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * quoted-string
 * [CFWS] DQUOTE *([FWS] qcontent) [FWS] DQUOTE [CFWS]
 */
bool MailHeader::lexIsQuotedString(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	if (lexIsDQUOTE(iss, strField)) {
		while (true) {
			lexIsFWS(iss, strField);
			if (! lexIsQcontent(iss, strField)) {
				break;
			}
		}
		lexIsFWS(iss, strField);
		if (! lexIsDQUOTE(iss, strField)) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		lexIsCFWS(iss, strField);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * word
 * atom / quoted-string
 */
bool MailHeader::lexIsWord(std::istringstream& iss, std::string& strField)
{
	if (lexIsAtom(iss, strField) || lexIsQuotedString(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * phrase
 * 1*word / obs-phrase
 */
bool MailHeader::lexIsPhrase(std::istringstream& iss, std::string& strField)
{
	if (lexIsWord(iss, strField)) {
		while (lexIsWord(iss, strField));
		return true;
	} else if (lexIsObsPhrase(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * utext
 * NO-WS-CTL / %d33-126 / obs-utext
 */
bool MailHeader::lexIsUtext(std::istringstream& iss, std::string& strField)
{
	if (lexIsNOWSCTL(iss, strField) || lexIsObsUtext(iss, strField)) {
		return true;
	} else if (33 <= iss.peek() && iss.peek() <= 126) {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * unstructured
 * *([FWS] utext) [FWS]
 */
bool MailHeader::lexIsUnstructured(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	while (! iss.eof()) {
		if (lexIsFWS(iss, strField)) {
			if (! lexIsUtext(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		} else if (! lexIsUtext(iss, strField)) {
			break;
		}
	}
	lexIsFWS(iss, strField);
	return true;
}

/**
 * date-time
 * [ day-of-week "," ] date FWS time [CFWS]
 */
bool MailHeader::lexIsDateTime(std::istringstream& iss, std::string& strField)
{
	if (lexIsDayOfWeek(iss, strField)) {
		if (iss.peek() != ',') {
			return false;
		}
		iss.seekg(1, std::ios::cur);
	}
	if (lexIsDate(iss, strField) && lexIsFWS(iss, strField) && lexIsTime(iss, strField)) {
		lexIsCFWS(iss, strField);
		return true;
	}
	return false;
}

/**
 * day-of-week
 * ([FWS] day-name) / obs-day-of-week
 */
bool MailHeader::lexIsDayOfWeek(std::istringstream& iss, std::string& strField)
{
	if (lexIsFWS(iss, strField)) {
		if (lexIsDayName(iss, strField)) {
			return true;
		}
	} else {
		if (lexIsDayName(iss, strField)) {
			return true;
		} else if (lexIsObsDayOfWeek(iss, strField)) {
			return true;
		}
	}
	return false;
}

/**
 * day-name
 * "Mon" / "Tue" / "Wed" / "Thu" / "Fri" / "Sat" / "Sun"
 */
bool MailHeader::lexIsDayName(std::istringstream& iss, std::string& strField)
{
	const char* szDayName[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
	for (int i = 0; i < sizeof(szDayName) / sizeof(szDayName[0]); i++) {
		if (strncmp(szDayName[i], strField.c_str(), strlen(szDayName[i])) == 0) {
			iss.seekg(strlen(szDayName[i]), std::ios::cur);
			return true;
		}
	}
	return false;
}

/**
 * date
 * day month year
 */
bool MailHeader::lexIsDate(std::istringstream& iss, std::string& strField)
{
	if (lexIsDay(iss, strField) && lexIsMonth(iss, strField) && lexIsYear(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * year
 * 4*DIGIT / obs-year
 */
bool MailHeader::lexIsYear(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[5];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int year = 0;
	int nMatched = sscanf_s(buf, "%04d", &year);
	if (nMatched == 1) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);

	if (lexIsObsYear(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * month
 * (FWS month-name FWS) / obs-month
 */
bool MailHeader::lexIsMonth(std::istringstream& iss, std::string& strField)
{
	if (lexIsFWS(iss, strField) && lexIsMonthName(iss, strField) && lexIsFWS(iss, strField)) {
		return true;
	}
	if (lexIsObsMonth(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * month-name
 * "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
 */
bool MailHeader::lexIsMonthName(std::istringstream& iss, std::string& strField)
{
	const char* szMonthName[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	std::ios::pos_type pos = iss.tellg();
	for (int i = 0; i < sizeof(szMonthName) / sizeof(szMonthName[0]); i++) {
		char buf[4];
		memset(buf, 0, sizeof(buf));
		iss.read(buf, sizeof(buf) - 1);
		if (strncmp(szMonthName[i], buf, strlen(szMonthName[i])) == 0) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * day
 * ([FWS] 1*2DIGIT) / obs-day
 */
bool MailHeader::lexIsDay(std::istringstream& iss, std::string& strField)
{
	if (lexIsFWS(iss, strField)) {
		if (isdigit(iss.peek())) {
			iss.seekg(1, std::ios::cur);
			if (isdigit(iss.peek())) {
				iss.seekg(1, std::ios::cur);
			}
			return true;
		}
	}
	if (isdigit(iss.peek())) {
		iss.seekg(1, std::ios::cur);
		if (isdigit(iss.peek())) {
			iss.seekg(1, std::ios::cur);
		}
		return true;
	}
	if (lexIsObsDay(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * time
 * time-of-day FWS zone
 */
bool MailHeader::lexIsTime(std::istringstream& iss, std::string& strField)
{
	if (lexIsTimeOfDay(iss, strField) && lexIsFWS(iss, strField) && lexIsZone(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * time-of-day
 * hour ":" minute [ ":" second ]
 */
bool MailHeader::lexIsTimeOfDay(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsHour(iss, strField) && iss.peek() == ':') {
		iss.seekg(1, std::ios::cur);
		if (lexIsMinute(iss, strField)) {
			if (iss.peek() == ':') {
				iss.seekg(1, std::ios::cur);
				if (! lexIsSecond(iss, strField)) {
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
bool MailHeader::lexIsHour(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int hour = 0;
	int matched = sscanf_s(buf, "02d", &hour);
	if (matched == 1) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	
	if (lexIsObsHour(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * minute
 * 2DIGIT / obs-minute
 */
bool MailHeader::lexIsMinute(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int minute = 0;
	int matched = sscanf_s(buf, "02d", &minute);
	if (matched == 1) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	
	if (lexIsObsMinute(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * second
 * 2DIGIT / obs-second
 */
bool MailHeader::lexIsSecond(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[3];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int second = 0;
	int matched = sscanf_s(buf, "02d", &second);
	if (matched == 1) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	
	if (lexIsObsSecond(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * zone
 * (( "+" / "-" ) 4DIGIT) / obs-zone
 */
bool MailHeader::lexIsZone(std::istringstream& iss, std::string& strField)
{
	if (! (iss.peek() == '+' || iss.peek() == '-')) {
		return false;
	}
	std::ios::pos_type pos = iss.tellg();
	char buf[5];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	int zone = 0;
	int matched = sscanf_s(buf, "04d", &zone);
	if (matched == 1) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);

	if (lexIsObsZone(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * address
 * mailbox / group
 */
bool MailHeader::lexIsAddress(std::istringstream& iss, std::string& strField)
{
	if (lexIsMailbox(iss, strField) || lexIsGroup(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * mailbox
 * name-addr / addr-spec
 */
bool MailHeader::lexIsMailbox(std::istringstream& iss, std::string& strField)
{
	if (lexIsNameAddr(iss, strField) || lexIsAddrSpec(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * name-addr
 * [display-name] angle-addr
 */
bool MailHeader::lexIsNameAddr(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsDisplayName(iss, strField);
	if (! lexIsAngleAddr(iss, strField)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	return true;
}

/**
 * angle-addr
 * [CFWS] "<" addr-spec ">" [CFWS] / obs-angle-addr
 */
bool MailHeader::lexIsAngleAddr(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsObsAngleAddr(iss, strField)) {
		return true;
	} else {
		lexIsCFWS(iss, strField);
		if (iss.peek() == '<' && iss.seekg(1, std::ios::cur)) {
			if (! (lexIsAddrSpec(iss, strField) && iss.peek() == '<'
				  && iss.seekg(1, std::ios::cur))) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			lexIsCFWS(iss, strField);
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * group
 * display-name ":" [mailbox-list / CFWS] ";" [CFWS]
 */
bool MailHeader::lexIsGroup(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (! (lexIsDisplayName(iss, strField) 
	      && iss.peek() == ':' && iss.seekg(1, std::ios::cur))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	if (! ((lexIsMailboxList(iss, strField) || lexIsCFWS(iss, strField))
	       && iss.peek() == ';' && iss.seekg(1, std::ios::cur))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	return true;
}

/**
 * display-name
 * phrase
 */
bool MailHeader::lexIsDisplayName(std::istringstream& iss, std::string& strField)
{
	return lexIsPhrase(iss, strField);
}

/**
 * mailbox-list
 * (mailbox *("," mailbox)) / obs-mbox-list
 */
bool MailHeader::lexIsMailboxList(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsObsMboxList(iss, strField)) {
		return true;
	} else {
		if (lexIsMailbox(iss, strField)) {
			while (true) {
				if (iss.peek() != ',') {
					break;
				}
				if (! lexIsMailbox(iss, strField)) {
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
 * address-list
 * address *("," address) / obs-addr-list
 */
bool MailHeader::lexIsAddressList(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsObsAddrList(iss, strField)) {
		return true;
	} else {
		if (lexIsAddress(iss, strField)) {
			while (true) {
				if (iss.peek() != ',') {
					break;
				}
				if (! lexIsAddress(iss, strField)) {
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
 * addr-spec
 * local-part "@" domain
 */
bool MailHeader::lexIsAddrSpec(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsLocalPart(iss, strField) && (iss.peek() == '@' && iss.seekg(1, std::ios::cur))
	   && lexIsDomain(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * local-part
 * dot-atom / quoted-string / obs-local-part
 */
bool MailHeader::lexIsLocalPart(std::istringstream& iss, std::string& strField)
{
	if (lexIsDotAtom(iss, strField) || lexIsQuotedString(iss, strField)
	   || lexIsObsLocalPart(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * domain
 * dot-atom / domain-literal / obs-domain
 */
bool MailHeader::lexIsDomain(std::istringstream& iss, std::string& strField)
{
	if (lexIsDotAtom(iss, strField) || lexIsDomainLiteral(iss, strField)
	   || lexIsObsDomain(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * domain-literal
 * [CFWS] "[" *([FWS] dcontent) [FWS] "]" [CFWS]
 */
bool MailHeader::lexIsDomainLiteral(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	if (iss.peek() == '[' && iss.seekg(1, std::ios::cur)) {
		while (true) {
			if (! lexIsFWS(iss, strField)) {
				break;
			}
			if (! lexIsDcontent(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
		lexIsFWS(iss, strField);
		if (iss.peek() == ']') {
			lexIsFWS(iss, strField);
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * dcontent
 * dtext / quoted-pair
 */
bool MailHeader::lexIsDcontent(std::istringstream& iss, std::string& strField)
{
	if (lexIsDtext(iss, strField) || lexIsQuotedPair(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * dtext
 * NO-WS-CTL / %d33-90 / %d94-126
 */
bool MailHeader::lexIsDtext(std::istringstream& iss, std::string& strField)
{
	const char *szNonDtext = "[]\\";
	int n = iss.peek();
	if (lexIsNOWSCTL(iss, strField)) {
		return true;
	} else if (33 < n && n < 126 && strchr(szNonDtext, n) == NULL) {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * fields
 * *(trace
 *   *(resent-date /
 *    resent-from /
 *    resent-sender /
 *    resent-to /
 *    resent-cc /
 *    resent-bcc /
 *    resent-msg-id))
 * *(orig-date /
 * from /
 * sender /
 * reply-to /
 * to /
 * cc /
 * bcc /
 * message-id /
 * in-reply-to /
 * references /
 * subject /
 * comments /
 * keywords /
 * optional-field)
 */
bool MailHeader::lexIsField(std::istringstream& iss, std::string& strField)
{
	while (true) {
		if (! lexIsTrace(iss, strField)) {
			break;
		}
		while (lexIsResentDate(iss, strField)
		      || lexIsResentFrom(iss, strField)
		      || lexIsResentSender(iss, strField)
		      || lexIsResentTo(iss, strField)
		      || lexIsResentCc(iss, strField)
		      || lexIsResentBcc(iss, strField)
		      || lexIsResentMsgID(iss, strField));
	}

	while (lexIsOrigDate(iss, strField)
		|| lexIsFrom(iss, strField)
		|| lexIsSender(iss, strField)
		|| lexIsReplyTo(iss, strField)
		|| lexIsTo(iss, strField)
		|| lexIsCc(iss, strField)
		|| lexIsBcc(iss, strField)
		|| lexIsMessageID(iss, strField)
		|| lexIsInReplyTo(iss, strField)
		|| lexIsReferences(iss, strField)
		|| lexIsSubject(iss, strField)
		|| lexIsComments(iss, strField)
		|| lexIsKeywords(iss, strField)
		|| lexIsOptionalField(iss, strField));
	return true;
}

/**
 * orig-date
 * "Date:" date-time CRLF
 */
bool MailHeader::lexIsOrigDate(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[6];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Date:") == 0) {
		if (lexIsDateTime(iss, strField) && lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * from
 * "From:" mailbox-list CRLF
 */
bool MailHeader::lexIsFrom(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[6];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "From:") == 0) {
		if (lexIsMailboxList(iss, strField) && lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * sender
 * "Sender:" mailbox CRLF
 */
bool MailHeader::lexIsSender(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[8];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Sender:") == 0) {
		if (lexIsMailbox(iss, strField) && lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * reply-to
 * "Reply-To:" address-list CRLF
 */
bool MailHeader::lexIsReplyTo(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[10];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Reply-To:") == 0) {
		if (lexIsMailboxList(iss, strField) && lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * to
 * "To:" address-list CRLF
 */
bool MailHeader::lexIsTo(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[4];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "To:") == 0) {
		if (lexIsMailboxList(iss, strField) && lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * cc
 * "Cc:" address-list CRLF
 */
bool MailHeader::lexIsCc(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[4];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Cc:") == 0) {
		if (lexIsMailboxList(iss, strField) && lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * bcc
 * "Bcc:" (address-list / [CFWS]) CRLF
 */
bool MailHeader::lexIsBcc(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[5];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Bcc:") == 0) {
		if ((lexIsAddressList(iss, strField) || lexIsCFWS(iss, strField)) && lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * message-id
 * "Message-ID:" msg-id CRLF
 */
bool MailHeader::lexIsMessageID(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[12];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Message-ID:") == 0) {
		if (lexIsMsgID(iss, strField) && lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * in-reply-to
 * "In-Reply-To:" 1*msg-id CRLF
 */
bool MailHeader::lexIsInReplyTo(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[13];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "In-Reply-To:") == 0) {
		if (lexIsMsgID(iss, strField)) {
			while (lexIsMsgID(iss, strField));
			if (! lexIsCRLF(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * references
 * "References:" 1*msg-id CRLF
 */
bool MailHeader::lexIsReferences(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[12];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "References:") == 0) {
		if (lexIsMsgID(iss, strField)) {
			while (lexIsMsgID(iss, strField));
			if (! lexIsCRLF(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * msg-id
 * [CFWS] "<" id-left "@" id-right ">" [CFWS]
 */
bool MailHeader::lexIsMsgID(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsCFWS(iss, strField) && iss.peek() == '<') {
		iss.seekg(1, std::ios::cur);
		if (lexIsIDLeft(iss, strField) && iss.peek() == '@') {
			iss.seekg(1, std::ios::cur);
			if (lexIsIDRight(iss, strField) && iss.peek() == '>') {
				iss.seekg(1, std::ios::cur);
				if (lexIsCFWS(iss, strField)) {
					return true;
				}
			}
		}
		iss.seekg(pos, std::ios::beg);
	}
	return false;
}

/**
 * id-left
 * dot-atom-text / no-fold-quote / obs-id-left
 */
bool MailHeader::lexIsIDLeft(std::istringstream& iss, std::string& strField)
{
	if (lexIsDotAtomText(iss, strField) || lexIsNoFoldQuote(iss, strField)
	   || lexIsObsIDLeft(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * id-right
 * dot-atom-text / no-fold-literal / obs-id-right
 */
bool MailHeader::lexIsIDRight(std::istringstream& iss, std::string& strField)
{
	if (lexIsDotAtomText(iss, strField) || lexIsNoFoldLiteral(iss, strField)
	   || lexIsObsIDRight(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * no-fold-quote
 * DQUOTE *(qtext / quoted-pair) DQUOTE
 */
bool MailHeader::lexIsNoFoldQuote(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsDQUOTE(iss, strField)) {
		while (lexIsQtext(iss, strField) || lexIsQuotedPair(iss, strField));
		if (lexIsDQUOTE(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * no-fold-literal
 * "[" *(dtext / quoted-pair) "]"
 */
bool MailHeader::lexIsNoFoldLiteral(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (iss.peek() == '[') {
		iss.seekg(1, std::ios::cur);
		while (lexIsDtext(iss, strField) || lexIsQuotedPair(iss, strField));
		if (iss.peek() == ']') {
			iss.seekg(1, std::ios::cur);
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * subject
 * "Subject:" unstructured CRLF
 */
bool MailHeader::lexIsSubject(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[9];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Subject:") == 0 && lexIsUnstructured(iss, strField)
	   && lexIsCRLF(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * comments
 * "Comments:" unstructured CRLF
 */
bool MailHeader::lexIsComments(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[10];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Comments:") == 0 && lexIsUnstructured(iss, strField)
	   && lexIsCRLF(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * keywords
 * "Keywords:" phrase *("," phrase) CRLF
 */
bool MailHeader::lexIsKeywords(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[10];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Keywords:") == 0 && lexIsPhrase(iss, strField)) {
		while (iss.peek() == ',') {
			iss.seekg(1, std::ios::cur);
			if (! lexIsPhrase(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return true;
			}
		}
		if (lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * resent-date
 * "Resent-Date:" date-time CRLF
 */
bool MailHeader::lexIsResentDate(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[13];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Resent-Date:") == 0 && lexIsDateTime(iss, strField)
	   && lexIsCRLF(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * resent-from
 * "Resent-From:" mailbox-list CRLF
 */
bool MailHeader::lexIsResentFrom(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[13];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Resent-From:") == 0 && lexIsMailboxList(iss, strField)
	   && lexIsCRLF(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * resent-sender
 * "Resent-Sender:" mailbox CRLF
 */
bool MailHeader::lexIsResentSender(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[15];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Resent-Sender:") == 0 && lexIsMailbox(iss, strField)
	   && lexIsCRLF(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * resent-to
 * "Resent-To:" address-list CRLF
 */
bool MailHeader::lexIsResentTo(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[11];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Resent-To:") == 0 && lexIsAddressList(iss, strField)
	   && lexIsCRLF(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * resent-cc
 * "Resent-Cc:" address-list CRLF
 */
bool MailHeader::lexIsResentCc(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[11];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Resent-Cc:") == 0 && lexIsAddressList(iss, strField)
	   && lexIsCRLF(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * resent-bcc
 * "Resent-Bcc:" (address-list / [CFWS]) CRLF
 */
bool MailHeader::lexIsResentBcc(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[12];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Resent-Bcc:") == 0 
	   && (((lexIsAddressList(iss, strField) || lexIsCFWS(iss, strField))
	        && lexIsCRLF(iss, strField))
	      || lexIsCRLF(iss, strField))) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * resent-msg-id
 * "Resent-Message-ID:" msg-id CRLF
 */
bool MailHeader::lexIsResentMsgID(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[19];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Resent-Message-ID:") == 0 && lexIsMsgID(iss, strField)
	   && lexIsCRLF(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * trace
 * [return] 1*received
 */
bool MailHeader::lexIsTrace(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsReturn(iss, strField);
	if (! lexIsReceived(iss, strField)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	while (lexIsReceived(iss, strField));
	return true;
}

/**
 * return
 * "Return-Path:" path CRLF
 */
bool MailHeader::lexIsReturn(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[13];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Return-Path:") == 0 && lexIsPath(iss, strField)
	   && lexIsCRLF(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * path
 * ([CFWS] "<" ([CFWS] / addr-spec) ">" [CFWS]) / obs-path
 */
bool MailHeader::lexIsPath(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if ((lexIsCFWS(iss, strField) && iss.peek() == '<') || iss.peek() == '<') {
		iss.seekg(1, std::ios::cur);
		lexIsCFWS(iss, strField);
		if (lexIsAddrSpec(iss, strField) && iss.peek() == '>') {
			iss.seekg(1, std::ios::cur);
			lexIsCFWS(iss, strField);
			return true;
		}
	} else if (lexIsObsPath(iss, strField)) {
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * received
 * "Received:" name-val-list ";" date-time CRLF
 */
bool MailHeader::lexIsReceived(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[10];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Received:") == 0 && lexIsNameValList(iss, strField) && iss.peek() == ';') {
		iss.seekg(1, std::ios::cur);
		if (lexIsDateTime(iss, strField) && lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * name-val-list
 * [CFWS] [name-val-pair *(CFWS name-val-pair)]
 */
bool MailHeader::lexIsNameValList(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	if (lexIsNameValPair(iss, strField)) {
		while (true) {
			if (! lexIsCFWS(iss, strField)) {
				break;
			}
			if (! lexIsNameValPair(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
	}
	return false;
}

/**
 * name-val-pair
 * item-name CFWS item-value
 */
bool MailHeader::lexIsNameValPair(std::istringstream& iss, std::string& strField)
{
	if (lexIsItemName(iss, strField) && lexIsCFWS(iss, strField)
	   && lexIsItemValue(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * item-name
 * ALPHA *(["-"] (ALPHA / DIGIT))
 */
bool MailHeader::lexIsItemName(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsALPHA(iss, strField)) {
		while (true) {
			if (iss.peek() == '-') {
				iss.seekg(1, std::ios::cur);
			}
			if (! (lexIsALPHA(iss, strField) || lexIsDIGIT(iss, strField))) {
				iss.seekg(pos, std::ios::beg);
				break;
			}
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * item-value
 * 1*angle-addr / addr-spec / atom / domain / msg-id
 */
bool MailHeader::lexIsItemValue(std::istringstream& iss, std::string& strField)
{
	if (lexIsAngleAddr(iss, strField)) {
		while (lexIsAngleAddr(iss, strField));
		return true;
	} else {
		if (lexIsAddrSpec(iss, strField) || lexIsAtom(iss, strField)
		   || lexIsDomain(iss, strField) || lexIsMsgID(iss, strField)) {
			return true;
		}
	}
	return false;
}

/**
 * optional-field
 * field-name ":" unstructured CRLF
 */
bool MailHeader::lexIsOptionalField(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsFieldName(iss, strField) && iss.peek() == ':') {
		iss.seekg(1, std::ios::cur);
		if (lexIsUnstructured(iss, strField) && lexIsCRLF(iss, strField)) {
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * field-name
 * 1*ftext
 */
bool MailHeader::lexIsFieldName(std::istringstream& iss, std::string& strField)
{
	if (lexIsFtext(iss, strField)) {
		while (lexIsFtext(iss, strField));
		return true;
	}
	return false;
}

/**
 * ftext
 * %d33-57 / %d59-126
 */
bool MailHeader::lexIsFtext(std::istringstream& iss, std::string& strField)
{
	int n = iss.peek();
	if (33 <= n && n <= 126 && n != 58) {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * obs-qp
 * "\" (%d0-127)
 */
bool MailHeader::lexIsObsQp(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (iss.peek() == '\\') {
		iss.seekg(1, std::ios::cur);
		if (0 < (int)iss.peek() && (int)iss.peek() < 127) {
			iss.seekg(1, std::ios::cur);
			return true;
		}
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-text
 * *LF *CR *(obs-char *LF *CR)
 */
bool MailHeader::lexIsObsText(std::istringstream& iss, std::string& strField)
{
	while (iss.peek() == '\n' && iss.seekg(1, std::ios::cur));
	while (iss.peek() == '\r' && iss.seekg(1, std::ios::cur));
	while (lexIsObsChar(iss, strField)) {
		while (iss.peek() == '\n' && iss.seekg(1, std::ios::cur));
		while (iss.peek() == '\r' && iss.seekg(1, std::ios::cur));
	}
	return true;
}

/**
 * obs-char
 * %d0-9 / %d11 / %d12 / %d14-127  ; %d0-127 except CR and LF
 */
bool MailHeader::lexIsObsChar(std::istringstream& iss, std::string& strField)
{
	int n = iss.peek();
	if ((0 <= n && n <= 127) && n != '\r' && n != '\n') {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * obs-utext
 * obs-text
 */
bool MailHeader::lexIsObsUtext(std::istringstream& iss, std::string& strField)
{
	return lexIsObsText(iss, strField);
}

/**
 * obs-phrase
 * word *(word / "." / CFWS)
 */
bool MailHeader::lexIsObsPhrase(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsWord(iss, strField)) {
		while (true) {
			if (iss.peek() == '.') {
				iss.seekg(1, std::ios::cur);
				continue;
			} else if (! (lexIsWord(iss, strField) || lexIsCFWS(iss, strField))) {
				break;
			}
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-FWS
 * 1*WSP *(CRLF 1*WSP)
 */
bool MailHeader::lexIsObsFWS(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsWSP(iss, strField)) {
		while (lexIsWSP(iss, strField));
		while (true) {
			if (! lexIsCRLF(iss, strField)) {
				break;
			}
			if (! lexIsWSP(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
			while (lexIsWSP(iss, strField));
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
bool MailHeader::lexIsObsDayOfWeek(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	if (! lexIsDayName(iss, strField)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	lexIsCFWS(iss, strField);
	return true;
}

/**
 * obs-year
 * [CFWS] 2*DIGIT [CFWS]
 */
bool MailHeader::lexIsObsYear(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	char buf[3];
	iss.read(buf, sizeof(buf) - 1);
	int year = 0;
	int matched = sscanf_s(buf, "%02d", &year);
	if (matched != 1) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	lexIsCFWS(iss, strField);
	return true;
}

/**
 * obs-month
 * CFWS month-name CFWS
 */
bool MailHeader::lexIsObsMonth(std::istringstream& iss, std::string& strField)
{
	if (lexIsCFWS(iss, strField) && lexIsMonthName(iss, strField)
	   && lexIsCFWS(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * obs-day
 * [CFWS] 1*2DIGIT [CFWS]
 */
bool MailHeader::lexIsObsDay(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	if (isdigit(iss.peek())) {
		iss.seekg(1, std::ios::cur);
		if (isdigit(iss.peek())) {
			iss.seekg(1, std::ios::cur);
		}
		lexIsCFWS(iss, strField);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-hour
 * [CFWS] 2DIGIT [CFWS]
 */
bool MailHeader::lexIsObsHour(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	char buf[3];
	iss.read(buf, sizeof(buf) - 1);
	int hour = 0;
	int matched = sscanf_s(buf, "%02d", &hour);
	if (matched != 1) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	lexIsCFWS(iss, strField);
	return true;
}

/**
 * obs-minute
 * [CFWS] 2DIGIT [CFWS]
 */
bool MailHeader::lexIsObsMinute(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	char buf[3];
	iss.read(buf, sizeof(buf) - 1);
	int minute = 0;
	int matched = sscanf_s(buf, "%02d", &minute);
	if (matched != 1) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	lexIsCFWS(iss, strField);
	return true;
}

/**
 * obs-second
 * [CFWS] 2DIGIT [CFWS]
 */
bool MailHeader::lexIsObsSecond(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	char buf[3];
	iss.read(buf, sizeof(buf) - 1);
	int second = 0;
	int matched = sscanf_s(buf, "%02d", &second);
	if (matched != 1) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	lexIsCFWS(iss, strField);
	return true;
}

/**
 * obs-zone
 * "UT" / "GMT" / "EST" / "EDT" / "CST" /
 * "CDT" / "MST" / "MDT" / "PST" / "PDT" /
 * %d65-73 / %d75-90 / %d97-105 / %d107-122
 */
bool MailHeader::lexIsObsZone(std::istringstream& iss, std::string& strField)
{
	const char *arrZone[] = { "UT", "GMT", "EST", "EDT", "CST", 
	                          "CDT", "MST", "MDT", "PST", "PDT" };
	std::ios::pos_type pos = iss.tellg();
	char buf[4];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	iss.seekg(pos, std::ios::beg);
	for (int i = 0; i < sizeof(arrZone) / sizeof(arrZone[0]); i++) {
		if (strncmp(buf, arrZone[i], strlen(arrZone[i])) == 0) {
			iss.seekg(strlen(arrZone[i]), std::ios::cur);
			return true;
		}
	}
	if (isalpha(iss.peek()) && iss.peek() != 'j' && iss.peek() != 'J') {
		iss.seekg(1, std::ios::cur);
		return true;
	}
	return false;
}

/**
 * obs-angle-addr
 * [CFWS] "<" [obs-route] addr-spec ">" [CFWS]
 */
bool MailHeader::lexIsObsAngleAddr(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	if (iss.peek() == '<') {
		iss.seekg(1, std::ios::cur);
		lexIsObsRoute(iss, strField);
		if (! (lexIsAddrSpec(iss, strField) 
			  && iss.peek() == '>' && iss.seekg(1, std::ios::cur))) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		lexIsCFWS(iss, strField);
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-route
 * [CFWS] obs-domain-list ":" [CFWS]
 */
bool MailHeader::lexIsObsRoute(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	lexIsCFWS(iss, strField);
	if (! (lexIsObsDomainList(iss, strField)
	      && iss.peek() == ':' && iss.seekg(1, std::ios::cur))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	lexIsCFWS(iss, strField);
	return true;
}

/**
 * obs-domain-list
 * "@" domain *(*(CFWS / "," ) [CFWS] "@" domain)
 */
bool MailHeader::lexIsObsDomainList(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (iss.peek() == '@' && iss.seekg(1, std::ios::beg)) {
		if (lexIsDomain(iss, strField)) {
			while (true) {
				if (! (lexIsCFWS(iss, strField)
				      || (iss.peek() == ',' && iss.seekg(1, std::ios::cur)))) {
					break;
				}
				lexIsCFWS(iss, strField);
				if (! (iss.peek() == '@' && lexIsDomain(iss, strField))) {
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
 * obs-local-part
 * word *("." word)
 */
bool MailHeader::lexIsObsLocalPart(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsWord(iss, strField)) {
		while (true) {
			if (iss.peek() != '.') {
				break;
			}
			if (! lexIsWord(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
		return true;
	}
	return false;
}

/**
 * obs-domain
 * atom *("." atom)
 */
bool MailHeader::lexIsObsDomain(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (lexIsAtom(iss, strField)) {
		while (true) {
			if (iss.peek() != '.') {
				break;
			}
			if (! lexIsDomain(iss, strField)) {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-mbox-list
 * *([mailbox] [CFWS] "," [CFWS])
 */
bool MailHeader::lexIsObsMboxList(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	while (true) {
		lexIsMailbox(iss, strField);
		lexIsCFWS(iss, strField);
		if (iss.peek() != ',') {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		lexIsCFWS(iss, strField);
	}
	return true;
}

/**
 * obs-addr-list
 * *([address] [CFWS] "," [CFWS])
 */
bool MailHeader::lexIsObsAddrList(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	while (true) {
		lexIsAddress(iss, strField);
		lexIsCFWS(iss, strField);
		if (iss.peek() != ',') {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		lexIsCFWS(iss, strField);
	}
	return true;
}

/**
 * obs-fields
 * *(obs-return, obs-received, obs-orig-date, obs-from, obs-sender,
 *   obs-reply-to, obs-to, obs-cc, obs-bcc, obs-message-id, 
 *   obs-in-reply-to, obs-references, obs-subject, obs-comments, 
 *   obs-keywords, obs-resent-date, obs-resent-from, obs-resent-send, 
 *   obs-resent-rply, obs-resent-to, obs-resent-cc, obs-resent-bcc,
 *   obs-resent-mid, obs-optional)
 */
bool MailHeader::lexIsObsFields(std::istringstream& iss, std::string& strField)
{
	while (lexIsObsReturn(iss, strField)
		   || lexIsObsReceived(iss, strField)
		   || lexIsObsOrigDate(iss, strField)
		   || lexIsObsFrom(iss, strField)
		   || lexIsObsSender(iss, strField)
		   || lexIsObsInReplyTo(iss, strField)
		   || lexIsObsTo(iss, strField)
		   || lexIsObsCc(iss, strField)
		   || lexIsObsBcc(iss, strField)
		   || lexIsObsMessageID(iss, strField)
		   || lexIsObsInReplyTo(iss, strField)
		   || lexIsObsReferences(iss, strField)
		   || lexIsObsSubject(iss, strField)
		   || lexIsObsComments(iss, strField)
		   || lexIsObsKeywords(iss, strField)
		   || lexIsObsResentDate(iss, strField)
		   || lexIsObsResentFrom(iss, strField)
		   || lexIsObsResentSend(iss, strField)
		   || lexIsObsResentRply(iss, strField)
		   || lexIsObsResentTo(iss, strField)
		   || lexIsObsResentCc(iss, strField)
		   || lexIsObsResentBcc(iss, strField)
		   || lexIsObsResentMid(iss, strField)
		   || lexIsObsOptional(iss, strField));
	return true;
}

/**
 * obs**
 * "XXXX" *WSP ":" YYYY CRLF
 */
bool MailHeader::lexIsObsHeader(std::istringstream& iss, std::string& strField,
								const char *szHeader, 
								bool (MailHeader::*lexer)(std::istringstream&, std::string&))
{
	MailHeader mh;
	std::ios::pos_type pos = iss.tellg();
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, strlen(szHeader));
	if (strcmp(buf, szHeader) == 0) {
		while (lexIsWSP(iss, strField));
		if (! (iss.peek() == ':' && iss.seekg(1, std::ios::cur))) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		if (! ((mh.*lexer)(iss, strField) && lexIsCRLF(iss, strField))) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-orig-date
 * "Date" *WSP ":" date-time CRLF
 */
bool MailHeader::lexIsObsOrigDate(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Date", &MailHeader::lexIsDateTime);
}

/**
 * obs-from
 * "From" *WSP ":" mailbox-list CRLF
 */
bool MailHeader::lexIsObsFrom(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "From", &MailHeader::lexIsMailboxList);
}

/**
 * obs-sender
 * "Sender" *WSP ":" mailbox CRLF
 */
bool MailHeader::lexIsObsSender(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Sender", &MailHeader::lexIsMailbox);
}

/**
 * obs-reply-to
 * "Reply-To" *WSP ":" mailbox-list CRLF
 */
bool MailHeader::lexIsObsReplyTo(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Reply-To", &MailHeader::lexIsMailboxList);
}

/**
 * obs-to
 * "To" *WSP ":" address-list CRLF
 */
bool MailHeader::lexIsObsTo(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "To", &MailHeader::lexIsAddressList);
}

/**
 * obs-cc
 * "Cc" *WSP ":" address-list CRLF
 */
bool MailHeader::lexIsObsCc(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Cc", &MailHeader::lexIsAddressList);
}

/**
 * address-list / [CFWS]
 */
bool MailHeader::lexIsAddreeListOrCFWS(std::istringstream& iss, std::string& strField)
{
	if (lexIsAddressList(iss, strField)) {
		return true;
	} else {
		lexIsCFWS(iss, strField);
		return true;
	}
	return false;
}

/**
 * obs-bcc
 * "Bcc" *WSP ":" (address-list / [CFWS]) CRLF
 */
bool MailHeader::lexIsObsBcc(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Bcc", &MailHeader::lexIsAddreeListOrCFWS);
}

/**
 * obs-message-id
 * "Message-ID" *WSP ":" msg-id CRLF
 */
bool MailHeader::lexIsObsMessageID(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Message-ID", &MailHeader::lexIsMsgID);
}

/**
 * (phrase / msg-id)
 */
bool MailHeader::lexIsPhraseOrMsgID(std::istringstream& iss, std::string& strField)
{
	if (lexIsPhrase(iss, strField) || lexIsMsgID(iss, strField)) {
		return true;
	}
	return false;
}

/**
 * *(phrase / msg-id)
 */
bool MailHeader::lexIsNPhraseOrMsgID(std::istringstream& iss, std::string& strField)
{
	while (lexIsPhraseOrMsgID(iss, strField));
	return true;
}

/**
 * obs-in-reply-to
 * "In-Reply-To" *WSP ":" *(phrase / msg-id) CRLF
 */
bool MailHeader::lexIsObsInReplyTo(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "In-Reply-To", &MailHeader::lexIsNPhraseOrMsgID);
}

/**
 * obs-references
 * "References" *WSP ":" *(phrase / msg-id) CRLF
 */
bool MailHeader::lexIsObsReferences(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "References", &MailHeader::lexIsNPhraseOrMsgID);
}

/**
 * obs-id-left
 * local-part
 */
bool MailHeader::lexIsObsIDLeft(std::istringstream& iss, std::string& strField)
{
	return lexIsLocalPart(iss, strField);
}

/**
 * obs-id-right
 * domain
 */
bool MailHeader::lexIsObsIDRight(std::istringstream& iss, std::string& strField)
{
	return lexIsDomain(iss, strField);
}

/**
 * obs-subject
 * "Subject" *WSP ":" unstructured CRLF
 */
bool MailHeader::lexIsObsSubject(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Subject", &MailHeader::lexIsUnstructured);
}

/**
 * obs-comments
 * "Comments" *WSP ":" unstructured CRLF
 */
bool MailHeader::lexIsObsComments(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Comments", &MailHeader::lexIsUnstructured);
}

/**
 * obs-keywords
 * "Keywords" *WSP ":" *([phrase] ",") CRLF
 */
bool MailHeader::lexIsObsKeywords(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	char buf[9];
	memset(buf, 0, sizeof(buf));
	iss.read(buf, sizeof(buf) - 1);
	if (strcmp(buf, "Keywords") == 0) {
		while (lexIsWSP(iss, strField));
		if (! (iss.peek() == ':' && iss.seekg(1, std::ios::cur))) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		while (true) {
			lexIsPhrase(iss, strField);
			if (iss.peek() != ',') {
				iss.seekg(pos, std::ios::beg);
				return false;
			}
		}
		if (! lexIsCRLF(iss, strField)) {
			iss.seekg(pos, std::ios::beg);
			return false;
		}
		return true;
	}
	iss.seekg(pos, std::ios::beg);
	return false;
}

/**
 * obs-resent-from
 * "Resent-From" *WSP ":" mailbox-list CRLF
 */
bool MailHeader::lexIsObsResentFrom(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Resent-From", &MailHeader::lexIsMailboxList);
}

/**
 * obs-resent-send
 * "Resent-Sender" *WSP ":" mailbox CRLF
 */
bool MailHeader::lexIsObsResentSend(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Resent-Sender", &MailHeader::lexIsMailbox);
}

/**
 * obs-resent-date
 * "Resent-Date" *WSP ":" date-time CRLF
 */
bool MailHeader::lexIsObsResentDate(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Resent-Date", &MailHeader::lexIsDateTime);
}

/**
 * obs-resent-to
 * "Resent-To" *WSP ":" address-list CRLF
 */
bool MailHeader::lexIsObsResentTo(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Resent-To", &MailHeader::lexIsAddressList);
}

/**
 * obs-resent-cc
 * "Resent-Cc" *WSP ":" address-list CRLF
 */
bool MailHeader::lexIsObsResentCc(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Resent-Cc", &MailHeader::lexIsAddressList);
}

/**
 * obs-resent-bcc
 * "Resent-Bcc" *WSP ":" (address-list / [CFWS]) CRLF
 */
bool MailHeader::lexIsObsResentBcc(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Resent-Bcc", &MailHeader::lexIsAddreeListOrCFWS);
}

/**
 * obs-resent-mid
 * "Resent-Message-ID" *WSP ":" msg-id CRLF
 */
bool MailHeader::lexIsObsResentMid(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Resent-Message-ID", &MailHeader::lexIsMsgID);
}

/**
 * obs-resent-rply
 * "Resent-Reply-To" *WSP ":" address-list CRLF
 */
bool MailHeader::lexIsObsResentRply(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Resent-Reply-To", &MailHeader::lexIsAddressList);
}

/**
 * obs-return
 * "Return-Path" *WSP ":" path CRLF
 */
bool MailHeader::lexIsObsReturn(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Return-Path", &MailHeader::lexIsPath);
}

/**
 * obs-received
 * "Received" *WSP ":" name-val-list CRLF
 */
bool MailHeader::lexIsObsReceived(std::istringstream& iss, std::string& strField)
{
	return lexIsObsHeader(iss, strField, "Received", &MailHeader::lexIsNameValList);
}

/**
 * obs-path
 * obs-angle-addr
 */
bool MailHeader::lexIsObsPath(std::istringstream& iss, std::string& strField)
{
	return lexIsObsAngleAddr(iss, strField);
}

/**
 * obs-optional
 * field-name *WSP ":" unstructured CRLF
 */
bool MailHeader::lexIsObsOptional(std::istringstream& iss, std::string& strField)
{
	std::ios::pos_type pos = iss.tellg();
	if (! lexIsFieldName(iss, strField)) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	while (lexIsWSP(iss, strField));
	if (! (iss.peek() == ':' && iss.seekg(1, std::ios::cur))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	if (! (lexIsUnstructured(iss, strField) && lexIsCRLF(iss, strField))) {
		iss.seekg(pos, std::ios::beg);
		return false;
	}
	return true;
}

/**
 * ヘッダを設定
 */
void MailHeader::setHeader(std::string strField)
{
	std::istringstream iss(strField);
	if (lexIsField(iss, strField) && lexIsObsFields(iss, strField)) {
		return;
	}
	return;
}

/**
 * 任意のフィールドを取得
 */
const std::list<std::pair<std::string, std::string>> MailHeader::getField(const TCHAR* szName) const
{
	std::list<std::pair<std::string, std::string>> listField;
	for (std::list<std::pair<std::string, std::string>>::const_iterator i = this->listField.begin(); i != this->listField.end(); i++) {
		listField.push_back(*i);
	}
	return listField;
}

/**
 * 任意のフィールドを削除
 */
void MailHeader::removeField(const TCHAR* szName)
{
	std::string strHeader;
	for (std::list<std::pair<std::string, std::string>>::const_iterator i = this->listField.begin(); i != this->listField.end(); ) {
		if (_tcscmp(szName, i->first.c_str()) == 0) {
			i = this->listField.erase(i);
			continue;
		}
		i++;
	}
}

/**
 * 任意のフィールドを追加
 */
void MailHeader::appendField(const TCHAR* szName, const TCHAR* szContent)
{
	this->listField.push_back(std::pair<std::string, std::string>(szName, szContent));
}

/* private 関数 */
/**
 * フィールドを解析
 */
std::pair<std::string, std::string> MailHeader::parseField(std::stringstream ss)
{
	while (! ss.eof()) {

	}
	return std::pair<std::string, std::string>("", "");
}


/**
 * メールボディクラス
 */
/**
 * コンストラクタ
 */
MailBody::MailBody(void)
{
}

/**
 * コンストラクタ
 */
MailBody::MailBody(std::string strBody)
{
	this->setBody(strBody);
}

/**
 * デストラクタ
 */
MailBody::~MailBody(void)
{
}

/**
 * []演算子のオーバーロード
 */
std::string MailBody::operator[](int nPage)
{
	// [TODO]
	return "";
}

/**
 * ボディを取得
 */
const std::string MailBody::getBody() const
{
	std::string strBody;
	if (this->listPage.size() > 1) {
		for (std::list<std::string>::const_iterator i = this->listPage.begin(); i != this->listPage.end(); ) {
			strBody.append(*i);
			i++;
			if (i != this->listPage.end()) {
				strBody.append("--");
				strBody.append(this->strBoundary);
			}
		}
	}
	return strBody;
}

/**
 * ボディを設定
 */
void MailBody::setBody(std::string strBody)
{
	if (this->strBoundary.compare("") == 0)
		return;
	this->listPage.clear();
	std::istringstream ss(strBody);
	std::string strPage, strLine;
	int nLenBoundary = strlen(this->strBoundary.c_str());
	while (std::getline(ss, strLine)) {
		if (strLine.compare(0, 2, "--", 0, 0) == 0
		   && strLine.compare(2, nLenBoundary, this->strBoundary.c_str(), 0, nLenBoundary) == 0) {
			   this->listPage.push_back(strPage);
			   strPage.clear();
		} else {
			strPage.append(strLine);
		}
	}
}

/**
 * ページ境界文字列を取得
 */
const std::string MailBody::getBoundary() const
{
	return this->strBoundary;
}

/**
 * ページ境界文字列を設定
 */
void MailBody::setBoundary(std::string strBoundry)
{
	this->strBoundary = strBoundary;
}


/**
 * メールクラス
 */
/**
 * コンストラクタ
 */
Mail::Mail(void)
{
}

/**
 * コンストラクタ
 */
Mail::Mail(std::string strMessage)
{
	this->setMessage(strMessage);
}

/**
 * デストラクタ
 */
Mail::~Mail(void)
{
}

/**
 * メッセージ（ヘッダとボディ）を取得
 */
const std::string Mail::getMessage() const
{
	return this->header.getHeader() + "\r\n" + this->body.getBody();
}

/**
 * メッセージ（ヘッダとボディ）を設定
 */
void Mail::setMessage(std::string strMessage)
{
	/* ヘッダとボディの境界までシーカを進める */
	std::istringstream iss(strMessage);
	char buf[4 + 1];
	memset(buf, 0, sizeof(buf));
	while (! iss.eof()) {
		iss.read(buf, 4);
		char *p = strchr(buf, '\r');
		if (p != NULL) {
			if (strcmp(buf, "\r\n\r\n") == 0)
				break;
			if (p != buf)
				iss.seekg(-4 + (p - buf), std::ios::cur);
		}
	}
	if (! iss || iss.eof())
		return;

	/* ヘッダを設定 */
	size_t nSizeHeader = iss.tellg();
	std::string strHeader(strMessage, 0, nSizeHeader);
	MailHeader header(strHeader);
	this->setHeader(header);

	/* ボディを設定 */
	iss.seekg(0, std::ios::end);
	size_t nSizeBody = (size_t) iss.tellg() - nSizeHeader;
	std::string strBody(strMessage, nSizeHeader, nSizeBody);
	MailBody body(strBody);
	std::list<MHFTYPE> listContentType = header.getField("Content-Type");
	if (listContentType.size() > 0) {
		std::string strContentType = listContentType.back().second;
		std::string strBoundary;
		std::istringstream iss2(strContentType);
		std::string strItem;
		while (std::getline(iss2, strItem, ';')) {
			if (strItem.compare(0, 9, "boundary=", 0, 9) == 0) {
				strBoundary = strItem.substr(9);
				break;
			}
		}
		if (strBoundary.compare("") != 0)
			body.setBoundary(strBoundary);
	}
	this->setBody(body);
}

/**
 * メールヘッダを取得
 */
const MailHeader Mail::getHeader() const
{
	return this->header;
}

/**
 * メールヘッダを設定
 */
void Mail::setHeader(MailHeader header)
{
	this->header = header;
}

/**
 * メールボディを取得
 */
const MailBody Mail::getBody() const
{
	return this->body;
}

/**
 * メールボディを設定
 */
void Mail::setBody(MailBody body)
{
	this->body = body;
}

/**
 * 文字列で表現された日付を解析する
 */
void Mail::parseDateStr(LPCTSTR szField, LPCTSTR szDateFormat, struct tm *tmDate, long int& tm_gmtoff) const
{
	DateTime::DTFORMAT format = DateTime::DTF_RFC2822;

	/* 日付フォーマットを調べる */
	if (_strnicmp(szDateFormat, "rfc2822", 7) == 0) {
		format = DateTime::DTF_RFC2822;
	}

	/* 日付フォーマットに従って日付文字列を解析する */
	DateTime dt;
	memset(tmDate, 0, sizeof(struct tm));
	tm_gmtoff = 0;
	switch (format) {
	case DateTime::DTF_RFC2822:
		dt.Parse(szField, DateTime::DTF_RFC2822, tmDate, tm_gmtoff);
		break;
	}
}

/**
 * メールインデックスを取得
 */
MailIndex Mail::getIndex() const
{
	MailIndex midx;
	this->setIndex(&midx);
	return midx;
}

/**
 * メールインデックスを取得
 */
MailIndex* Mail::createIndex() const
{
	MailIndex *midx = new MailIndex();
	this->setIndex(midx);
	return midx;
}

/**
 * メールインデックスデータを設定
 */
void Mail::setIndex(MailIndex* midx) const
{
	midx->strSubject = this->header.getField("Subject").back().second;
	midx->strTo = this->header.getField("To").back().second;
	midx->strFrom = this->header.getField("From").back().second;
	midx->strMsgId = this->header.getField("Message-Id").back().second;
	midx->strReferences = this->header.getField("References").back().second;

	const char* tmFieldName[] = {"Date", "Received"};
	time_t* tmTarget[] = {&(midx->tSend), &(midx->tRecv)};
	for (int i = 0; i < sizeof(tmFieldName) / sizeof(tmFieldName[0]); i++) {
		time_t buftime;
		long curTimezone;
		struct tm tmDate = {{0}};
		long int tm_gmtoff = 0;
		parseDateStr(this->header.getField("Date").back().second.c_str(), "rfc2822", &tmDate, tm_gmtoff);
		_tzset();
		_get_timezone(&curTimezone);
		tmDate.tm_hour += -(int)(tm_gmtoff / 60) -(int)(curTimezone / 3600);
		tmDate.tm_min += -(tm_gmtoff % 60) -((int)(curTimezone / 60) % 60);
		buftime = mktime(&tmDate);
		localtime_s(&tmDate, &buftime);
		*tmTarget[i] = buftime;
	}
}
