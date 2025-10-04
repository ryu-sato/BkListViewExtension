#include "Mail.h"
#include "DateTime.h"
#include "MailHeaderParser.h"

/**
 * ----------------------------------------------------------------------
 * MailHeader
 * ���[���w�b�_�N���X
 * ----------------------------------------------------------------------
 */
/**
 * �R���X�g���N�^
 */
MailHeader::MailHeader(void)
{
}

/**
 * �R���X�g���N�^
 */
MailHeader::MailHeader(std::string strHeader)
{
	this->setHeader(strHeader);
}

/**
 * �f�X�g���N�^
 */
MailHeader::~MailHeader(void)
{
}

/**
 * []���Z�q�̃I�[�o�[���[�h
 */
std::list<MailHeaderField> MailHeader::operator[](const TCHAR* szFieldName)
{
	return this->getField(szFieldName);
}

/**
 * �w�b�_���擾
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
 * ��͏������t�b�N���ăw�b�_�t�B�[���h��ۑ�����
 */
bool MailHeader::hookParse(std::string strHeader, MailHeaderField::MHFTYPE type,
						   std::ios::pos_type posBeg, std::ios::pos_type posEnd)
{
	static std::ios::pos_type begField, endField, begContent, endContent;
	static MailHeaderField::MHFTYPE typeField = MailHeaderField::T_NOUSE_MAX;
	static MailHeaderField::MHFTYPE typeContent = MailHeaderField::T_NOUSE_MAX;
	static std::string strFieldName, strContent;

	/* ������ */
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

	/* �w�b�_�t�B�[���h�̉�͎��ɖ{���̃g�[�N���^�C�v���L������ */
	if (typeField == MailHeaderField::T_NOUSE_MAX) {
		if (posBeg == posEnd && MailHeaderField::isField(type)) {
			begField = posBeg;
			typeField = type;
			typeContent = MailHeaderField::getFieldContentType(type);
			return true;
		}
	}

	/* �w�b�_�t�B�[���h�{���̃g�[�N����ۑ����� */
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
 * �w�b�_��ݒ�
 * (fields / obs-fields) [CRLF body]
 */
void MailHeader::setHeader(std::string strField)
{
	MailHeaderParser mhfp(strField, MailHeaderField::HEADER, true);
	mhfp.setListener(this);
	mhfp.parse();
}

/**
 * �C�ӂ̃t�B�[���h���擾
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
 * �C�ӂ̃t�B�[���h���폜
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
 * �C�ӂ̃t�B�[���h��ǉ�
 */
void MailHeader::appendField(const TCHAR* szName, const TCHAR* szContent)
{
	this->listField.push_back(MailHeaderField(szName, szContent));
}

/**
 * �C�ӂ̃t�B�[���h��ǉ�
 */
void MailHeader::appendField(MailHeaderField *field)
{
	this->listField.push_back(MailHeaderField(field->getName(), field->getContent()));
}


/**
 * ----------------------------------------------------------------------
 * MailBody
 * ���[���{�f�B�N���X
 * ----------------------------------------------------------------------
 */
/**
 * �R���X�g���N�^
 */
MailBody::MailBody(void)
{
}

/**
 * �R���X�g���N�^
 */
MailBody::MailBody(std::string strBody)
{
	this->setBody(strBody);
}

/**
 * �f�X�g���N�^
 */
MailBody::~MailBody(void)
{
}

/**
 * []���Z�q�̃I�[�o�[���[�h
 */
std::string MailBody::operator[](int nPage)
{
	// [TODO]
	return "";
}

/**
 * �{�f�B���擾
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
 * �{�f�B��ݒ�
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
 * �y�[�W���E��������擾
 */
const std::string MailBody::getBoundary() const
{
	return this->strBoundary;
}

/**
 * �y�[�W���E�������ݒ�
 */
void MailBody::setBoundary(std::string strBoundry)
{
	this->strBoundary = strBoundary;
}


/**
 * ----------------------------------------------------------------------
 * Mail
 * ���[���N���X
 * ----------------------------------------------------------------------
 */
/**
 * �R���X�g���N�^
 */
Mail::Mail(void)
{
}

/**
 * �R���X�g���N�^
 */
Mail::Mail(std::string strMailSource)
{
	this->setMessage(strMailSource);
}

/**
 * �f�X�g���N�^
 */
Mail::~Mail(void)
{
}

/**
 * ���b�Z�[�W�i�w�b�_�ƃ{�f�B�j���擾
 */
const std::string Mail::getMessage() const
{
	return this->header.getHeader() + "\r\n" + this->body.getBody();
}

/**
 * ���b�Z�[�W�i�w�b�_�ƃ{�f�B�j��ݒ�
 */
void Mail::setMessage(std::string strMailSource)
{
	/* �w�b�_�ƃ{�f�B�̋��E�܂ŃV�[�J��i�߂� */
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

	/* �w�b�_��ݒ� */
	size_t nSizeHeader = iss.tellg();
	std::string strHeader(strMailSource, 0, nSizeHeader);
	MailHeader header(strHeader);
	this->setHeader(header);

	/* �{�f�B��ݒ� */
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
 * ���[���w�b�_���擾
 */
const MailHeader Mail::getHeader() const
{
	return this->header;
}

/**
 * ���[���w�b�_��ݒ�
 */
void Mail::setHeader(MailHeader header)
{
	this->header = header;
}

/**
 * ���[���{�f�B���擾
 */
const MailBody Mail::getBody() const
{
	return this->body;
}

/**
 * ���[���{�f�B��ݒ�
 */
void Mail::setBody(MailBody body)
{
	this->body = body;
}

/**
 * ������ŕ\�����ꂽ���t����͂���
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

	/* ���t�t�H�[�}�b�g�𒲂ׂ� */
	DateTime::DTFORMAT format = DateTime::DTF_RFC5822;
	for (int i = 0; i < sizeof(dateformat) / sizeof(dateformat[0]); i++) {
		if (_strnicmp(szDateFormat, dateformat[i].name, lstrlen(dateformat[i].name)) == 0) {
			format = dateformat[i].format;
		}
	}

	/* ���t�t�H�[�}�b�g�ɏ]���ē��t���������͂��� */
	DateTime dt;
	memset(tmDate, 0, sizeof(struct tm));
	tm_gmtoff = 0;
	dt.Parse(szField, format, tmDate, tm_gmtoff);
}

/**
 * ���[���C���f�b�N�X���擾
 */
MailIndex Mail::getIndex() const
{
	MailIndex midx;
	this->setIndex(&midx);
	return midx;
}

/**
 * ���[���C���f�b�N�X���擾
 */
MailIndex* Mail::createIndex() const
{
	MailIndex *midx = new MailIndex();
	this->setIndex(midx);
	return midx;
}

/**
 * ���[���C���f�b�N�X�f�[�^��ݒ�
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

	/* ���[�����M�����iDate�j���擾���� */
	const std::list<MailHeaderField> listDate = this->header.getField("Date");
	if (listDate.size() > 0) {
		const std::string strDate = listDate.back().getContent();
		DateParseListener dpl;
		MailHeaderParser dateParser(strDate, MailHeaderField::T_DATE_TIME, true);
		dateParser.setListener(&dpl);
		dateParser.parse();
		time_t tSend = dpl.mkTime();
		if (tSend < 0) {
			// [TODO] error ����
			midx->tSend = 0;
		} else {
			midx->tSend = tSend;
		}
	}

	/* ���[���z�M�����iReceived�j���擾���� */
	midx->tRecv = midx->tSend;
	const std::list<MailHeaderField> listReceived = this->header.getField("Received");
	if (listReceived.size() > 0) {
		const std::string strDate = "Received:" + listReceived.front().getContent();
		DateParseListener dpl;
		// [TODO] Received ���p�[�X�ł��Ȃ����ߔz�M�������擾�ł��Ȃ��̂�����������
		MailHeaderParser dateParser(strDate, MailHeaderField::RECEIVED, true);
		dateParser.setListener(&dpl);
		dateParser.parse();
		time_t tRecv = dpl.mkTime();
		if (tRecv < 0) {
			// [TODO] error ����
			midx->tRecv = 0;
		} else {
			midx->tRecv = tRecv;
		}
	}

	/* ���[����M�����i���[�������[����ۑ����������j���擾���� */
	;
}
