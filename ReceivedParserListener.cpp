#include "ReceivedParserListener.h"

/**
 * �R���X�g���N�^
 */
ReceivedParserListener::ReceivedParserListener(void)
{
	memset(&(this->tmDate), 0, sizeof(struct tm));
	this->tm_gmtoffset = 0;
}

/**
 * �f�X�g���N�^
 */
ReceivedParserListener::~ReceivedParserListener(void)
{
}

/**
 * Received �t�B�[���h�̉�͌��ʂ��t�b�N����
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