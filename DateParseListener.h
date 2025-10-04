#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <time.h>
#include <locale>
#include "MailHeaderParser.h"

/**
 * ----------------------------------------------------------------------
 * DateParserListener
 * Date フィールドの解析クラス
 * ----------------------------------------------------------------------
 */class DateParseListener :
	public MailHeaderParseListener
{
public:
	/* public プロパティ */
	struct tm tmDate;
	int tm_gmtoffset;

	/* public メソッド */
	DateParseListener(void);   // コンストラクタ
	~DateParseListener(void);  // デストラクタ
	virtual bool hookParse(std::string strHeader, MailHeaderField::MHFTYPE type,
	                       std::ios::pos_type posBeg, std::ios::pos_type posEnd); // Date フィールドの解析結果をフックする
	time_t mkTime();           // 解析した時刻を UTC 時刻からの経過時間を返す
};
