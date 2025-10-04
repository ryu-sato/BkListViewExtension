#pragma once

#include <tchar.h>
#include <string>
#include <sstream>
#include <map>
#include <list>

#include "MailIndex.h"
#include "MailHeaderField.h"
#include "MailHeaderParser.h"
#include "DateParseListener.h"

/**
 * ----------------------------------------------------------------------
 * MailHeader
 * メールヘッダクラス
 * 
 * ヘッダの仕様は MFC5322 に準ずる(cf. http://srgia.com/docs/rfc5322j.html)
 * ----------------------------------------------------------------------
 */
class MailHeader : public MailHeaderParseListener
{
public:
	MailHeader(void);                                                      // コンストラクタ
	MailHeader(std::string strHeader);                                     // コンストラクタ
	~MailHeader(void);                                                     // デストラクタ

	std::list<MailHeaderField> MailHeader::operator[](const TCHAR* szFieldName);  // []演算子のオーバーロード

	const std::string getHeader() const;                                   // ヘッダを取得
	void setHeader(std::string strField);                                  // ヘッダを設定
	const std::list<MailHeaderField> getField(const TCHAR* szName) const;  // 任意のフィールドを取得
	void removeField(const TCHAR* szName);                                 // 任意のフィールドを削除
	void appendField(const TCHAR* szName, const TCHAR* szContent);         // 任意のフィールドを追加
	void appendField(MailHeaderField *field);                              // 任意のフィールドを追加
	virtual bool hookParse(std::string strField, MailHeaderField::MHFTYPE type,
	                       std::ios::pos_type posBeg, std::ios::pos_type posEnd);

private:
	std::list<MailHeaderField> listField;                                  // ヘッダフィールド
};


/**
 * ----------------------------------------------------------------------
 * MailBody
 * メールボディクラス
 * ----------------------------------------------------------------------
 */
class MailBody
{
public:
	MailBody(void);                               // コンストラクタ
	MailBody(std::string strBody);                // コンストラクタ
	~MailBody(void);                              // デストラクタ

	std::string MailBody::operator[](int nPage);  // []演算子のオーバーロード

	const std::string getBody() const;            // ボディを取得
	void setBody(std::string strBody);            // ボディを設定
	const std::string getBoundary() const;        // ページ境界文字列を取得
	void setBoundary(std::string strBoundry);     // ページ境界文字列を設定

private:
	std::list<std::string> listPage;              // メールページ
	std::string strBoundary;                      // ページ境界文字列
};


/**
 * ----------------------------------------------------------------------
 * Mail
 * メールクラス
 * ----------------------------------------------------------------------
 */
class Mail
{
public:
	Mail(void);                               // コンストラクタ
	Mail(std::string strMessage);             // コンストラクタ
	~Mail(void);                              // デストラクタ

	const std::string getMessage() const;     // メッセージ（ヘッダとボディ）を取得
	void setMessage(std::string strMessage);  // メッセージ（ヘッダとボディ）を設定
	const MailHeader getHeader() const;       // メールヘッダを取得
	void setHeader(MailHeader mapField);      // メールヘッダを設定
	const MailBody getBody() const;           // メールボディを取得
	void setBody(MailBody message);           // メールボディを設定
	MailIndex getIndex() const;               // メールインデックスを取得
	MailIndex* createIndex() const;           // メールインデックスを作成

	void parseDateStr(LPCTSTR szField, LPCTSTR szDateFormat, struct tm *tmDate, long int& tm_gmtoff) const;

private:
	MailHeader header;                        // メールヘッダ
	MailBody body;                            // メールボディ

	void setIndex(MailIndex* midx) const;     // メールインデックスデータを設定
};
