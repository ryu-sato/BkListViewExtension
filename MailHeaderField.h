/**
 * ----------------------------------------------------------------------
 * MailHeaderField
 * ���[���w�b�_�̃t�B�[���h�Ǘ��N���X
 * 
 * MailHeaderField �̓t�B�[���h���Ɩ{�����Z�b�g�ƂȂ��Ă���B
 * �t�B�[���h���� RFC�� �Œ�`���ꂽ���O�ł���B
 * �{���͓����f�[�^�`���iUTF8�j�ŕۑ������B
 * ----------------------------------------------------------------------
 */

#pragma once

#include <tchar.h>
#include <string>

class MailHeaderField
{
public:
	/* �t�B�[���h�^�C�v */
	typedef enum {
		/* �t�B�[���h */
		HEADER,			FIELDS,			ORIG_DATE,		FROM,			SENDER,
		REPLY_TO,		TO,				CC,				BCC,			MESSAGE_ID,
		INREPLY_TO,		REFERENCES,		SUBJECT,		COMMENTS,		KEYWORDS,
		RESENT_DATE,	RESENT_FROM,	RESENT_SENDER,	RESENT_TO,		RESENT_CC,
		RESENT_BCC,		RESENT_MSGID,	TRACE,			RETURN,			RECEIVED,
		OPTIONAL_FIELD,	OBS_FIELDS,		OBS_HEADER,		OBS_ORIG_DATE,	OBS_FROM,
		OBS_SENDER,		OBS_REPLY_TO,	OBS_TO,			OBS_CC,			OBS_BCC,
		OBS_MESSAGE_ID,	OBS_INREPLY_TO,	OBS_REFERENCES,	OBS_SUBJECT,	OBS_COMMENTS,
		OBS_KEYWORDS,	OBS_RESENT_FROM,OBS_RESENT_SEND,OBS_RESENT_DATE,OBS_RESENT_TO,
		OBS_RESENT_CC,	OBS_RESENT_BCC,	OBS_RESENT_MID,	OBS_RESENT_RPLY,OBS_RETURN,
		OBS_RECEIVED,	OBS_OPTIONAL,

		/* �g�[�N�� */
		T_ADDREE_LIST_OR_CFWS_LIST,		T_ADDRESS_LIST_OR_CFWS,		T_PHRASE_LIST,
		T_N_RECEIVED_TOKEN_AND_DATE_TIME,							T_PHRASE_OR_MSGID,
		T_N_PHRASE_OR_MSGID,			T_N_RECEIVED_TOKEN,

		T_DQUOTE,			T_WSP,				T_SP,				T_HTAB,		
		T_CRLF,				T_ALPHA,			T_DIGIT,			T_VCHAR,
		T_NO_WS_CTL,		T_TEXT,				T_SPECIALS,			T_QUOTED_PAIR,
		T_FWS,				T_CTEXT,			T_CCONTENT,			T_COMMENT,
		T_CFWS,				T_ATEXT,			T_ATOM,				T_DOT_ATOM,
		T_DOT_ATOM_TEXT,	T_QTEXT,			T_QCONTENT,			T_QUOTED_STRING,
		T_WORD,				T_PHRASE,			T_UNSTRUCTURED,		T_DATE_TIME,
		T_DAY_OF_WEEK,		T_DAY_NAME,			T_DATE,				T_DAY,
		T_MONTH,			T_YEAR,				T_TIME,				T_TIME_OF_DAY,
		T_HOUR,				T_MINUTE,			T_SECOND,			T_ZONE,
		T_ADDRESS,			T_MAILBOX,			T_NAME_ADDR,		T_ANGLE_ADDR,
		T_GROUP,			T_DISPLAY_NAME,		T_MAILBOX_LIST,		T_ADDRESS_LIST,
		T_GROUP_LIST,		T_ADDR_SPEC,		T_LOCAL_PART,		T_DOMAIN,
		T_DOMAIN_LITERAL,	T_DTEXT,			T_MSG_ID,			T_N_MSG_ID,
		T_ID_LEFT,			T_ID_RIGHT,			T_NO_FOLD_LITERAL,	T_PATH,
		T_RECEIVED_TOKEN,	T_FIELD_NAME,		T_FTEXT,			T_OBS_NO_WS_CTL,
		T_OBS_CTEXT,		T_OBS_QTEXT,		T_OBS_UTEXT,		T_OBS_QP,
		T_OBS_BODY,			T_OBS_UNSTRUCT,		T_OBS_PHRASE,		T_OBS_PHRASE_LIST,
		T_OBS_FWS,			T_OBS_DAY_OF_WEEK,	T_OBS_DAY,			T_OBS_YEAR,
		T_OBS_HOUR,			T_OBS_MINUTE,		T_OBS_SECOND,		T_OBS_ZONE,
		T_OBS_ANGLE_ADDR,	T_OBS_ROUTE,		T_OBS_DOMAIN_LIST,	T_OBS_MBOX_LIST,
		T_OBS_ADDR_LIST,	T_OBS_GROUP_LIST,	T_OBS_LOCAL_PART,	T_OBS_DOMAIN,
		T_OBS_DTEXT,		T_OBS_ID_LEFT,		T_OBS_ID_RIGHT,

		/* �����l�i�ԕ��j */
		T_NOUSE_MAX
	} MHFTYPE;

	MailHeaderField(void);                                        // �R���X�g���N�^
	MailHeaderField(std::string strName, std::string strContent); // �R���X�g���N�^
	~MailHeaderField(void);                                       // �f�X�g���N�^
	const std::string getName() const;                            // �t�B�[���h�����擾����
	void setName(std::string strName);                            // �t�B�[���h����ݒ肷��
	const std::string getContent() const;                         // �t�B�[���h�{�����擾����
	void setContent(const std::string strContent);                // �t�B�[���h�{����ݒ肷��
	static const std::string getFieldName(MHFTYPE fieldType);     // �t�B�[���h�^�C�v�ɉ������t�B�[���h�����擾����
	static MHFTYPE getFieldType(std::string strFieldName);        // �t�B�[���h�^�C�v���擾����
	static MHFTYPE getFieldContentType(MHFTYPE fieldType);        // �t�B�[���h�{�̂̃^�C�v���擾����
	static bool isField(MHFTYPE fieldType);                       // �t�B�[���h�^�C�v�ł��邩
	static bool isFieldToken(MHFTYPE fieldType);                  // �g�[�N���^�C�v�ł��邩

private:
	typedef struct {
		MHFTYPE fieldType;         // �t�B�[���h�^�C�v
		std::string name;          // �t�B�[���h��
		MHFTYPE fieldContentType;  // �t�B�[���h�̖{�����`����t�B�[���h��
	} FIELDINFO;                   // �t�B�[���h���
	static const FIELDINFO fieldInfo[];  // ���[���w�b�_�t�B�[���h���
	std::string strName;                 // �t�B�[���h��
	std::string strContent;              // �t�B�[���h�{��
	
};
