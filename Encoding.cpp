#include "Encoding.h"

namespace Charset
{
	/**
	 * ==================================================
	 * InstalledCodePage class
	 * ==================================================
	 */

	/**
	 * コンストラクタ
	 */
	InstalledCodePage::InstalledCodePage(void)
	{
		EnumSystemCodePages(InstalledCodePage::EnumCodePagesProc, CP_INSTALLED);
	}

	/**
	 * デストラクタ
	 */
	InstalledCodePage::~InstalledCodePage(void)
	{
	}

	/**
	 * インスタンスを取得（シングルトンパターン）
	 */
	InstalledCodePage& InstalledCodePage::getInstance(void)
	{
		static InstalledCodePage cpInstalled;
		return cpInstalled;
	}

	/**
	 * コードページの名前と番号の対応表を取得
	 */
	const std::map<std::basic_string<TCHAR>, UINT> InstalledCodePage::getCodePageMap() const
	{
		return this->cpmap;
	}

	/**
	 * コードページの名前から番号を検索
	 */
	UINT InstalledCodePage::operator[](LPCTSTR lpCodePageName) const
	{
		if (this->cpmap.find(lpCodePageName) == this->cpmap.end())
			return 0;
		return this->cpmap.find(lpCodePageName)->second;
	}

	/**
	 * コードページの名前と番号を対応表に追加
	 */
	void InstalledCodePage::append(LPCTSTR lpCodePageName, UINT nCodePage)
	{
		this->cpmap.insert(ICP_CONTAINER(lpCodePageName, nCodePage));
	}

	/* static 関数群 */
	/**
	 * コードページ情報を処理するプロシージャ
	 */
	BOOL CALLBACK InstalledCodePage::EnumCodePagesProc(LPTSTR lpCodePageString)
	{
		UINT nCodePage = _ttoi(lpCodePageString);
		CPINFOEX cpiex;
		GetCPInfoEx(nCodePage, 0, &cpiex);
		InstalledCodePage::getInstance().append(cpiex.CodePageName, cpiex.CodePage);
		return TRUE;
	}

	/**
	 * ==================================================
	 * CodePageNameMap class
	 * ==================================================
	 */
	/**
	 * コンストラクタ
	 */
	CodePageNameMap::CodePageNameMap(void)
	{
	}

	/**
	 * デストラクタ
	 */
	CodePageNameMap::~CodePageNameMap(void)
	{
	}

	/**
	 * インスタンスを取得（シングルトンパターン）
	 */
	CodePageNameMap& CodePageNameMap::getInstance(void)
	{
		static CodePageNameMap cpInstalled;
		return cpInstalled;
	}

	/**
	 * コードページの名前から番号を検索
	 */
	UINT CodePageNameMap::operator[](LPCSTR lpCodePageName)
	{
		if (this->cpmapCache.find(lpCodePageName) != this->cpmapCache.end())
			return this->cpmapCache.find(lpCodePageName)->second;
		UINT nCodePage = this->searchCodePageFile(lpCodePageName);
		this->cpmapCache.insert(CPNM_TYPE(lpCodePageName, nCodePage));
		return nCodePage;
	}

	/**
	 * コードページの名前と番号を対応表を読み込み
	 */
	UINT CodePageNameMap::searchCodePageFile(LPCSTR lpKeyName)
	{
		/* 対応表のファイルが開けなかったらエラー表示して終了 */
		std::basic_string<TCHAR> strFName = _T("C:\\Program Files (x86)\\RimArts\\B2\\PlugIns\\CodePageName.bin");
		std::ifstream ifs(strFName.c_str(), std::ios::binary);
		if (! ifs) {
			std::cerr << _T("cannot open file to read: ") << strFName << std::endl;
			return 0;
		}

		/* 対応表を読み込む（クイックサーチアルゴリズムを採用） */
		size_t nFileSize = ifs.seekg(0,  std::ios::end).tellg();
		struct { 
			size_t pos; 
			MAPBLOCK block; 
		} beg = { 0, 0 }, 
			mid = { nFileSize / 2 - nFileSize / 2 % sizeof(MAPBLOCK) },
			end = { nFileSize - sizeof(MAPBLOCK) },
			*scope[] = { &beg, &mid, &end };
		while (beg.pos <= end.pos) {
			for (int i = 0; i < sizeof(scope) / sizeof(scope[0]); i++) {
				ifs.seekg(scope[i]->pos,  std::ios::beg);
				ifs.read((char *) &scope[i]->block, sizeof(scope[i]->block));
				int cmp = _stricmp(lpKeyName, scope[i]->block.szCodePageName);
				if (cmp == 0)
					return atol(scope[i]->block.szCodePage);
				else if ((i == 0 && cmp < 0) || (i == 2 && cmp > 0))  // out of range
					return 0;
			}
			if (_stricmp(lpKeyName, mid.block.szCodePageName) > 0) {
				beg.pos = mid.pos + sizeof(MAPBLOCK);
				end.pos = end.pos - sizeof(MAPBLOCK);
				mid.pos = (beg.pos + end.pos) / 2 - ((beg.pos + end.pos) / 2 % sizeof(MAPBLOCK));
			} else {
				beg.pos = beg.pos + sizeof(MAPBLOCK);
				end.pos = mid.pos - sizeof(MAPBLOCK);
				mid.pos = (beg.pos + end.pos) / 2 - ((beg.pos + end.pos) / 2 % sizeof(MAPBLOCK));
			}
		}
		return 0;
	}

	/**
	 * ==================================================
	 * EncodingException class
	 * ==================================================
	 */

	/**
	 * コンストラクタ
	 */
	EncodingException::EncodingException()
	{
		this->nErrorCode = GetLastError();
	}

	/**
	 * エンコーダー例外クラス
	 */
	Encoding& EncodingException::getACPEncoding()
	{
		static Charset::Encoding encACP((UINT) CP_ACP);
		return encACP;
	}

	/**
	 * エラーコードが示すメッセージを取得
	 */
	std::basic_string<TCHAR> EncodingException::getErrorMessage()
	{
		switch (this->nErrorCode) {
			case ERROR_INSUFFICIENT_BUFFER:
				return _T("ERROR_INSUFFICIENT_BUFFER");
			case ERROR_INVALID_FLAGS:
				return _T("ERROR_INVALID_FLAGS");
			case ERROR_INVALID_PARAMETER:
				return _T("ERROR_INVALID_PARAMETER");
			case ERROR_NO_UNICODE_TRANSLATION:
				return _T("ERROR_NO_UNICODE_TRANSLATION");
			default:
				return _T("NULL");
		}
	}

	/**
	 * ==================================================
	 * Encoding class
	 * ==================================================
	 */

	Encoding Encoding::ACP((UINT) CP_ACP);
	Encoding Encoding::UTF8((UINT) CP_UTF8);

	/**
	 * コンストラクタ
	 */
	Encoding::Encoding(UINT nCodePage)
	{
		initialize(nCodePage);
	}

	/**
	 * コンストラクタ
	 */
	Encoding::Encoding(std::basic_string<CHAR> strCharSetName)
	{
		initialize(strCharSetName.c_str());
	}

	/**
	 * コンストラクタ
	 */
	Encoding::Encoding(std::basic_string<WCHAR> strCharSetName)
	{
		initialize(strCharSetName.c_str());
	}

	/**
	 * コンストラクタ
	 */
	Encoding::Encoding(LPCSTR szCharSetName)
	{
		initialize(szCharSetName);
	}

	Encoding::Encoding(LPCWSTR szCharSetName)
	{
		initialize(szCharSetName);
	}

	/**
	 * デストラクタ
	 */
	Encoding::~Encoding(void)
	{
	}

	/**
	 * ==演算子のオーバーロード
	 */
	bool Encoding::operator==(Encoding& target)
	{
		return (this->cpiex.CodePage == target.cpiex.CodePage);
	}

	/**
	 * エンコード情報を取得
	 */
	const CPINFOEX Encoding::getCPInfoEx(void) const
	{
		return this->cpiex;
	}

	/**
	 * ワイド文字列をバイト列に変換（変換後のバイト列にはNULL文字を含まない）
	 */
	std::vector<byte> Encoding::getWBytes(const std::basic_string<WCHAR> &strFrom) const
	{
		return this->getWBytes(strFrom.c_str());
	}

	/**
	 * ワイド文字列をバイト列に変換（変換後のバイト列にはNULL文字を含まない）
	 */
	std::vector<byte> Encoding::getWBytes(LPCWSTR strFrom) const
	{
		int lenTo = WideCharToMultiByte(this->cpiex.CodePage, WC_COMPOSITECHECK, strFrom, -1, NULL, 0, NULL, NULL);
		if (lenTo == 0) {
			static Charset::EncodingException e;
			throw e;
		}
		LPSTR bufTo = new CHAR[lenTo];  // lenTo は NULL 文字を含む長さ
		memset(bufTo, 0, sizeof(CHAR) * lenTo);
		WideCharToMultiByte(this->cpiex.CodePage, WC_COMPOSITECHECK, strFrom, -1, bufTo, lenTo, NULL, NULL);
		std::vector<byte> ret(bufTo, bufTo + lenTo - 1);  // ret(addr1, addr2) は addr1 <= addr < addr2 の範囲でコピー
		delete[] (bufTo);
		return ret;
	}

	/**
	 * 文字列をバイト列に変換（変換後のバイト列にはNULL文字を含まない）
	 */
	std::vector<byte> Encoding::getBytes(const std::basic_string<CHAR> &strFrom) const
	{
		return this->getBytes(strFrom.c_str());
	}

	/**
	 * 文字列をバイト列に変換（変換後のバイト列にはNULL文字を含まない）
	 */
	std::vector<byte> Encoding::getBytes(LPCSTR strFrom) const
	{
		std::vector<byte> ret(strFrom, strFrom + lstrlen(strFrom));  // ret(addr1, addr2) addr1 <= addr < addr2 をコピー
		return ret;
	}

	/**
	 * バイト列からワイド文字列に変換（変換後の文字列にはNULL文字を含む）
	 */
	std::basic_string<WCHAR> Encoding::getWString(const std::vector<byte> &bytes) const
	{
		if (bytes.size() == 0) {
			std::basic_string<WCHAR> empty;
			return empty;
		}
		int lenTo = MultiByteToWideChar(this->cpiex.CodePage, 0, (LPCSTR) &bytes.front(), bytes.size(), NULL, 0);
		if (lenTo == 0) {
			static Charset::EncodingException e;
			throw e;
		}
		LPWSTR bufWide = new WCHAR[lenTo + 1];  // lenTo は NULL 文字を含まない長さ
		memset(bufWide, 0, sizeof(WCHAR) * (lenTo + 1));
		MultiByteToWideChar(this->cpiex.CodePage, 0, (LPCSTR) &bytes.front(), bytes.size(), bufWide, lenTo);
		std::basic_string<WCHAR> ret(bufWide, bufWide + lenTo);  // ret(addr1, addr2): addr1 <= addr < addr2 をコピー
		delete[] (bufWide);
		return ret;
	}

	/**
	 * バイト列からワイド文字列に変換（変換後の文字列にはNULL文字を含む）
	 */
	std::basic_string<char> Encoding::getString(const std::vector<byte> &bytes) const
	{
		std::basic_string<char> ret(bytes.begin(), bytes.end());
		return ret;
	}

	/* ===== private 関数群 ===== */
	void Encoding::initialize(UINT nCodePage)
	{
		GetCPInfoEx(nCodePage, 0, &this->cpiex);
	}

	void Encoding::initialize(LPCSTR szCharSetName)
	{
		initialize(CodePageNameMap::getInstance()[szCharSetName]);
	}

	void Encoding::initialize(LPCWSTR szCharSetName)
	{
		int lenTo = WideCharToMultiByte(3, WC_COMPOSITECHECK, szCharSetName, -1, NULL, 0, NULL, NULL);
		if (lenTo == 0) {
			static Charset::EncodingException e;
			throw e;
		}
		LPSTR bufTo = new CHAR[lenTo];  // lenTo は NULL 文字を含む長さ
		memset(bufTo, 0, sizeof(CHAR) * lenTo);
		WideCharToMultiByte(3, WC_COMPOSITECHECK, szCharSetName, -1, bufTo, lenTo, NULL, NULL);
		std::vector<byte> ret(bufTo, bufTo + lenTo - 1);  // ret(addr1, addr2) は addr1 <= addr < addr2 の範囲でコピー
		initialize(CodePageNameMap::getInstance()[bufTo]);
		delete[] (bufTo);
	}
}

namespace Binary
{
	/**
	 * エンコード
	 */
	std::vector<byte> Base64::Encode(const std::string strFrom)
	{
		std::vector<byte> vEncoded(strFrom.begin(), strFrom.end());
		return vEncoded;
	}

	/**
	 * エンコード
	 */
	std::vector<byte> Base64::Encode(const std::vector<byte> vFrom)
	{
		std::vector<byte> vEncoded;
		return vEncoded;
	}

	/**
	 * デコード [RFC 2045 準拠] http://www.ietf.org/rfc/rfc2045.txt
	 */
	std::vector<byte> Base64::Decode(const std::string strFrom)
	{
		std::vector<byte> vFrom(strFrom.begin(), strFrom.end());
		return Decode(vFrom);
	}

	/**
	 * デコード [RFC 2045 準拠] http://www.ietf.org/rfc/rfc2045.txt
	 */
	std::vector<byte> Base64::Decode(const std::vector<byte> vFrom)
	{
		std::vector<byte> vDecoded;
		if (vFrom.size() % 4 != 0) {  // invalid size
			// [TODO] error処理
			return vDecoded;
		}

		std::vector<byte>::const_iterator i = vFrom.begin();
		while (i != vFrom.end()) {
			DWORD dwDecoded = 0;
			int nBytes = -1;  // デコード対象として有効なバイト数（'='で補完されていない部分）
			for (int j = 0; j < 4; j++, i++, nBytes++) {
				byte b = 0;
				if ('A' <= *i && *i <= 'Z')
					b = *i - 'A';
				else if ('a' <= *i && *i <= 'z')
					b = 26 + (*i - 'a');
				else if ('0' <= *i && *i <= '9')
					b = 52 + (*i - '0');
				else if (*i == '+')
					b = 62;
				else if (*i == '/')
					b = 63;
				else if (*i == '=') {
					nBytes--;
					continue;
				}
				dwDecoded |= (b << (6 * (3 - j)));
			}
			byte *p = (byte *) &dwDecoded;
			for (int j = 0; j < nBytes; j++) {
				vDecoded.push_back(p[2-j]);
			}
		}
		return vDecoded;
	}
}
