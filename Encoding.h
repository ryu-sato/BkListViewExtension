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
	 * �C���X�g�[�����ꂽ�R�[�h�y�[�W�ꗗ
	 */
	class InstalledCodePage
	{
		typedef std::map<std::basic_string<TCHAR>, UINT>::value_type ICP_CONTAINER;

	public:
		const std::map<std::basic_string<TCHAR>, UINT> getCodePageMap(void) const;
		UINT operator[](LPCTSTR lpCodePageName) const;
		static InstalledCodePage& getInstance(void);

	private:
		std::map<std::basic_string<TCHAR>, UINT> cpmap;  // �R�[�h�y�[�W�̖��O�Ɣԍ��̑Ή��\

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
	 * IANA����̃R�[�h�y�[�W���ꗗ
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
		std::map<std::string, UINT> cpmapCache;  // �R�[�h�y�[�W�̖��O�Ɣԍ��̑Ή��\

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
	 * �G���R�[�_�[�N���X
	 */
	class Encoding
	{
	public:
		/* public �v���p�e�B�Q */
		static Encoding ACP;
		static Encoding UTF8;

		/* public �R���X�g���N�^ / �f�X�g���N�^ */
		Encoding(UINT nCodePage);
		Encoding(std::basic_string<CHAR> strCharSetName);
		Encoding(std::basic_string<WCHAR> strCharSetName);
		Encoding(LPCSTR szCharSetName);
		Encoding(LPCWSTR szCharSetName);
		~Encoding(void);

		/* public ���Z�q�̃I�[�o�[���[�h�Q */
		bool operator==(Encoding& target);
		bool operator!=(Encoding& target) { return ! (*this == target); };

		/* public �֐��Q */
		const CPINFOEX           getCPInfoEx(void) const;
		std::vector<byte>        getWBytes(const std::basic_string<WCHAR> &strFrom) const;
		std::vector<byte>        getWBytes(LPCWSTR strFrom) const;
		std::vector<byte>        getBytes(const std::basic_string<CHAR> &strFrom) const;
		std::vector<byte>        getBytes(LPCSTR strFrom) const;
		std::basic_string<WCHAR> getWString(const std::vector<byte> &bytes) const;
		std::basic_string<CHAR>  getString(const std::vector<byte> &bytes) const;

	private:
		/* private �v���p�e�B�Q */
		CPINFOEX cpiex;       // �����R�[�h���

		/* private �֐��Q */
		void initialize(UINT nCodePage);          // �������֐�
		void initialize(LPCSTR szCharSetName);   // �������֐�
		void initialize(LPCWSTR szCharSetName);  // �������֐�
	};

	/**
	 * ==================================================
	 * EncodingException class
	 * ==================================================
	 */
	/**
	 * �G���R�[�_�[��O�N���X
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
