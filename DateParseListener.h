#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <time.h>
#include <locale>
#include "MailHeaderParser.h"

/**
 * ----------------------------------------------------------------------
 * DateParserListener
 * Date �t�B�[���h�̉�̓N���X
 * ----------------------------------------------------------------------
 */class DateParseListener :
	public MailHeaderParseListener
{
public:
	/* public �v���p�e�B */
	struct tm tmDate;
	int tm_gmtoffset;

	/* public ���\�b�h */
	DateParseListener(void);   // �R���X�g���N�^
	~DateParseListener(void);  // �f�X�g���N�^
	virtual bool hookParse(std::string strHeader, MailHeaderField::MHFTYPE type,
	                       std::ios::pos_type posBeg, std::ios::pos_type posEnd); // Date �t�B�[���h�̉�͌��ʂ��t�b�N����
	time_t mkTime();           // ��͂��������� UTC ��������̌o�ߎ��Ԃ�Ԃ�
};
