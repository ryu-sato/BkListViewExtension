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

BOOL bkIsMailSelected();                                                        // 1つ以上のメールが選択されているか
BOOL bkIsThreadMode();                                                          // スレッド表示されているか
std::vector<std::basic_string<TCHAR>> bkGetSelectedMail();                      // 選択中のメール一覧を取得
LPCTSTR bkGetFolderIndexFilePath(LPCTSTR pDataFolder, LPCTSTR pRelPathFolder);  // メールインデックスファイル（Folder.idx）へのパスを取得
LPCTSTR bkGetFolderIniFilePath(LPCTSTR pDataFolder, LPCTSTR pRelPathFolder);    // メールボックス設定ファイル（Folder.ini）へのパスを取得
void    bkUnfoldingHeader(std::string &strHeader);                              // メールヘッダの改行を除去する [RFC2822 準拠] http://www.ietf.org/rfc/rfc0822.txt
void    bkDecodeMIMEHeader(const std::string &strMIMEHeader,                    // メールヘッダのMIMEをデコードする [RFC2047準拠] http://www.ietf.org/rfc/rfc2047.txt
						   std::basic_string<WCHAR> &strDecoded);
void    bkGetSpecifiedHeader(LPTSTR szResult, size_t maxSize, LPCTSTR szMailID, // メールの任意のヘッダを取得
						     LPCTSTR szHeaderName, LPCTSTR szElementName = NULL);
int     bksprintf(LPSTR szOutput, size_t maxSize, LPCTSTR szFormat,             // フォーマットに従ってメールインデックスを整形
			      const MailIndex *midx);
int     bksprintf(LPSTR szOutput, size_t maxSize, LPCTSTR szFormat,             // フォーマットに従ってメールインデックスを整形
			      LPCTSTR szMailID);
MailIndex* bkCreateMailIndexByFile(LPCTSTR szbkMailID);                         // メールインデックスを取得
MailIndex* bkCreateMailIndexByHeader(LPCTSTR szbkpMailID);                      // メールインデックスを取得
void ParseFolderIdx(LPCSTR pFolder, SVPCALLBACK callback);                      // メールインデックスファイルを順次読み込み
