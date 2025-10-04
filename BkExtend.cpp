#include "BkExtend.h"
#include "BkCommon.h"
#include "BkListViewExtension.h"
#include "DateTime.h"
#include "Mail.h"

#pragma comment(lib, "shlwapi.lib")

using namespace std;

/**
 * ���[�����I������Ă��邩
 */
BOOL bkIsMailSelected()
{
	TCHAR szMailID[256];
	int nMailPos = 0;
	nMailPos = bka.GetNextMail(-1, szMailID, sizeof(szMailID), TRUE);
	return (nMailPos != -1);
}

/**
 * �X���b�h���[�h�̗L���ł��邩
 * #\xxxxx.mb#tatsurou@mail.sfc.wide.ad.jp#INBOX[1f].ini �ɂ� Status Thread=1 �Ȃ�L��
 */
BOOL bkIsThreadMode()
{
	basic_string<TCHAR> strIni = bkGetFolderIniFilePath(bka.GetDataFolder(), bka.GetCurrentFolder());
	int isThreadMode = GetPrivateProfileInt(_T("Status"), _T("Thread"), 0, strIni.c_str());
	return (isThreadMode != 0);
}

/**
 * ���[���C���f�b�N�X�t�@�C���iFolder.idx�j�ւ̃p�X���擾
 */
LPCTSTR bkGetFolderIndexFilePath(LPCTSTR pDataFolder, LPCTSTR pRelPathFolder)
{
	static basic_string<TCHAR> buf;

	buf = pDataFolder;
	buf.append(pRelPathFolder);
	if (PathIsDirectory(buf.c_str())) {  // POP �̏ꍇ
		buf += _T("Folder.idx");
	} else if (PathFileExists(buf.c_str())) {  // IMAP �̏ꍇ
		size_t nPosSuffix = buf.find_last_of(_T('.'));
		basic_string<TCHAR> strSuffix = buf.substr(nPosSuffix);
		if (strSuffix.compare(_T(".ini")) == 0) {  // [MEMO] �Ȃ��� ini �ւ̃p�X���Ԃ邱�Ƃ�����BBecky! �̃o�O���낤���B
			buf.replace(nPosSuffix, strSuffix.length(), _T(".idx"));
		}
	}
	return buf.c_str();
}

/**
 * ���[���{�b�N�X�ݒ�t�@�C���iFolder.ini�j�ւ̃p�X���擾
 */
LPCTSTR bkGetFolderIniFilePath(LPCTSTR pDataFolder, LPCTSTR pRelPathFolder)
{
	static basic_string<TCHAR> buf;

	buf = pDataFolder;
	buf += pRelPathFolder;
	if (PathIsDirectory(buf.c_str())) {  // POP �̏ꍇ
		buf += _T("Folder.ini");
	} else if (PathFileExists(buf.c_str())) {  // IMAP �̏ꍇ
		size_t nPosSuffix = buf.find_last_of(_T('.'));
		basic_string<TCHAR> strSuffix = buf.substr(nPosSuffix);
		if (strSuffix.compare(_T(".idx")) == 0) {
			buf.replace(nPosSuffix, strSuffix.length(), _T(".ini"));
		}
	}
	return buf.c_str();
}

/**
 * ���[���w�b�_�̉��s���������� [RFC2822 ����] http://www.ietf.org/rfc/rfc0822.txt
 * 
 * > Unfolding is accomplished by regarding CRLF immediately followed by a 
 * > LWSP-char as equivalent to the LWSP-char.
 *
 * > SPACE       =  <ASCII SP, space>            ; (     40,      32.)
 * > HTAB        =  <ASCII HT, horizontal-tab>   ; (     11,       9.)
 * > CRLF        =  CR LF
 * > LWSP-char   =  SPACE / HTAB                 ; semantics = SPACE
 */
void bkUnfoldingHeader(std::string &strHeader)
{
	for (std::string::iterator i = strHeader.begin(); i != strHeader.end(); i++) {
		if (!(*i == '\r' && *(i + 1) == '\n' && iswspace(*(i + 2))))
			continue;
		strHeader.erase(i, i + 2);
	}
}



/**
 * ���[���w�b�_��MIME���f�R�[�h���� [RFC2047����] http://www.ietf.org/rfc/rfc2047.txt
 * 
 * �ȉ��̌��t�̒�`�� RFC2047 �Q�Ƃ̂���
 *   encoded-word, charset, encoding, encoded-text
 */
void bkDecodeMIMEHeader(const std::string &strMIMEHeader, std::basic_string<WCHAR> &strDecoded)
{

#if 0
	std::string::const_iterator i = strMIMEHeader.begin();
	std::string::const_iterator beg = i;
	bool bEWcontinued = false;   // encoded-word ���A�����Ă��邩�ǂ���
	// [TODO] comment �̃f�R�[�h�ɑΉ�
	std::string strLWSP;
	while (true) {

		/* �f�R�[�h�Ώۂ� LWSP ��؂�Œ��o */
		if (i != strMIMEHeader.end() && ! iswspace(*i)) {
			i++;
			continue;
		}
		std::string strAtom(beg, i);
		if (i != strMIMEHeader.end()) {
			beg = i + 1;
			if (strAtom == _T("")) {
				strLWSP.append(1, *i++);
				continue;
			}
		}

		/* encoded-word �͕������������C�h��������i�[ */
		int cmp1 = strncmp(strAtom.c_str(), "=?", 2);
		int cmp2 = strncmp(strAtom.c_str() + strlen(strAtom.c_str()) - 2, "?=", 2);
		enum atom_type { encoded_word, token } at;
		if (strncmp(strAtom.c_str(), "=?", 2) == 0
		   && strncmp(strAtom.c_str() + strlen(strAtom.c_str()) - 2, "?=", 2) == 0)
			at = encoded_word;
		else
			at = token;
		switch (at) {
			case encoded_word: {

				/* �A������ encoded-word �̊Ԃ� LWSP �͖������� */
				if (! bEWcontinued) {
					Charset::Encoding encACP = Charset::Encoding::ACP;
					strDecoded.append(encACP.getWString(encACP.getBytes(strLWSP)));
					strLWSP = "";
				}
				strLWSP.append(1, *i);

				/*  �\���v�f���擾�i0. charset�A1. encoding�A2. encoded-text�j */
				std::istringstream ss(strAtom);
				ss.seekg(2, std::ios::cur);
				std::string strMIMEItems[3];
				for (int i = 0; i < 3; i++) {
					if (! getline(ss, strMIMEItems[i], '?')) {  // too few MIME items.
						// [TODO] error ����
						break;
					}
				}
				char c1 = ss.peek();
				ss.seekg(1, std::ios::cur);
				char c2 = ss.peek();
				ss.seekg(1, std::ios::cur);
				if (! ss.eof()) {  // too many MIME items
					break;
				}

				/* �\���v�f�����������`�F�b�N���� */
				if (strMIMEItems[0].length() <= 0) {
					// [TODO] error ����
					break;
				}
				Charset::Encoding encMIME(strMIMEItems[0]);  // �s���Ȃ� EncodingException �����s�����
				enum encode_method { em_base64, em_qprintable } em;
				if (strMIMEItems[1].length() != 1) {
					// [TODO] error ����
					break;
				}
				switch (tolower(strMIMEItems[1].at(0))) {
				case 'b':  // BASE64  [RFC2045]
					em = em_base64;
					break;
				case 'q':  // Quoted-Printable
					em = em_qprintable;
					break;
				}

				/* �e�L�X�g���f�R�[�h���ă��C�h������Ƃ��ċL�� */
				std::vector<byte> decoded;
				if (em == em_base64) {
					decoded = Binary::Base64::Decode(strMIMEItems[2]);
				} else if (em == em_qprintable) {
					// [TODO] qprintable �������f�R�[�h
				}
				strDecoded.append(encMIME.getWString(decoded));
				bEWcontinued = true;
				break;
			}

			case token: {

				/* �e�L�X�g�����C�h������Ƃ��ċL�� */
				Charset::Encoding encACP = Charset::Encoding::ACP;
				strDecoded.append(encACP.getWString(encACP.getBytes(strLWSP)));
				strDecoded.append(encACP.getWString(encACP.getBytes(strAtom)));
				strLWSP = "";
				strLWSP.append(1, *i);
				bEWcontinued = false;
			}
		}

		/* �f�R�[�h�Ώۂ��Ȃ��Ȃ�����o�b�t�@���i�[���ă��[�v�I�� */
		if (i == strMIMEHeader.end())
			break;
		i++;
	}
#endif
}

/**
 * �w�肵�����[��ID����C�ӂ̃w�b�_���擾����i�w�b�_���͕Ԃ��Ȃ��j
 */
void bkGetSpecifiedHeader(LPTSTR szResult, size_t maxSize, LPCTSTR szMailID, LPCTSTR szHeaderName, LPCTSTR szElementName)
{
	//char *szAllHeader = bka.GetHeader(szMailID);
	LPSTR szAllSource = bka.GetSource(szMailID);
	CMIMEItem item;
	item.FromString(szAllSource);
	CHAR buf[256];
	item.GetHeader(szHeaderName, buf, sizeof(buf));
	std::string strHeader = buf;
	bkUnfoldingHeader(strHeader);
	std::basic_string<WCHAR> strDecoded;
	bkDecodeMIMEHeader(strHeader, strDecoded);

	//MessageBox(NULL, szAllSource, _T("szAllSource"), MB_OK);
	MessageBox(NULL, szHeaderName, _T("szHeaderName"), MB_OK);
	MessageBox(NULL, strHeader.c_str(), _T("strHeader"), MB_OK);

	Charset::Encoding encACP = Charset::Encoding::ACP;
	std::string strBuf = encACP.getString(encACP.getWBytes(strDecoded));
	_sntprintf_s(szResult, maxSize, _TRUNCATE, _T("%s"), strBuf.c_str());
	MessageBox(NULL, strBuf.c_str(), _T("strDecoded"), MB_OK);
	bka.Free(szAllSource);
}

/**
 * ������ŕ\�����ꂽ���t����͂���
 */
void parseDateStr(LPCTSTR szField, LPCTSTR szDateFormat, struct tm *tmDate, long int& tm_gmtoff)
{
	DateTime::DTFORMAT format = DateTime::DTF_RFC2822;

	/* ���t�t�H�[�}�b�g�𒲂ׂ� */
	if (strnicmp(szDateFormat, "rfc2822", 7) == 0) {
		format = DateTime::DTF_RFC2822;
	}

	/* ���t�t�H�[�}�b�g�ɏ]���ē��t���������͂��� */
	DateTime dt;
	memset(tmDate, 0, sizeof(struct tm));
	tm_gmtoff = 0;
	switch (format) {
	case DateTime::DTF_RFC2822:
		dt.Parse(szField, DateTime::DTF_RFC2822, tmDate, tm_gmtoff);
		break;
	}
}

/**
 * �w�肵��ID�̃��[���ɑ΂���Becky!�}�N���V���{�����܂ޕ�������i�[����
 */
int bksprintf(LPSTR szOutput, size_t maxSize, LPCTSTR szFormat, LPCTSTR szbkMailID)
{
	Mail mail;
	LPSTR szAllSource = bka.GetSource(szbkMailID);
	mail.setMessage(szAllSource);
	bka.Free(szAllSource);


	//MailIndex *midx = bkCreateMailIndexByFile(szbkMailID);
	MailIndex *midx = bkCreateMailIndexByHeader(szbkMailID);
	if (midx == NULL) {
		midx = bkCreateMailIndexByHeader(szbkMailID);
	}
	return bksprintf(szOutput, maxSize, szFormat, midx);
}

/**
 * �w�肵��ID�̃��[���ɑ΂���Becky!�}�N���V���{�����܂ޕ�������i�[����
 */
int bksprintf(LPTSTR szOutput, size_t maxSize, LPCTSTR szFormat, const MailIndex *midx)
{
#define ARRLEN(arr) (sizeof(arr) / sizeof(arr[0]))
	if (szFormat == NULL) {
		Logger::getInstance().print(_T("bksprintf() failed. NULL is passed for format."));
		return 0;
	}

	size_t nLen = 0;                // �t�H�[�}�b�g��̕�����
	const TCHAR *p;                 // �t�H�[�}�b�g������ւ̃|�C���^
	szOutput[0] = _T('\0');
	p = szFormat;
	while (nLen < maxSize && *p) {
		switch (*p) {
			case _T('%'): {
				if (*++p == _T('\0')) break;
				TCHAR symbol = *p++;                // �}�N���V���{��
				std::basic_string<TCHAR> strField;  // �}�N���V���{����W�J����������
				switch (symbol) {
					case _T('f'):  // %f: �Q�ƃ��[����From�t�B�[���h
						strField = midx->strFrom.c_str();
						break;
					case _T('t'):  // %t: �Q�ƃ��[����To�t�B�[���h
						strField = midx->strTo.c_str();
						break;
					case _T('d'):  // %d: �Q�ƃ��[����Date�t�B�[���h
						strField = midx->getFormatedTime(_T("%Y/%m/%d %H:%M"));
						break;
					case _T('D'):  // %D{fmt}: �Q�ƃ��[����Date�t�B�[���h���A���[�J�����Ԃɕϊ����A�t�H�[�}�b�g����������
					case _T('R'):  // %R{fmt}: �Q�ƃ��[���̔z�M�������A���[�J�����ԂŃt�H�[�}�b�g����������
					case _T('W'):  // %W{fmt}: �Q�ƃ��[���̎�M�������A���[�J�����ԂŃt�H�[�}�b�g����������
					case _T('T'): { // %T{fmt}: ���ݓ��t���t�H�[�}�b�g����������
						struct tm tmDate = {{0}};
						const TCHAR *pFmtBeg = p;
						const TCHAR *pFmtEnd = p;
						TCHAR szTimeFormat[256];

						/* �����t�H�[�}�b�g{fmt}��؂�o�� */
						if (*p != _T('{'))  // �J�n�����͕K�{�B�Ȃ���Εs���Ƃ��Ĉ���
							continue;
						pFmtBeg = ++p;
						while (*p != _T('\0') && *p != _T('}'))
							p++;
						if (*p == _T('\0'))  // �I�[�����͕K�{�B�Ȃ���Εs���Ƃ��Ĉ���
							continue;
						pFmtEnd = ++p;
						memset(szTimeFormat, 0, sizeof(szTimeFormat));
						memcpy(szTimeFormat, pFmtBeg, (pFmtEnd - 1) - pFmtBeg);

						/* �����t�H�[�}�b�g�ɏ]�������t�t�H�[�}�b�g���쐬 */
						switch (symbol) {
						case _T('D'):
							// [TODO] Date�t�B�[���h���w�肵���t�H�[�}�b�g�� strField �ɕۑ�����
							strField = midx->getFormatedTime(szTimeFormat, MailIndex::DATETYPE_SEND);
							break;
						case _T('R'):
							strField = midx->getFormatedTime(szTimeFormat, MailIndex::DATETYPE_RECV);
							break;
						case _T('W'):
							strField = midx->getFormatedTime(szTimeFormat, MailIndex::DATETYPE_DNLD);
							break;
						case _T('T'): {
							TCHAR szField[1024];
							time_t now;
							time(&now);
							localtime_s(&tmDate, &now);
							_tcsftime(szField, sizeof(szField), szTimeFormat, &tmDate);
							strField = szField;
							break;
									  }
						}
						break;
					}
					case _T('i'):  // %i: �Q�ƃ��[����Message-ID�t�B�[���h
						strField = midx->strMsgId;
						break;
					case _T('I'):  // %I: �Q�ƃ��[����Message-ID�t�B�[���h���炻����͂� "<>" ������������B
						strField = midx->getMessageID(MailIndex::MSGIDFMT_NOLTFT);
						break;
					case _T('s'):  // %s: �Q�ƃ��[����Subject�t�B�[���h
						strField = midx->getACPSubject();
						break;
					case _T('S'):  // %S: �Q�ƃ��[����Subject�t�B�[���h����A"Re:"��"Fw:"�Ȃǂ̕���������������
						strField = midx->getSubject(MailIndex::SBJFMT_NOREFW, Charset::Encoding::ACP);
						break;
					case _T('n'):  // %n: �Q�ƃ��[����From�t�B�[���h����{���̕����𔲂��o��������
						strField = midx->getFrom(MailIndex::FROMFMT_NAME);
						break;
				}
				nLen += _sntprintf_s(szOutput + nLen, maxSize - nLen, _TRUNCATE, _T("%s"), strField.c_str());
				break;
			}
			case _T('\r'):
			case _T('\n'):
				// \r, \n �͖�������
				p++;
				break;
			default:
				szOutput[nLen] = *p;
				szOutput[nLen+1] = '\0';
				nLen++;
				p++;
				break;
		}
	}
	return nLen;
}

/**
 * ���[���C���f�b�N�X����������
 */
size_t searchMailIndex(ifstream& ifs, LPCTSTR pMsgID, size_t beg)
{
	size_t nFileSize = ifs.seekg(0,  std::ios::end).tellg();
	size_t nBeg = 0, nEnd = nFileSize - 1, nMid = (nBeg + nEnd) / 2;
	size_t nBegLineFeed = 0, nMidLineHead = 0, nMidLineFeed = 0, nEndLineHead = 0;
	size_t nMsgID = 0, nBegMsgID = 0, nEndMsgID = 0, nMidMsgID = 0;

	basic_istringstream<TCHAR>(pMsgID) >> hex >> nMsgID;
	while (nBeg < nEnd) {
		basic_string<TCHAR> strMsgID;

		/* �ŏ��l���擾 */
		ifs.seekg(nBeg,  std::ios::beg);
		ifs.ignore(nFileSize, _T('\x01'));
		getline(ifs, strMsgID, _T('\x01'));
		basic_istringstream<TCHAR>(strMsgID) >> hex >> nBegMsgID;
		if (nMsgID == nBegMsgID)
			return nBeg;
		nBegLineFeed = ifs.ignore(nFileSize, _T('\n')).seekg(-1, std::ios::cur).tellg();

		/* �ő�l���擾 */
		ifs.seekg(nEnd, std::ios::beg);
		if (ifs.peek() == _T('\n'))
			ifs.seekg(-1, std::ios::cur);
		while (ifs.tellg() > 0 && ifs.peek() != _T('\n'))
			ifs.seekg(-1, std::ios::cur);  // ���s������ 2bytes �߂�͗l
		nEndLineHead = ifs.ignore(nFileSize, _T('\n')).tellg();
		ifs.ignore(nFileSize, '\x01');
		getline(ifs, strMsgID, '\x01');
		basic_istringstream<TCHAR>(strMsgID) >> hex >> nEndMsgID;
		if (nMsgID == nEndMsgID)
			return nEndLineHead;

		/* ���Ԓl�i���悻�j���擾 */
		ifs.seekg(nMid, std::ios::beg);
		if (ifs.peek() == _T('\n'))
			ifs.seekg(-1, std::ios::cur);
		while (ifs.tellg() > 0 && ifs.peek() != _T('\n'))
			ifs.seekg(-1, std::ios::cur);  // ���s������ 2bytes �߂�͗l
		nMidLineHead = ifs.ignore(nFileSize, _T('\n')).tellg();
		ifs.ignore(nFileSize, _T('\x01'));
		getline(ifs, strMsgID, _T('\x01'));
		basic_istringstream<TCHAR>(strMsgID) >> hex >> nMidMsgID;
		if (nMsgID == nMidMsgID)
			return nMidLineHead;
		nMidLineFeed = ifs.ignore(nFileSize, _T('\n')).seekg(-1, std::ios::cur).tellg();

		/* �����Ώۂ̑召�֌W���猟���͈͂��i�� */
		if (nMsgID < nMidMsgID) {
			nBeg = nBegLineFeed + sizeof(_T('\n'));
			nEnd = nMidLineHead - sizeof(_T('\n'));
			nMid = (nBeg + nEnd) / 2;
		} else {
			nBeg = nMidLineFeed + sizeof(_T('\n'));
			nEnd = nEndLineHead - sizeof(_T('\n'));
			nMid = (nBeg + nEnd) / 2;
		}

		basic_stringstream<TCHAR> pBuf;
		pBuf << _T("nMsgID: ") << nMsgID << _T(", nBegMsgID: ") << nBegMsgID << _T(", nEndMsgID: ") << nEndMsgID << _T(", nMidMsgID: ") << nMidMsgID;
		Logger::getInstance().print(_T("=== CURRENT LOOP ==="));
		Logger::getInstance().print(pBuf.str().c_str());
		Logger::getInstance().print(_T("================="));
		pBuf.str(_T(""));
		pBuf << _T("nBeg: ") << nBeg << _T(", nEnd: ") << nEnd << _T(", nMid: ") << nMid;
		Logger::getInstance().print(_T("=== NEXT LOOP ==="));
		Logger::getInstance().print(pBuf.str().c_str());
		Logger::getInstance().print(_T("================="));
	}
	return -1;
}

/**
 * �w�肵��ID�̃��[���C���f�b�N�X���擾
 */
MailIndex *bkCreateMailIndexByFile(LPCTSTR szbkMailID)
{
	/* ���[���C���f�b�N�X�t�@�C���̖��O���擾 */
	basic_string<TCHAR> strRelPathFolder;
	basic_string<TCHAR> strMsgID;
	basic_istringstream<TCHAR> p(szbkMailID);
	getline(p, strRelPathFolder, _T('?'));
	getline(p, strMsgID);
	basic_string<TCHAR> strAbsPathIndexFile = bkGetFolderIndexFilePath(bka.GetDataFolder(), strRelPathFolder.c_str());

	/* ���[���C���f�b�N�X���擾 */
	ifstream ifs(strAbsPathIndexFile.c_str(), ifstream::in);
	if (! ifs) {  // �ǂݍ��ݎ��s
		Logger::getInstance().print(basic_string<TCHAR>(_T("can't open index file. file name: ")) + strAbsPathIndexFile);
		return NULL;
	}
	size_t nPosIndexLine = searchMailIndex(ifs, strMsgID.c_str(), 0);
	if (nPosIndexLine == -1) {  // not found
		Logger::getInstance().print(basic_string<TCHAR>(_T("can't found mail. ID: ") + strMsgID));
		return NULL;
	}
	ifs.seekg(nPosIndexLine, std::ios::beg);
	basic_string<TCHAR> strMailIndex;
	getline(ifs, strMailIndex);
	return new MailIndex(szbkMailID, strMailIndex.c_str());
}

/**
 * �w�肵��ID�̃��[���C���f�b�N�X���擾
 */
MailIndex *bkCreateMailIndexByHeader(LPCTSTR szbkMailID)
{
	LPCTSTR szMailSource = bka.GetSource(szbkMailID);
	Mail mail(szMailSource);
	MailIndex *midx = mail.createIndex();
	bka.Free((LPVOID) szMailSource);
	return midx;

#if 0
#define GETHEADERSTR(member, header) {                              \
	TCHAR szBuf[1024];                                              \
	bkGetSpecifiedHeader(szBuf, sizeof(szBuf), szbkMailID, header); \
	member = szBuf;                                                 \
};
#define GETHEADERTIME(member, header) {                                  \
	TCHAR szBuf[1024];                                                   \
	time_t buftime;                                                      \
	long curTimezone;                                                    \
	struct tm tmDate = {{0}};                                            \
	long int tm_gmtoff = 0;                                              \
	bkGetSpecifiedHeader(szBuf, sizeof(szBuf), szbkMailID, header);      \
	parseDateStr(szBuf, "rfc2822", &tmDate, tm_gmtoff);                  \
	_tzset();                                                            \
	_get_timezone(&curTimezone);                                         \
	tmDate.tm_hour += -(int)(tm_gmtoff / 60) -(int)(curTimezone / 3600); \
	tmDate.tm_min += -(tm_gmtoff % 60) -((int)(curTimezone / 60) % 60);  \
	buftime = mktime(&tmDate);                                           \
	localtime_s(&tmDate, &buftime);                                      \
	member = buftime;                                                    \
};

	MailIndex *midx = new MailIndex;

	TCHAR buf[64];
	memset(buf, 0, sizeof(buf));
	bka.GetCharSet(szbkMailID, buf, sizeof(buf) / sizeof(buf[0]));
	//Charset::Encoding encACP = Charset::Encoding::ACP;
	//midx->strSubject = encACP.getString(encACP.getWBytes(encACP.getWString(encACP.getBytes(midx->strSubject)))).c_str();
	//MessageBox(NULL, buf, "GetCharSet()", MB_OK);
	midx->strCharSet = buf;

	Charset::Encoding enc = Charset::Encoding(midx->strCharSet);
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, ".%d", enc.getCPInfoEx().CodePage);
	MessageBox(NULL, buf, "GetCharSet()", MB_OK);
	setlocale(LC_CTYPE, buf);
	midx->strBkMailID = szbkMailID;
	GETHEADERSTR(midx->strSubject,    "Subject");
	GETHEADERSTR(midx->strTo,         "To");
	GETHEADERSTR(midx->strFrom,       "From");
	GETHEADERSTR(midx->strReferences, "References");
	GETHEADERSTR(midx->strMsgId,      "Message-ID");
	GETHEADERTIME(midx->tSend,        "Date");
	GETHEADERTIME(midx->tRecv,        "Received");
	
	char b[1024];
	sprintf(b, "subject = %s, charset = %s", midx->strSubject.c_str(), midx->strCharSet.c_str());
	MessageBox(NULL, b, "title", MB_OK);
	return midx;
#endif
}

/**
 * ���[���ꗗ�f�[�^(Folder.idx)��ǂݍ���
 */
void ParseFolderIdx(LPCSTR pFolder, SVPCALLBACK callback)
{
	/* ���[���C���f�b�N�X�t�@�C���̖��O���擾 */
	basic_string<TCHAR> strAbsPathIndexFile = bkGetFolderIndexFilePath(bka.GetDataFolder(), pFolder);
	ifstream ifs(strAbsPathIndexFile.c_str(), ifstream::in);
	if (! ifs) {  // �ǂݍ��ݎ��s
		Logger::getInstance().print(basic_string<TCHAR>(_T("can't open index file: ")) + strAbsPathIndexFile);
		return;
	}

	basic_string<TCHAR> strLine;
	for (int line = 0; getline(ifs, strLine); line++) {

		/* Becky! �o�[�W�������L�����s�͖��� */
		if (line == 0) {
			continue;
		}

		/* �e�v�f���p�[�X���ăR�[���o�b�N�֐����Ăяo�� */
		istringstream p(strLine);
		string elem;
		for (int col = 0; getline(p, elem, '\x01'); col++) {
			BOOL iscontinue = callback(line, col, elem);
			if (!iscontinue)
				break;
		}
	}
	return;
}
