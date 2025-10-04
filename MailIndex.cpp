#include "MailIndex.h"
#include "BkCommon.h"
#include "Logger.h"

/**
 * コンストラクタ
 */
MailIndex::MailIndex()
{
	initialize();  // プロパティを初期化
}

/**
 * コンストラクタ
 */
MailIndex::MailIndex(LPCTSTR pbkMailID, LPCTSTR pMailIndex)
{
	initialize();  // プロパティを初期化

#define CLASS_MEMBER(m) { typeid(this->m), &this->m, sizeof(this->m) }
	struct class_member {
		const type_info& ti;
		void *val; 
		int size;
	} member[] = {
		CLASS_MEMBER(dwBodyPtr),  CLASS_MEMBER(dwMsgID),  CLASS_MEMBER(dwFileName),
		CLASS_MEMBER(strSubject), CLASS_MEMBER(strFrom),  CLASS_MEMBER(strTo),
		CLASS_MEMBER(strMsgId),   CLASS_MEMBER(strReferences),
		CLASS_MEMBER(tSend),      CLASS_MEMBER(tRecv),    CLASS_MEMBER(tDnld),
		CLASS_MEMBER(dwSize),     CLASS_MEMBER(dwStatus), CLASS_MEMBER(nColor),
		CLASS_MEMBER(nPriority),  CLASS_MEMBER(dwParentID),
		CLASS_MEMBER(strCharSet), CLASS_MEMBER(strTemp),  CLASS_MEMBER(strExtAtch),
	};

	this->strBkMailID = pbkMailID;
	istringstream p(pMailIndex);
	basic_string<TCHAR> elem;
	for (int col = 0; getline(p, elem, '\x01'); col++) {
		if (col < sizeof(member) / sizeof(member[0])) {
			if (member[col].ti == typeid(basic_string<TCHAR>)) {
				*((basic_string<TCHAR>*) member[col].val) = elem;
			} else if (member[col].ti == typeid(DWORD)
				|| member[col].ti == typeid(time_t)
				|| member[col].ti == typeid(short)
				|| member[col].ti == typeid(COLORREF)) {
				 _stscanf_s(elem.c_str(), _T("%x"), member[col].val);
			} else {
				Logger::getInstance().print(_T("unknown type value."));
				memcpy(member[col].val, elem.c_str(), member[col].size);
			}
		} else {
			this->listExt.push_back(elem);
		}
	}
}

/**
 * コピーコンストラクタ
 */
MailIndex::MailIndex(const MailIndex& copyFrom)
{
	this->strBkMailID = copyFrom.strBkMailID;
	this->dwBodyPtr = copyFrom.dwBodyPtr;
	this->dwMsgID = copyFrom.dwMsgID;
	this->dwFileName = copyFrom.dwFileName;
	this->strSubject = copyFrom.strSubject;
	this->strTo = copyFrom.strTo;
	this->strFrom = copyFrom.strFrom;
	this->strMsgId = copyFrom.strMsgId;
	this->strReferences = copyFrom.strReferences;
	this->tSend = copyFrom.tSend;
	this->tRecv = copyFrom.tRecv;
	this->tDnld = copyFrom.tDnld;
	this->dwSize = copyFrom.dwSize;
	this->dwStatus = copyFrom.dwStatus;
	this->nColor = copyFrom.nColor;
	this->nPriority = copyFrom.nPriority;
	this->dwParentID = copyFrom.dwParentID;
	this->strCharSet = copyFrom.strCharSet;
	this->strTemp = copyFrom.strTemp;
	this->strExtAtch = copyFrom.strExtAtch;

	for (list<basic_string<TCHAR>>::const_iterator i = copyFrom.listExt.begin(); i != copyFrom.listExt.end(); i++) {
		this->listExt.push_back(*i);
	}
}

/**
 * デストラクタ
 */
MailIndex::~MailIndex()
{
	this->listExt.clear();
}

/**
 * メールの参照先のMessage-Idリスト
 */
list<basic_string<TCHAR>> MailIndex::getReferences() const
{
	list<basic_string<TCHAR>> listReferences;

	basic_string<TCHAR> buf = this->strReferences;
	bool is_sep = true;
	for (basic_string<TCHAR>::iterator p = buf.begin(); p != buf.end(); p++) {
		if (is_sep && *p == _T('<')) {
			is_sep = false;
			continue;
		} else if (!is_sep && *p == _T('>')) {
			is_sep = true;
			continue;
		}
		if (is_sep)
			*p = _T('\x01');
	}

	istringstream q(buf);
	basic_string<TCHAR> msgid;
	while (getline(q, msgid, _T('\x01'))) {
		if (msgid.empty())
			continue;
		listReferences.push_back(msgid);
	}

	return listReferences;
}

std::basic_string<TCHAR> MailIndex::getACPSubject() const
{
	Charset::Encoding encTo((UINT) CP_ACP);
	Charset::Encoding encFrom(this->strCharSet);
	return encTo.getString(encTo.getWBytes(encFrom.getWString(encFrom.getBytes(this->strSubject))));
}

std::basic_string<WCHAR> MailIndex::getWSubject() const
{
	Charset::Encoding encFrom(this->strCharSet.c_str());
	return encFrom.getWString(encFrom.getBytes(this->strSubject));
}

/**
 * 整形された時刻文字列（送信/受信/取得）を取得
 */
std::basic_string<TCHAR> MailIndex::getFormatedTime(LPCTSTR szFormat, MailIndex::DATETYPE type) const
{
	TCHAR szTime[1024];
	struct tm tmDate = {{0}};
	switch (type) {
		case MailIndex::DATETYPE_SEND:
			localtime_s(&tmDate, &this->tSend);
			break;
		case MailIndex::DATETYPE_RECV:
			localtime_s(&tmDate, &this->tRecv);
			break;
		case MailIndex::DATETYPE_DNLD:
			localtime_s(&tmDate, &this->tDnld);
			break;
	}
	_tcsftime(szTime, sizeof(szTime), szFormat, &tmDate);
	return szTime;
}

std::basic_string<TCHAR> MailIndex::getMessageID(MailIndex::MSGIDFMT format) const
{
	switch (format) {
	case MailIndex::MSGIDFMT_NOLTFT:
		TCHAR szMessageID[1024];
		basic_istringstream<TCHAR> ss(this->strMsgId);
		ios::pos_type beg = ss.ignore(this->strMsgId.length(), _T('<')).tellg();
		ss.getline(szMessageID, sizeof(szMessageID) / sizeof(szMessageID[0]), _T('>'));
		return szMessageID;
	}
	return this->strMsgId;
}

/**
 * サブジェクトを取得
 */
std::basic_string<TCHAR> MailIndex::getSubject(MailIndex::SBJFMT format, Charset::Encoding encode) const
{
	std::basic_string<WCHAR> strWSubject;
	Charset::Encoding encFrom(this->strCharSet);
	strWSubject = encFrom.getWString(encFrom.getBytes(this->strSubject));

	switch (format) {
		/* 先頭の RE:, FW: を削除する */
		case MailIndex::SBJFMT_NOREFW: {
#ifdef _UNICODE
			std::basic_string<WCHAR> strRE = _T("re:");
			std::basic_string<WCHAR> strFW = _T("fw:");
#else
			std::basic_string<WCHAR> strRE = Charset::Encoding::ACP.getWString(Charset::Encoding::ACP.getBytes(_T("re:")));
			std::basic_string<WCHAR> strFW = Charset::Encoding::ACP.getWString(Charset::Encoding::ACP.getBytes(_T("fw:")));
#endif
			std::basic_string<WCHAR>::iterator ite = strWSubject.begin();
			while (iswspace(*ite))
				ite++;
			if (_wcsnicmp(&*ite, strRE.c_str(), strRE.length()) == 0)
				ite += strRE.length();
			if (_wcsnicmp(&*ite, strFW.c_str(), strFW.length()) == 0)
				ite += strFW.length();
			while (iswspace(*ite))
				ite++;
			strWSubject.erase(strWSubject.begin(), ite);
			return encode.getString(encode.getWBytes(strWSubject));
		}

		/* 先頭の ML シーケンス( (ML xxxxx), [ML xxxx] ) を削除する */
		case MailIndex::SBJFMT_NOMLSEQ: {
			std::basic_istringstream<WCHAR> ss(strWSubject);
			while (iswspace(ss.peek()))
				ss.seekg(1, std::ios::cur);
			WCHAR mark[][2] = { _T('()'), _T('[]'), _T('{}') };
			for (int i = 0; i < sizeof(mark) / sizeof(mark[0]); i++) {
				if (ss.peek() == mark[i][0]) {
					ss.ignore(strWSubject.length(), mark[i][1]);
					break;
				}
			}
			while (iswspace(ss.peek()))
				ss.seekg(1, std::ios::cur);
		}
	}
	return encode.getString(encode.getBytes(this->strSubject));
}

/**
 * 送信者を取得
 */
std::basic_string<TCHAR> MailIndex::getFrom(MailIndex::FROMFMT format = MailIndex::FROMFMT_DEFAULT) const
{
	switch (format) {
	case MailIndex::FROMFMT_DEFAULT:
	default:
		return this->strFrom;
	case MailIndex::FROMFMT_NAME:
	case MailIndex::FROMFMT_ADDRESS: {
		char szName[1024] = _T("");
		char szAddr[1024] = _T("");
		GetNameAndAddr(szName, sizeof(szName), szAddr, sizeof(szAddr), this->strFrom.c_str());
		if (format == MailIndex::FROMFMT_NAME)
			return szName;
		else // MailIndex::FROMFMT_ADDRESS
			return szAddr;
		}
	}
}

/* private 関数群 */
void MailIndex::initialize()
{
	this->dwBodyPtr = 0;
	this->dwMsgID = 0;
	this->dwFileName = 0;
	this->tSend = 0;
	this->tRecv = 0;
	this->tDnld = 0;
	this->dwSize = 0;
	this->dwStatus = 0;
	this->nColor = 0;
	this->nPriority = 0;
	this->dwParentID = 0;
}
