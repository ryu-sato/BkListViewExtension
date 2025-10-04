#include "Mail.h"
#include "DateTime.h"
#include "MailHeaderParser.h"

/**
 * ----------------------------------------------------------------------
 * MailHeader
 * メールヘッダクラス
 * ----------------------------------------------------------------------
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
std::list<MailHeaderField> MailHeader::operator[](const TCHAR* szFieldName)
{
	return this->getField(szFieldName);
}

/**
 * ヘッダを取得
 */
const std::string MailHeader::getHeader() const
{
	std::string strHeader;
	for (std::list<MailHeaderField>::const_iterator i = this->listField.begin(); i != this->listField.end(); i++) {
		strHeader += i->getName();
		strHeader += ":";
		strHeader += i->getContent();
		strHeader += "\r\n";
	}
	return strHeader;
}

/**
 * 解析処理をフックしてヘッダフィールドを保存する
 */
bool MailHeader::hookParse(std::string strHeader, MailHeaderField::MHFTYPE type,
						   std::ios::pos_type posBeg, std::ios::pos_type posEnd)
{
	static std::ios::pos_type begField, endField, begContent, endContent;
	static MailHeaderField::MHFTYPE typeField = MailHeaderField::T_NOUSE_MAX;
	static MailHeaderField::MHFTYPE typeContent = MailHeaderField::T_NOUSE_MAX;
	static std::string strFieldName, strContent;

	/* 初期化 */
	if (type == typeField && posBeg != posEnd) {
		begField = 0;
		endField = 0;
		begContent = 0;
		endContent = 0;
		typeField = MailHeaderField::T_NOUSE_MAX;
		typeContent = MailHeaderField::T_NOUSE_MAX;
		strFieldName = _TEXT("");
		strContent = _TEXT("");
		return true;
	}

	/* ヘッダフィールドの解析時に本文のトークンタイプを記憶する */
	if (typeField == MailHeaderField::T_NOUSE_MAX) {
		if (posBeg == posEnd && MailHeaderField::isField(type)) {
			begField = posBeg;
			typeField = type;
			typeContent = MailHeaderField::getFieldContentType(type);
			return true;
		}
	}

	/* ヘッダフィールド本文のトークンを保存する */
	if (type == typeContent) {
		if (posBeg == posEnd) {
			begContent = posBeg;
			if (typeField == MailHeaderField::OPTIONAL_FIELD
				|| typeField == MailHeaderField::OBS_OPTIONAL) {
				std::string::size_type index = strHeader.find(':', begField);
				if (index != std::string::npos) {
					strFieldName = strHeader.substr(begField, index - begField);
				} else {
					strFieldName = _TEXT("_UNKNOWN_NAME_");
				}
			} else {
				strFieldName = MailHeaderField::getFieldName(typeField);
			}
			return true;
		} else {
			endContent = posEnd;
			strContent = strHeader.substr(begContent, endContent - begContent);
			appendField(strFieldName.c_str(), strContent.c_str());
			return true;
		}
	}
	return true;
}


/**
 * ヘッダを設定
 * (fields / obs-fields) [CRLF body]
 */
void MailHeader::setHeader(std::string strField)
{
	MailHeaderParser mhfp(strField, MailHeaderField::HEADER, true);
	mhfp.setListener(this);
	mhfp.parse();
}

/**
 * 任意のフィールドを取得
 */
const std::list<MailHeaderField> MailHeader::getField(const TCHAR* szName) const
{
	std::list<MailHeaderField> listField;
	for (std::list<MailHeaderField>::const_iterator i = this->listField.begin(); i != this->listField.end(); i++) {
		if (_tcscmp(szName, i->getName().c_str()) == 0) {
			listField.push_back(*i);
		}
	}
	return listField;
}

/**
 * 任意のフィールドを削除
 */
void MailHeader::removeField(const TCHAR* szName)
{
	std::string strHeader;
	for (std::list<MailHeaderField>::const_iterator i = this->listField.begin(); i != this->listField.end(); ) {
		if (_tcscmp(szName, i->getName().c_str()) == 0) {
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
	this->listField.push_back(MailHeaderField(szName, szContent));
}

/**
 * 任意のフィールドを追加
 */
void MailHeader::appendField(MailHeaderField *field)
{
	this->listField.push_back(MailHeaderField(field->getName(), field->getContent()));
}


/**
 * ----------------------------------------------------------------------
 * MailBody
 * メールボディクラス
 * ----------------------------------------------------------------------
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
 * ----------------------------------------------------------------------
 * Mail
 * メールクラス
 * ----------------------------------------------------------------------
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
Mail::Mail(std::string strMailSource)
{
	this->setMessage(strMailSource);
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
void Mail::setMessage(std::string strMailSource)
{
	/* ヘッダとボディの境界までシーカを進める */
	std::istringstream iss(strMailSource);
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
	std::string strHeader(strMailSource, 0, nSizeHeader);
	MailHeader header(strHeader);
	this->setHeader(header);

	/* ボディを設定 */
	iss.seekg(0, std::ios::end);
	size_t nSizeBody = (size_t) iss.tellg() - nSizeHeader;
	std::string strBody(strMailSource, nSizeHeader, nSizeBody);
	MailBody body(strBody);
	const std::list<MailHeaderField> listContentType = header.getField("Content-Type");
	if (listContentType.size() > 0) {
		std::string strContentType = listContentType.back().getContent();
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
	struct __struct_date_format {
		char *name;
		DateTime::DTFORMAT format;
	} dateformat[] = {
		{ "rfc2822", DateTime::DTF_RFC2822 },
		{ "rfc5822", DateTime::DTF_RFC5822 },
	};

	/* 日付フォーマットを調べる */
	DateTime::DTFORMAT format = DateTime::DTF_RFC5822;
	for (int i = 0; i < sizeof(dateformat) / sizeof(dateformat[0]); i++) {
		if (_strnicmp(szDateFormat, dateformat[i].name, lstrlen(dateformat[i].name)) == 0) {
			format = dateformat[i].format;
		}
	}

	/* 日付フォーマットに従って日付文字列を解析する */
	DateTime dt;
	memset(tmDate, 0, sizeof(struct tm));
	tm_gmtoff = 0;
	dt.Parse(szField, format, tmDate, tm_gmtoff);
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
#define CLASS_MEMBER(mem, fieldname) { &midx->mem, fieldname }
	struct class_member {
		void *val;
		TCHAR *fieldname;
	} member[] = {
		CLASS_MEMBER(strSubject, "Subject"), CLASS_MEMBER(strTo,    "To"), 
		CLASS_MEMBER(strFrom,    "From"),    CLASS_MEMBER(strMsgId, "Message-ID"),
	};

	for (int i = 0; i < sizeof(member) / sizeof(member[0]); i++) {
		const std::list<MailHeaderField> listTargetField = this->header.getField(member[i].fieldname);
		if (listTargetField.size() > 0) {
			std::string strBuf = listTargetField.back().getContent();
			*((std::basic_string<TCHAR>*) member[i].val) = strBuf.c_str();
		}
	}

	/* メール送信日時（Date）を取得する */
	const std::list<MailHeaderField> listDate = this->header.getField("Date");
	if (listDate.size() > 0) {
		const std::string strDate = listDate.back().getContent();
		DateParseListener dpl;
		MailHeaderParser dateParser(strDate, MailHeaderField::T_DATE_TIME, true);
		dateParser.setListener(&dpl);
		dateParser.parse();
		time_t tSend = dpl.mkTime();
		if (tSend < 0) {
			// [TODO] error 処理
			midx->tSend = 0;
		} else {
			midx->tSend = tSend;
		}
	}

	/* メール配信日時（Received）を取得する */
	midx->tRecv = midx->tSend;
	const std::list<MailHeaderField> listReceived = this->header.getField("Received");
	if (listReceived.size() > 0) {
		const std::string strDate = "Received:" + listReceived.front().getContent();
		DateParseListener dpl;
		// [TODO] Received がパースできないため配信日時が取得できないのを解決させる
		MailHeaderParser dateParser(strDate, MailHeaderField::RECEIVED, true);
		dateParser.setListener(&dpl);
		dateParser.parse();
		time_t tRecv = dpl.mkTime();
		if (tRecv < 0) {
			// [TODO] error 処理
			midx->tRecv = 0;
		} else {
			midx->tRecv = tRecv;
		}
	}

	/* メール受信日時（メーラがメールを保存した時刻）を取得する */
	;
}
