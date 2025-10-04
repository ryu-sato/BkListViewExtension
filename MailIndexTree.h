#pragma once

#include <set>
#include <map>
#include <list>
#include "windows.h"
#include "BkExtend.h"

/**
 * メールインデックスノード
 */
class MailIndexNode
{
public:
	/* public 関数群 */
	MailIndexNode(MailIndex *midx);                                  // コンストラクタ
	~MailIndexNode();                                                // デストラクタ
	void                 setMailIndex(MailIndex *midx);              // データを設定
	const MailIndex*     getMailIndex() const;                       // データを取得
	void                 appendChild(MailIndexNode *child);          // 子ノードを追加
	void                 removeChild(MailIndexNode *child);          // 子ノードを削除
	const MailIndexNode* getChild() const;                           // 子ノードを取得
	const MailIndexNode* getNext() const;                            // 兄弟ノードを取得
	void                 setParent(MailIndexNode *parent);           // 親ノードを設定
	const MailIndexNode* getParent() const;                          // 親ノードを取得
	bool                 hasGrandChild(MailIndexNode *target) const; // ノードが子（孫まで含む）に存在するか
	bool                 isRoot() const;                             // ルートノードであるか
	bool                 isFirstChild() const;                       // 子ノードの先頭であるか
	bool                 isLastChild() const;                        // 子ノードの最後尾であるか
	bool                 hasChild() const;                           // 子ノードがいるか
	bool                 hasNext() const;                            // 兄弟ノードがいるか
	int                  depth() const;                              // 深さを取得

protected:
	/* protected 変数群 */
	MailIndex     *midx;                  // メールインデックス（NULLを許可）
	MailIndexNode *parent;                // 親ノード（ルートはNULL）
	MailIndexNode *child;                 // 子ノード
	MailIndexNode *next;                  // 兄弟ノード（最後尾はNULL）
};


/**
 * メールインデックスツリー
 */
class MailIndexTree
{
public:
	/* public 変数群 */
	MailIndexNode *rootNode;             // ルートノード

	/* public 関数群 */
	MailIndexTree();                     // コンストラクタ
	~MailIndexTree();                    // デストラクタ
	void      insert(MailIndex *midx);   // データ挿入
	const int countNode();               // ノード数をカウント
	void      prune();                   // ツリーを縮尺する

protected:
	/* protected 変数群 */
	std::map<std::basic_string<TCHAR>,MailIndexNode*> id_table;  // ツリー内で使用される全てのノードID集合
};
