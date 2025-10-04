#pragma once

#include <tchar.h>
#include <string>
#include <sstream>
#include <map>
#include <list>

#include "MailIndex.h"
#include "MailHeaderField.h"
#include "MailHeaderParser.h"
#include "DateParseListener.h"

/**
 * ----------------------------------------------------------------------
 * MailHeader
 * ���[���w�b�_�N���X
 * 
 * �w�b�_�̎d�l�� MFC5322 �ɏ�����(cf. http://srgia.com/docs/rfc5322j.html)
 * ----------------------------------------------------------------------
 */
class MailHeader : public MailHeaderParseListener
{
public:
	MailHeader(void);                                                      // �R���X�g���N�^
	MailHeader(std::string strHeader);                                     // �R���X�g���N�^
	~MailHeader(void);                                                     // �f�X�g���N�^

	std::list<MailHeaderField> MailHeader::operator[](const TCHAR* szFieldName);  // []���Z�q�̃I�[�o�[���[�h

	const std::string getHeader() const;                                   // �w�b�_���擾
	void setHeader(std::string strField);                                  // �w�b�_��ݒ�
	const std::list<MailHeaderField> getField(const TCHAR* szName) const;  // �C�ӂ̃t�B�[���h���擾
	void removeField(const TCHAR* szName);                                 // �C�ӂ̃t�B�[���h���폜
	void appendField(const TCHAR* szName, const TCHAR* szContent);         // �C�ӂ̃t�B�[���h��ǉ�
	void appendField(MailHeaderField *field);                              // �C�ӂ̃t�B�[���h��ǉ�
	virtual bool hookParse(std::string strField, MailHeaderField::MHFTYPE type,
	                       std::ios::pos_type posBeg, std::ios::pos_type posEnd);

private:
	std::list<MailHeaderField> listField;                                  // �w�b�_�t�B�[���h
};


/**
 * ----------------------------------------------------------------------
 * MailBody
 * ���[���{�f�B�N���X
 * ----------------------------------------------------------------------
 */
class MailBody
{
public:
	MailBody(void);                               // �R���X�g���N�^
	MailBody(std::string strBody);                // �R���X�g���N�^
	~MailBody(void);                              // �f�X�g���N�^

	std::string MailBody::operator[](int nPage);  // []���Z�q�̃I�[�o�[���[�h

	const std::string getBody() const;            // �{�f�B���擾
	void setBody(std::string strBody);            // �{�f�B��ݒ�
	const std::string getBoundary() const;        // �y�[�W���E��������擾
	void setBoundary(std::string strBoundry);     // �y�[�W���E�������ݒ�

private:
	std::list<std::string> listPage;              // ���[���y�[�W
	std::string strBoundary;                      // �y�[�W���E������
};


/**
 * ----------------------------------------------------------------------
 * Mail
 * ���[���N���X
 * ----------------------------------------------------------------------
 */
class Mail
{
public:
	Mail(void);                               // �R���X�g���N�^
	Mail(std::string strMessage);             // �R���X�g���N�^
	~Mail(void);                              // �f�X�g���N�^

	const std::string getMessage() const;     // ���b�Z�[�W�i�w�b�_�ƃ{�f�B�j���擾
	void setMessage(std::string strMessage);  // ���b�Z�[�W�i�w�b�_�ƃ{�f�B�j��ݒ�
	const MailHeader getHeader() const;       // ���[���w�b�_���擾
	void setHeader(MailHeader mapField);      // ���[���w�b�_��ݒ�
	const MailBody getBody() const;           // ���[���{�f�B���擾
	void setBody(MailBody message);           // ���[���{�f�B��ݒ�
	MailIndex getIndex() const;               // ���[���C���f�b�N�X���擾
	MailIndex* createIndex() const;           // ���[���C���f�b�N�X���쐬

	void parseDateStr(LPCTSTR szField, LPCTSTR szDateFormat, struct tm *tmDate, long int& tm_gmtoff) const;

private:
	MailHeader header;                        // ���[���w�b�_
	MailBody body;                            // ���[���{�f�B

	void setIndex(MailIndex* midx) const;     // ���[���C���f�b�N�X�f�[�^��ݒ�
};
