#include "BkExtend.h"
#include "BkCommon.h"
#include "BkListViewExtension.h"
#include "DateTime.h"
#include "Mail.h"

#pragma comment(lib, "shlwapi.lib")

using namespace std;

/**
 * メールが選択されているか
 */
BOOL bkIsMailSelected()
{
	TCHAR szMailID[256];
	int nMailPos = 0;
	nMailPos = bka.GetNextMail(-1, szMailID, sizeof(szMailID), TRUE);
	return (nMailPos != -1);
}

/**
 * スレッドモードの有効であるか
 * #\xxxxx.mb#tatsurou@mail.sfc.wide.ad.jp#INBOX[1f].ini にて Status Thread=1 なら有効
 */
BOOL bkIsThreadMode()
{
	basic_string<TCHAR> strIni = bkGetFolderIniFilePath(bka.GetDataFolder(), bka.GetCurrentFolder());
	int isThreadMode = GetPrivateProfileInt(_T("Status"), _T("Thread"), 0, strIni.c_str());
	return (isThreadMode != 0);
}

/**
 * メールインデックスファイル（Folder.idx）へのパスを取得
 */
LPCTSTR bkGetFolderIndexFilePath(LPCTSTR pDataFolder, LPCTSTR pRelPathFolder)
{
	static basic_string<TCHAR> buf;

	buf = pDataFolder;
	buf.append(pRelPathFolder);
	if (PathIsDirectory(buf.c_str())) {  // POP の場合
		buf += _T("Folder.idx");
	} else if (PathFileExists(buf.c_str())) {  // IMAP の場合
		size_t nPosSuffix = buf.find_last_of(_T('.'));
		basic_string<TCHAR> strSuffix = buf.substr(nPosSuffix);
		if (strSuffix.compare(_T(".ini")) == 0) {  // [MEMO] なぜか ini へのパスが返ることがある。Becky! のバグだろうか。
			buf.replace(nPosSuffix, strSuffix.length(), _T(".idx"));
		}
	}
	return buf.c_str();
}

/**
 * メールボックス設定ファイル（Folder.ini）へのパスを取得
 */
LPCTSTR bkGetFolderIniFilePath(LPCTSTR pDataFolder, LPCTSTR pRelPathFolder)
{
	static basic_string<TCHAR> buf;

	buf = pDataFolder;
	buf += pRelPathFolder;
	if (PathIsDirectory(buf.c_str())) {  // POP の場合
		buf += _T("Folder.ini");
	} else if (PathFileExists(buf.c_str())) {  // IMAP の場合
		size_t nPosSuffix = buf.find_last_of(_T('.'));
		basic_string<TCHAR> strSuffix = buf.substr(nPosSuffix);
		if (strSuffix.compare(_T(".idx")) == 0) {
			buf.replace(nPosSuffix, strSuffix.length(), _T(".ini"));
		}
	}
	return buf.c_str();
}

/**
 * メールヘッダの改行を除去する [RFC2822 準拠] http://www.ietf.org/rfc/rfc0822.txt
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
 * メールヘッダのMIMEをデコードする [RFC2047準拠] http://www.ietf.org/rfc/rfc2047.txt
 * 
 * 以下の言葉の定義は RFC2047 参照のこと
 *   encoded-word, charset, encoding, encoded-text
 */
void bkDecodeMIMEHeader(const std::string &strMIMEHeader, std::basic_string<WCHAR> &strDecoded)
{

#if 0
	std::string::const_iterator i = strMIMEHeader.begin();
	std::string::const_iterator beg = i;
	bool bEWcontinued = false;   // encoded-word が連続しているかどうか
	// [TODO] comment のデコードに対応
	std::string strLWSP;
	while (true) {

		/* デコード対象を LWSP 区切りで抽出 */
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

		/* encoded-word は複合化したワイド文字列を格納 */
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

				/* 連続した encoded-word の間の LWSP は無視する */
				if (! bEWcontinued) {
					Charset::Encoding encACP = Charset::Encoding::ACP;
					strDecoded.append(encACP.getWString(encACP.getBytes(strLWSP)));
					strLWSP = "";
				}
				strLWSP.append(1, *i);

				/*  構成要素を取得（0. charset、1. encoding、2. encoded-text） */
				std::istringstream ss(strAtom);
				ss.seekg(2, std::ios::cur);
				std::string strMIMEItems[3];
				for (int i = 0; i < 3; i++) {
					if (! getline(ss, strMIMEItems[i], '?')) {  // too few MIME items.
						// [TODO] error 処理
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

				/* 構成要素が正しいかチェックする */
				if (strMIMEItems[0].length() <= 0) {
					// [TODO] error 処理
					break;
				}
				Charset::Encoding encMIME(strMIMEItems[0]);  // 不正なら EncodingException が発行される
				enum encode_method { em_base64, em_qprintable } em;
				if (strMIMEItems[1].length() != 1) {
					// [TODO] error 処理
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

				/* テキストをデコードしてワイド文字列として記憶 */
				std::vector<byte> decoded;
				if (em == em_base64) {
					decoded = Binary::Base64::Decode(strMIMEItems[2]);
				} else if (em == em_qprintable) {
					// [TODO] qprintable 方式をデコード
				}
				strDecoded.append(encMIME.getWString(decoded));
				bEWcontinued = true;
				break;
			}

			case token: {

				/* テキストをワイド文字列として記憶 */
				Charset::Encoding encACP = Charset::Encoding::ACP;
				strDecoded.append(encACP.getWString(encACP.getBytes(strLWSP)));
				strDecoded.append(encACP.getWString(encACP.getBytes(strAtom)));
				strLWSP = "";
				strLWSP.append(1, *i);
				bEWcontinued = false;
			}
		}

		/* デコード対象がなくなったらバッファを格納してループ終了 */
		if (i == strMIMEHeader.end())
			break;
		i++;
	}
#endif
}

/**
 * 指定したメールIDから任意のヘッダを取得する（ヘッダ名は返さない）
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
 * 文字列で表現された日付を解析する
 */
void parseDateStr(LPCTSTR szField, LPCTSTR szDateFormat, struct tm *tmDate, long int& tm_gmtoff)
{
	DateTime::DTFORMAT format = DateTime::DTF_RFC2822;

	/* 日付フォーマットを調べる */
	if (strnicmp(szDateFormat, "rfc2822", 7) == 0) {
		format = DateTime::DTF_RFC2822;
	}

	/* 日付フォーマットに従って日付文字列を解析する */
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
 * 指定したIDのメールに対してBecky!マクロシンボルを含む文字列を格納する
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
 * 指定したIDのメールに対してBecky!マクロシンボルを含む文字列を格納する
 */
int bksprintf(LPTSTR szOutput, size_t maxSize, LPCTSTR szFormat, const MailIndex *midx)
{
#define ARRLEN(arr) (sizeof(arr) / sizeof(arr[0]))
	if (szFormat == NULL) {
		Logger::getInstance().print(_T("bksprintf() failed. NULL is passed for format."));
		return 0;
	}

	size_t nLen = 0;                // フォーマット後の文字列長
	const TCHAR *p;                 // フォーマット文字列へのポインタ
	szOutput[0] = _T('\0');
	p = szFormat;
	while (nLen < maxSize && *p) {
		switch (*p) {
			case _T('%'): {
				if (*++p == _T('\0')) break;
				TCHAR symbol = *p++;                // マクロシンボル
				std::basic_string<TCHAR> strField;  // マクロシンボルを展開した文字列
				switch (symbol) {
					case _T('f'):  // %f: 参照メールのFromフィールド
						strField = midx->strFrom.c_str();
						break;
					case _T('t'):  // %t: 参照メールのToフィールド
						strField = midx->strTo.c_str();
						break;
					case _T('d'):  // %d: 参照メールのDateフィールド
						strField = midx->getFormatedTime(_T("%Y/%m/%d %H:%M"));
						break;
					case _T('D'):  // %D{fmt}: 参照メールのDateフィールドを、ローカル時間に変換し、フォーマットした文字列
					case _T('R'):  // %R{fmt}: 参照メールの配信日時を、ローカル時間でフォーマットした文字列
					case _T('W'):  // %W{fmt}: 参照メールの受信日時を、ローカル時間でフォーマットした文字列
					case _T('T'): { // %T{fmt}: 現在日付をフォーマットした文字列
						struct tm tmDate = {{0}};
						const TCHAR *pFmtBeg = p;
						const TCHAR *pFmtEnd = p;
						TCHAR szTimeFormat[256];

						/* 時刻フォーマット{fmt}を切り出す */
						if (*p != _T('{'))  // 開始文字は必須。なければ不正として扱う
							continue;
						pFmtBeg = ++p;
						while (*p != _T('\0') && *p != _T('}'))
							p++;
						if (*p == _T('\0'))  // 終端文字は必須。なければ不正として扱う
							continue;
						pFmtEnd = ++p;
						memset(szTimeFormat, 0, sizeof(szTimeFormat));
						memcpy(szTimeFormat, pFmtBeg, (pFmtEnd - 1) - pFmtBeg);

						/* 時刻フォーマットに従った日付フォーマットを作成 */
						switch (symbol) {
						case _T('D'):
							// [TODO] Dateフィールドを指定したフォーマットで strField に保存する
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
					case _T('i'):  // %i: 参照メールのMessage-IDフィールド
						strField = midx->strMsgId;
						break;
					case _T('I'):  // %I: 参照メールのMessage-IDフィールドからそれを囲む "<>" を取った部分。
						strField = midx->getMessageID(MailIndex::MSGIDFMT_NOLTFT);
						break;
					case _T('s'):  // %s: 参照メールのSubjectフィールド
						strField = midx->getACPSubject();
						break;
					case _T('S'):  // %S: 参照メールのSubjectフィールドから、"Re:"や"Fw:"などの部分を除いたもの
						strField = midx->getSubject(MailIndex::SBJFMT_NOREFW, Charset::Encoding::ACP);
						break;
					case _T('n'):  // %n: 参照メールのFromフィールドから本名の部分を抜き出したもの
						strField = midx->getFrom(MailIndex::FROMFMT_NAME);
						break;
				}
				nLen += _sntprintf_s(szOutput + nLen, maxSize - nLen, _TRUNCATE, _T("%s"), strField.c_str());
				break;
			}
			case _T('\r'):
			case _T('\n'):
				// \r, \n は無視する
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
 * メールインデックスを検索する
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

		/* 最小値を取得 */
		ifs.seekg(nBeg,  std::ios::beg);
		ifs.ignore(nFileSize, _T('\x01'));
		getline(ifs, strMsgID, _T('\x01'));
		basic_istringstream<TCHAR>(strMsgID) >> hex >> nBegMsgID;
		if (nMsgID == nBegMsgID)
			return nBeg;
		nBegLineFeed = ifs.ignore(nFileSize, _T('\n')).seekg(-1, std::ios::cur).tellg();

		/* 最大値を取得 */
		ifs.seekg(nEnd, std::ios::beg);
		if (ifs.peek() == _T('\n'))
			ifs.seekg(-1, std::ios::cur);
		while (ifs.tellg() > 0 && ifs.peek() != _T('\n'))
			ifs.seekg(-1, std::ios::cur);  // 改行文字は 2bytes 戻る模様
		nEndLineHead = ifs.ignore(nFileSize, _T('\n')).tellg();
		ifs.ignore(nFileSize, '\x01');
		getline(ifs, strMsgID, '\x01');
		basic_istringstream<TCHAR>(strMsgID) >> hex >> nEndMsgID;
		if (nMsgID == nEndMsgID)
			return nEndLineHead;

		/* 中間値（およそ）を取得 */
		ifs.seekg(nMid, std::ios::beg);
		if (ifs.peek() == _T('\n'))
			ifs.seekg(-1, std::ios::cur);
		while (ifs.tellg() > 0 && ifs.peek() != _T('\n'))
			ifs.seekg(-1, std::ios::cur);  // 改行文字は 2bytes 戻る模様
		nMidLineHead = ifs.ignore(nFileSize, _T('\n')).tellg();
		ifs.ignore(nFileSize, _T('\x01'));
		getline(ifs, strMsgID, _T('\x01'));
		basic_istringstream<TCHAR>(strMsgID) >> hex >> nMidMsgID;
		if (nMsgID == nMidMsgID)
			return nMidLineHead;
		nMidLineFeed = ifs.ignore(nFileSize, _T('\n')).seekg(-1, std::ios::cur).tellg();

		/* 検索対象の大小関係から検索範囲を絞る */
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
 * 指定したIDのメールインデックスを取得
 */
MailIndex *bkCreateMailIndexByFile(LPCTSTR szbkMailID)
{
	/* メールインデックスファイルの名前を取得 */
	basic_string<TCHAR> strRelPathFolder;
	basic_string<TCHAR> strMsgID;
	basic_istringstream<TCHAR> p(szbkMailID);
	getline(p, strRelPathFolder, _T('?'));
	getline(p, strMsgID);
	basic_string<TCHAR> strAbsPathIndexFile = bkGetFolderIndexFilePath(bka.GetDataFolder(), strRelPathFolder.c_str());

	/* メールインデックスを取得 */
	ifstream ifs(strAbsPathIndexFile.c_str(), ifstream::in);
	if (! ifs) {  // 読み込み失敗
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
 * 指定したIDのメールインデックスを取得
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
 * メール一覧データ(Folder.idx)を読み込む
 */
void ParseFolderIdx(LPCSTR pFolder, SVPCALLBACK callback)
{
	/* メールインデックスファイルの名前を取得 */
	basic_string<TCHAR> strAbsPathIndexFile = bkGetFolderIndexFilePath(bka.GetDataFolder(), pFolder);
	ifstream ifs(strAbsPathIndexFile.c_str(), ifstream::in);
	if (! ifs) {  // 読み込み失敗
		Logger::getInstance().print(basic_string<TCHAR>(_T("can't open index file: ")) + strAbsPathIndexFile);
		return;
	}

	basic_string<TCHAR> strLine;
	for (int line = 0; getline(ifs, strLine); line++) {

		/* Becky! バージョンを記した行は無視 */
		if (line == 0) {
			continue;
		}

		/* 各要素をパースしてコールバック関数を呼び出す */
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
