#pragma once

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include <tchar.h>
#include <windows.h>
#include "Encoding.h"

using namespace std;

/**
 * Becky!メールインデックス（1通）
 *
 * <フォーマット> ※Becky! ver 2.57.01 現在
 * 1メール1行で記述される。1行内にメール一覧表示用の項目があり、'\x01' で区切られる。
 *  cf. http://www.rimarts.com/b2board/b2board.cgi?ol=200602&tree=r30943
 */
class MailIndex
{
public:
	enum MSGIDFMT { MSGIDFMT_DEFAULT, MSGIDFMT_NOLTFT };
	enum SBJFMT   { SBJFMT_DEFAULT, SBJFMT_NOREFW, SBJFMT_NOMLSEQ };
	enum FROMFMT  { FROMFMT_DEFAULT, FROMFMT_NAME, FROMFMT_ADDRESS };
	enum DATETYPE { DATETYPE_SEND, DATETYPE_RECV, DATETYPE_DNLD };

	/* public変数群 */
	std::basic_string<TCHAR> strBkMailID;    // Becky管理上のメールID
	DWORD dwBodyPtr;                         // このメールアイテムのbmfファイル中の先頭からの位置
	DWORD dwMsgID;                           // このメールアイテムをフォルダ中でユニークに識別する為のDWORD値
	DWORD dwFileName;                        // bmfファイルのファイル名部分
	std::basic_string<TCHAR> strSubject;     // メールの件名
	std::basic_string<TCHAR> strTo;          // メールの差出人
	std::basic_string<TCHAR> strFrom;        // メールの宛先
	std::basic_string<TCHAR> strMsgId;       // メールのMessage-Idフィールド
	std::basic_string<TCHAR> strReferences;  // メールの参照先のMessage-Id（In-Reply-To, Referenceフィールドから取得）
	time_t tSend;                            // メールの送信日時（C言語のtime_t値）（Dateフィールドより取得）
	time_t tRecv;                            // メールの配信日時（C言語のtime_t値）（Received フィールドより取得）
	time_t tDnld;                            // メールの受信日時（C言語のtime_t値）（受信時に決定）
	DWORD dwSize;                            // メールのサイズ（バイト数）
	DWORD dwStatus;                          // メールのステータスフラグ
	                                         // 下位ビットより以下の意味を持ちます。
	                                         // 0x00000001 既読
	                                         // 0x00000002 転送済み
	                                         // 0x00000004 返信済み
	                                         // 0x00000008 添付あり（Content-Typeがmultipartヘッダである）
	                                         // 0x00000020 スレッド表示で、下位のメッセージを閉じた状態（スレッドの最上位メッセージでのみ有効）
	                                         // 0x00000040 スレッド表示で、下位にメッセージを持つ
	                                         // 0x00000080 スレッド表示で、下位に未読メッセージを持つ
	                                         // 0x00000100 分割メッセージ(message/partial)の一部
	                                         // 0x00000200 Resentヘッダにより転送されたメッセージ
	                                         // 0x00000400 MDN処理済み（開封通知の送信に同意してもしなくてもビットが立つ）
	                                         // これ以外のビットは未使用か予約済みのため、常に０
	                                         // 0x00001000 フラグつき
	                                         // 0x00002000 HTML形式
	                                         // 0x00010000 宛先に自分のメールアドレスが含まれる（v2.24)
	                                         // 0x00020000 Ccに自分のメールアドレスが含まれる（v2.24)
	COLORREF nColor;                         // カラーラベルのCOLORREF値
	short nPriority;                         // ５段階の重要度
	DWORD dwParentID;                        // スレッド表示の際の親アイテムのdwMsgID
	std::basic_string<TCHAR> strCharSet;     // このメールのキャラクタセット（空でも可）
	std::basic_string<TCHAR> strTemp;        // テンポラリ文字列（内容は不定、通常空）
	std::basic_string<TCHAR> strExtAtch;     // (v2.05より）添付ファイルを別ファイルに保存している場合、
	                                         // その添付ファイルのファイル名部分。複数ある場合は "/" で
	                                         // 区切られる。
	std::list<std::basic_string<TCHAR>> listExt;  // 項目数が増えた時の拡張用領域

	/* public関数群 */
	MailIndex();                                       // コンストラクタ
	MailIndex(LPCTSTR pbkMailID, LPCTSTR pMailIndex);  // コンストラクタ
	MailIndex(const MailIndex& copyFrom);              // コピーコンストラクタ
	~MailIndex();                                      // デストラクタ
	std::list<std::basic_string<TCHAR>> getReferences() const;  // メールの参照先のMessage-Idリスト
	std::basic_string<TCHAR> getACPSubject() const;
	std::basic_string<WCHAR> getWSubject() const;
	std::basic_string<TCHAR> getFormatedTime(LPCTSTR szFormat, 
		                                     MailIndex::DATETYPE type = MailIndex::DATETYPE_SEND) const;
	std::basic_string<TCHAR> getMessageID(MailIndex::MSGIDFMT format) const;
	std::basic_string<TCHAR> getSubject(MailIndex::SBJFMT format = MailIndex::SBJFMT_DEFAULT,
		                                Charset::Encoding encode = Charset::Encoding::ACP) const;
	std::basic_string<TCHAR> getFrom(MailIndex::FROMFMT format) const;

private:
	/* private 関数群 */
	void initialize();  // 初期化
};
