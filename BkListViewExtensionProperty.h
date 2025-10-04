#pragma once

#include <sstream>
#include <math.h>
#include <windows.h>
#include <tchar.h>
#include <string>
#include "resource.h"

class BkListViewExtensionProperty
{
public:
	// コピーフォーマットデータ
	typedef struct _property
	{
		struct CopyFormat {
			int rule;
			TCHAR strSeperator[256];
			TCHAR strFormat[256];
		} thon, thoff;
		TCHAR strBulletOfNormalItem[32];
		TCHAR strBulletForJoint[32];
		TCHAR strBulletOfLastItem[32];
		struct SampleMailIndex {
			TCHAR strSubject[256];
			TCHAR strFrom[256];
			TCHAR strTo[256];
			TCHAR strMessgeID[256];
			time_t tDnld;
			time_t tSend;
			time_t tRecv;
		} smpmidx;
	} PROPERTY;

	/* メソッド群 */
	static BkListViewExtensionProperty* getInstance();              // インスタンスを取得（シングルトンパターン）
	static INT_PTR showEditDialog(HINSTANCE hInstance, HWND hWnd);  // 設定ダイアログボックスを表示
	static const PROPERTY getProperty();                            // 設定を取得
	static void setProperty(PROPERTY prop);                         // 設定を保存
	static void readIniFile();                                      // iniファイルの読み込み
	static void saveIniFile();                                      // iniファイルへの書き込み
	static BOOL CALLBACK DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);  // ダイアログプロシージャ

private:
	/* プロパティ群 */
	PROPERTY prop;                                           // 設定データ
	std::basic_string<TCHAR> strIniFilePath;                 // iniファイルへのパス

	/* メソッド群 */
	BkListViewExtensionProperty();                           // コンストラクタ
	~BkListViewExtensionProperty();                          // デストラクタ
};
