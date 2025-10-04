#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <tchar.h>
#include <windows.h>

namespace Charset {

	/**
	 * ==================================================
	 * InstalledCodePage class
	 * ==================================================
	 */

	/**
	 * インストールされたコードページ一覧
	 */
	class InstalledCodePage
	{
		typedef std::map<std::basic_string<TCHAR>, UINT>::value_type ICP_CONTAINER;

	public:
		const std::map<std::basic_string<TCHAR>, UINT> getCodePageMap(void) const;
		UINT operator[](LPCTSTR lpCodePageName) const;
		static InstalledCodePage& getInstance(void);

	private:
		std::map<std::basic_string<TCHAR>, UINT> cpmap;  // コードページの名前と番号の対応表

		InstalledCodePage(void);
		~InstalledCodePage(void);
		void append(LPCTSTR lpCodePageName, UINT nCodePage);

		static BOOL CALLBACK EnumCodePagesProc(LPTSTR lpCodePageString);
	};

	/**
	 * ==================================================
	 * CodePageNameMap class
	 * ==================================================
	 */

	/**
	 * IANA既定のコードページ名一覧
	 */
	class CodePageNameMap
	{
		typedef std::map<std::string, UINT>::value_type CPNM_TYPE;
		typedef struct {
			CHAR szCodePageName[46];
			CHAR szCodePage[6];
		} MAPBLOCK;

	public:
		UINT operator[](LPCSTR lpCodePageName);
		static CodePageNameMap& getInstance(void);

	private:
		std::map<std::string, UINT> cpmapCache;  // コードページの名前と番号の対応表

		CodePageNameMap(void);
		~CodePageNameMap(void);
		UINT searchCodePageFile(LPCSTR lpKeyName);
	};

	/**
	 * ==================================================
	 * Encoding class
	 * ==================================================
	 */

	/**
	 * エンコーダークラス
	 */
	class Encoding
	{
	public:
		/* public プロパティ群 */
		static Encoding ACP;
		static Encoding UTF8;

		/* public コンストラクタ / デストラクタ */
		Encoding(UINT nCodePage);
		Encoding(std::basic_string<CHAR> strCharSetName);
		Encoding(std::basic_string<WCHAR> strCharSetName);
		Encoding(LPCSTR szCharSetName);
		Encoding(LPCWSTR szCharSetName);
		~Encoding(void);

		/* public 演算子のオーバーロード群 */
		bool operator==(Encoding& target);
		bool operator!=(Encoding& target) { return ! (*this == target); };

		/* public 関数群 */
		const CPINFOEX           getCPInfoEx(void) const;
		std::vector<byte>        getWBytes(const std::basic_string<WCHAR> &strFrom) const;
		std::vector<byte>        getWBytes(LPCWSTR strFrom) const;
		std::vector<byte>        getBytes(const std::basic_string<CHAR> &strFrom) const;
		std::vector<byte>        getBytes(LPCSTR strFrom) const;
		std::basic_string<WCHAR> getWString(const std::vector<byte> &bytes) const;
		std::basic_string<CHAR>  getString(const std::vector<byte> &bytes) const;

	private:
		/* private プロパティ群 */
		CPINFOEX cpiex;       // 文字コード情報

		/* private 関数群 */
		void initialize(UINT nCodePage);          // 初期化関数
		void initialize(LPCSTR szCharSetName);   // 初期化関数
		void initialize(LPCWSTR szCharSetName);  // 初期化関数
	};

	/**
	 * ==================================================
	 * EncodingException class
	 * ==================================================
	 */
	/**
	 * エンコーダー例外クラス
	 */
	class EncodingException : public std::exception
	{
	public:
		EncodingException();
		Encoding& getACPEncoding();
		std::basic_string<TCHAR> getErrorMessage();
	private:
		DWORD nErrorCode;
	};

}

namespace Binary {
	class Base64 {
	public:
		static std::vector<byte> Encode(const std::string strFrom);
		static std::vector<byte> Encode(const std::vector<byte> vFrom);
		static std::vector<byte> Decode(const std::string strFrom);
		static std::vector<byte> Decode(const std::vector<byte> vFrom);
	};

	class QuotedPrintable {
	};
}
