/**
 * ----------------------------------------------------------------------
 * MailHeaderField
 * メールヘッダのフィールド管理クラス
 * ----------------------------------------------------------------------
 */

/* クラスの定義 */
#include "MailHeaderField.h"

/* メールヘッダフィールド情報 */
const MailHeaderField::FIELDINFO MailHeaderField::fieldInfo[] = {
	{ ORIG_DATE      , _TEXT("Date")             , T_DATE_TIME                      },
	{ FROM           , _TEXT("From")             , T_MAILBOX_LIST                   },
	{ SENDER         , _TEXT("Sender")           , T_MAILBOX                        },
	{ REPLY_TO       , _TEXT("Reply-To")         , T_ADDRESS_LIST                   },
	{ TO             , _TEXT("To")               , T_ADDRESS_LIST                   },
	{ CC             , _TEXT("Cc")               , T_ADDRESS_LIST                   },
	{ BCC            , _TEXT("Bcc")              , T_ADDRESS_LIST                   },
	{ MESSAGE_ID     , _TEXT("Message-ID")       , T_MSG_ID                         },
	{ INREPLY_TO     , _TEXT("In-Reply-To")      , T_N_MSG_ID                       },
	{ REFERENCES     , _TEXT("References")       , T_N_MSG_ID                       },
	{ SUBJECT        , _TEXT("Subject")          , T_UNSTRUCTURED                   },
	{ COMMENTS       , _TEXT("Comments")         , T_UNSTRUCTURED                   },
	{ KEYWORDS       , _TEXT("Keywords")         , T_PHRASE_LIST                    },
	{ RESENT_DATE    , _TEXT("Resent-Date")      , T_DATE_TIME                      },
	{ RESENT_FROM    , _TEXT("Resent-From")      , T_MAILBOX_LIST                   },
	{ RESENT_SENDER  , _TEXT("Resent-Sender")    , T_MAILBOX                        },
	{ RESENT_TO      , _TEXT("Resent-To")        , T_ADDRESS_LIST                   },
	{ RESENT_CC      , _TEXT("Resent-Cc")        , T_ADDRESS_LIST                   },
	{ RESENT_BCC     , _TEXT("Resent-Bcc")       , T_ADDRESS_LIST                   },
	{ RESENT_MSGID   , _TEXT("Resent-Message-ID"), T_MSG_ID                         },
	{ RETURN         , _TEXT("Return-Path")      , T_PATH                           },
	{ RECEIVED       , _TEXT("Received")         , T_N_RECEIVED_TOKEN_AND_DATE_TIME },
	{ OPTIONAL_FIELD , _TEXT("")                 , T_UNSTRUCTURED                   },
	{ OBS_ORIG_DATE  , _TEXT("Date")             , T_DATE_TIME                      },
	{ OBS_FROM       , _TEXT("From")             , T_MAILBOX_LIST                   },
	{ OBS_SENDER     , _TEXT("Sender")           , T_MAILBOX                        },
	{ OBS_REPLY_TO   , _TEXT("Reply-To")         , T_ADDRESS_LIST                   },
	{ OBS_TO         , _TEXT("To")               , T_ADDRESS_LIST                   },
	{ OBS_CC         , _TEXT("Cc")               , T_ADDRESS_LIST                   },
	{ OBS_BCC        , _TEXT("Bcc")              , T_ADDRESS_LIST                   },
	{ OBS_MESSAGE_ID , _TEXT("Message-ID")       , T_MSG_ID                         },
	{ OBS_INREPLY_TO , _TEXT("In-Reply-To")      , T_N_PHRASE_OR_MSGID              },
	{ OBS_REFERENCES , _TEXT("References")       , T_N_PHRASE_OR_MSGID              },
	{ OBS_SUBJECT    , _TEXT("Subject")          , T_UNSTRUCTURED                   },
	{ OBS_COMMENTS   , _TEXT("Comments")         , T_UNSTRUCTURED                   },
	{ OBS_KEYWORDS   , _TEXT("Keywords")         , T_OBS_PHRASE_LIST                },
	{ OBS_RESENT_FROM, _TEXT("Resent-From")      , T_MAILBOX_LIST                   },
	{ OBS_RESENT_SEND, _TEXT("Resent-Sender")    , T_MAILBOX                        },
	{ OBS_RESENT_DATE, _TEXT("Resent-Date")      , T_DATE_TIME                      },
	{ OBS_RESENT_TO  , _TEXT("Resent-To")        , T_ADDRESS_LIST                   },
	{ OBS_RESENT_CC  , _TEXT("Resent-Cc")        , T_ADDRESS_LIST                   },
	{ OBS_RESENT_BCC , _TEXT("Resent-Bcc")       , T_ADDRESS_LIST                   },
	{ OBS_RESENT_MID , _TEXT("Resent-Message-ID"), T_MSG_ID                         },
	{ OBS_RESENT_RPLY, _TEXT("Resent-Reply-To")  , T_ADDRESS_LIST                   },
	{ OBS_RETURN     , _TEXT("Return-Path")      , T_PATH                           },
	{ OBS_RECEIVED   , _TEXT("Received")         , T_N_RECEIVED_TOKEN               },
	{ OBS_OPTIONAL   , _TEXT("")                 , T_UNSTRUCTURED                   },
};

/**
 * コンストラクタ
 */
MailHeaderField::MailHeaderField(void)
{
	this->setName("");
	this->setContent("");
}

/**
 * コンストラクタ
 */
MailHeaderField::MailHeaderField(std::string strName, std::string strContent)
{
	this->setName(strName);
	this->setContent(strContent);
}

/**
 * デストラクタ
 */
MailHeaderField::~MailHeaderField(void)
{
}

/**
 * フィールド名を取得する
 */
const std::string MailHeaderField::getName() const
{
	return this->strName;
}

/**
 * フィールド名を設定する
 */
void MailHeaderField::setName(const std::string strName)
{
	this->strName = strName;
}

/**
 * フィールド本文を取得する
 */
const std::string MailHeaderField::getContent() const
{
	return this->strContent;
}

/**
 * フィールド本文を設定する
 */
void MailHeaderField::setContent(std::string strContent)
{
	this->strContent = strContent;
}

/**
 * フィールドタイプに応じたフィールド名を取得する
 */
const std::string MailHeaderField::getFieldName(MailHeaderField::MHFTYPE fieldType)
{
	for (int i = 0; i < sizeof(fieldInfo) / sizeof(fieldInfo[0]); i++) {
		if (fieldInfo[i].fieldType == fieldType) {
			return fieldInfo[i].name;
		}
	}
	return "";
}

/**
 * フィールドタイプを取得する
 */
MailHeaderField::MHFTYPE MailHeaderField::getFieldType(std::string strFieldName)
{
	for (int i = 0; i < sizeof(fieldInfo) / sizeof(fieldInfo[0]); i++) {
		if (fieldInfo[i].name == strFieldName) {
			return fieldInfo[i].fieldType;
		}
	}
	return MailHeaderField::T_NOUSE_MAX;
}

/**
 * フィールド本体のタイプを取得する
 */
MailHeaderField::MHFTYPE MailHeaderField::getFieldContentType(MailHeaderField::MHFTYPE fieldType)
{
	for (int i = 0; i < sizeof(fieldInfo) / sizeof(fieldInfo[0]); i++) {
		if (fieldInfo[i].fieldType == fieldType) {
			return fieldInfo[i].fieldContentType;
		}
	}
	return MailHeaderField::T_NOUSE_MAX;
}

/**
 * フィールドタイプであるか
 */
bool MailHeaderField::isField(MHFTYPE fieldType)
{
	return (HEADER <= fieldType && fieldType <= OBS_OPTIONAL
	        && fieldType != HEADER && fieldType != FIELDS && fieldType != TRACE
			&& fieldType != OBS_FIELDS && fieldType != OBS_HEADER);
}

/**
 * トークンタイプであるか
 */
bool MailHeaderField::isFieldToken(MHFTYPE fieldType)
{
	return (!isField(fieldType) && fieldType < T_NOUSE_MAX);
}
