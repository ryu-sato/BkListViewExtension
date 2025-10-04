#pragma once
#include "MailHeaderParser.h"

/**
 * ----------------------------------------------------------------------
 * ReceivedParserListener
 * Received フィールドの解析クラス
 * ----------------------------------------------------------------------
 */
class ReceivedParserListener :
	public MailHeaderParser
{
public:
	/* public プロパティ */
	struct tm tmDate;  // 解析した時刻の構造体
	int tm_gmtoffset;  // 解析した時刻の GMT からのオフセット秒

	/* public メソッド */
	ReceivedParserListener(void);  // コンストラクタ
	~ReceivedParserListener(void); // デストラクタ
	virtual bool hookParse(std::string strHeader, MailHeaderField::MHFTYPE type,
	                       std::ios::pos_type posBeg, std::ios::pos_type posEnd);  // Received フィールドの解析結果をフックする
	time_t mkTime();               // 解析した時刻を UTC 時刻からの経過時間を返す
};
