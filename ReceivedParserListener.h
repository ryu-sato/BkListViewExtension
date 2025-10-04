#pragma once
#include "MailHeaderParser.h"

/**
 * ----------------------------------------------------------------------
 * ReceivedParserListener
 * Received �t�B�[���h�̉�̓N���X
 * ----------------------------------------------------------------------
 */
class ReceivedParserListener :
	public MailHeaderParser
{
public:
	/* public �v���p�e�B */
	struct tm tmDate;  // ��͂��������̍\����
	int tm_gmtoffset;  // ��͂��������� GMT ����̃I�t�Z�b�g�b

	/* public ���\�b�h */
	ReceivedParserListener(void);  // �R���X�g���N�^
	~ReceivedParserListener(void); // �f�X�g���N�^
	virtual bool hookParse(std::string strHeader, MailHeaderField::MHFTYPE type,
	                       std::ios::pos_type posBeg, std::ios::pos_type posEnd);  // Received �t�B�[���h�̉�͌��ʂ��t�b�N����
	time_t mkTime();               // ��͂��������� UTC ��������̌o�ߎ��Ԃ�Ԃ�
};
