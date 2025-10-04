#pragma warning (disable : 4996)
#pragma warning (disable : 4267)
#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <map>
#include <bitset>
#include <algorithm>
#include <cctype>
#include <string>
#include <list>
#include <vector>
#include <tchar.h>
#include <shlwapi.h>
#include <windows.h>
#include "BeckyApi.h"
#include "BkCommon.h"
#include "MailIndex.h"
#include "Logger.h"

typedef BOOL(*SVPCALLBACK)(int line, int col, std::string elem);

extern CBeckyAPI bka; // You can have only one instance in a project.
extern HINSTANCE g_hInstance;
extern char szIni[_MAX_PATH+2]; // Ini file to save your plugin settings.

BOOL bkIsMailSelected();                                                        // 1�ȏ�̃��[�����I������Ă��邩
BOOL bkIsThreadMode();                                                          // �X���b�h�\������Ă��邩
std::vector<std::basic_string<TCHAR>> bkGetSelectedMail();                      // �I�𒆂̃��[���ꗗ���擾
LPCTSTR bkGetFolderIndexFilePath(LPCTSTR pDataFolder, LPCTSTR pRelPathFolder);  // ���[���C���f�b�N�X�t�@�C���iFolder.idx�j�ւ̃p�X���擾
LPCTSTR bkGetFolderIniFilePath(LPCTSTR pDataFolder, LPCTSTR pRelPathFolder);    // ���[���{�b�N�X�ݒ�t�@�C���iFolder.ini�j�ւ̃p�X���擾
void    bkUnfoldingHeader(std::string &strHeader);                              // ���[���w�b�_�̉��s���������� [RFC2822 ����] http://www.ietf.org/rfc/rfc0822.txt
void    bkDecodeMIMEHeader(const std::string &strMIMEHeader,                    // ���[���w�b�_��MIME���f�R�[�h���� [RFC2047����] http://www.ietf.org/rfc/rfc2047.txt
						   std::basic_string<WCHAR> &strDecoded);
void    bkGetSpecifiedHeader(LPTSTR szResult, size_t maxSize, LPCTSTR szMailID, // ���[���̔C�ӂ̃w�b�_���擾
						     LPCTSTR szHeaderName, LPCTSTR szElementName = NULL);
int     bksprintf(LPSTR szOutput, size_t maxSize, LPCTSTR szFormat,             // �t�H�[�}�b�g�ɏ]���ă��[���C���f�b�N�X�𐮌`
			      const MailIndex *midx);
int     bksprintf(LPSTR szOutput, size_t maxSize, LPCTSTR szFormat,             // �t�H�[�}�b�g�ɏ]���ă��[���C���f�b�N�X�𐮌`
			      LPCTSTR szMailID);
MailIndex* bkCreateMailIndexByFile(LPCTSTR szbkMailID);                         // ���[���C���f�b�N�X���擾
MailIndex* bkCreateMailIndexByHeader(LPCTSTR szbkpMailID);                      // ���[���C���f�b�N�X���擾
void ParseFolderIdx(LPCSTR pFolder, SVPCALLBACK callback);                      // ���[���C���f�b�N�X�t�@�C���������ǂݍ���
