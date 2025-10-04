#include "ReceivedParserListener.h"

/**
 * コンストラクタ
 */
ReceivedParserListener::ReceivedParserListener(void)
{
	memset(&(this->tmDate), 0, sizeof(struct tm));
	this->tm_gmtoffset = 0;
}

/**
 * デストラクタ
 */
ReceivedParserListener::~ReceivedParserListener(void)
{
}

/**
 * Received フィールドの解析結果をフックする
 */
bool ReceivedParserListener::hookParse(std::string strHeader, MailHeaderField::MHFTYPE type,
									   std::ios::pos_type posBeg, std::ios::pos_type posEnd)
{
	if (isBegin) {
		switch (type)
		{
		case MailHeaderField::T_DAY:
		case MailHeaderField::T_MONTH:
		case MailHeaderField::T_YEAR:
		case MailHeaderField::T_HOUR:
		case MailHeaderField::T_MINUTE:
		case MailHeaderField::T_SECOND:
		case MailHeaderField::T_ZONE:
			break;
		}
	} else {
	}
}