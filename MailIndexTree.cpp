#include "MailIndexTree.h"

/* --------------------------------------------------
 * メールインデックスノード
 * -------------------------------------------------- */

/**
 * コンストラクタ
 */
MailIndexNode::MailIndexNode(MailIndex *midx)
{
	setMailIndex(midx);
	setParent(NULL);
	this->child = NULL;
	this->next = NULL;
}

/**
 * デストラクタ
 */
MailIndexNode::~MailIndexNode()
{
}

/**
 * データを設定
 */
void MailIndexNode::setMailIndex(MailIndex *midx)
{
	this->midx = midx;
}

/**
 * データを取得
 */
const MailIndex* MailIndexNode::getMailIndex() const
{
	return (const MailIndex *) this->midx;
}

/**
 * 子ノードを追加
 */
void MailIndexNode::appendChild(MailIndexNode *child)
{
	/* 既に子（孫）に存在したら追加しない（ループ回避） */
	if (hasGrandChild(child))
		return;

	/* 既存の親子関係を破棄する */
	if (child->parent != NULL) {
		child->parent->removeChild(child);
		child->parent = NULL;
	}
	
	/* 新たな親子・兄弟関係を作成する */
	MailIndexNode *prev = this->child;
	for (MailIndexNode *p = this->child; p != NULL; p = p->next) {
		prev = p;
	}
	if (this->child == NULL)
		this->child = child;
	else
		prev->next = child;
	child->parent = this;
}

/**
 * 子ノードを除外
 */
void MailIndexNode::removeChild(MailIndexNode *child)
{
	MailIndexNode *prev = this->child;
	for (MailIndexNode *p = this->child; p != NULL && p != child; p = p->next) {
		prev = p;
	}
	if (this->child == NULL || prev->next != child)
		return;
	if (prev->isFirstChild())
		this->child = prev->next;
	else
		prev->next = child->next;
}

/**
 * 子ノードを取得
 */
const MailIndexNode* MailIndexNode::getChild() const
{
	return this->child;
}

/**
 * 兄弟ノードを取得
 */
const MailIndexNode* MailIndexNode::getNext() const
{
	return this->next;
}

/**
 * 親ノードを設定
 */
void MailIndexNode::setParent(MailIndexNode *parent)
{
	this->parent = parent;
}

/**
 * 親ノードを取得
 */
const MailIndexNode* MailIndexNode::getParent() const
{
	return this->parent;
}

/**
 * ノードが子（孫まで含む）に存在するか
 */
bool MailIndexNode::hasGrandChild(MailIndexNode *target) const
{
	for (const MailIndexNode *child = this->getChild(); child != NULL; child = child->getNext()) {
		if (child == target || child->hasGrandChild(target))
			return true;
	}
	return false;
}

/**
 * ルートノードであるか
 */
bool MailIndexNode::isRoot() const
{
	return (this->parent == NULL);
}

/**
 * 子ノードの先頭であるか
 */
bool MailIndexNode::isFirstChild() const
{
	if (this->isRoot())
		return true;
	return (this->parent->getChild() == this);
}

/**
 * 子ノードの最後尾であるか
 */
bool MailIndexNode::isLastChild() const
{
	if (this->isRoot())
		return true;
	return (this->next == NULL);
}

/**
 * 子ノードがいるか
 */
bool MailIndexNode::hasChild() const
{
	return (this->child != NULL);
}

/**
 * 兄弟ノードがいるか
 */
bool MailIndexNode::hasNext() const
{
	return (this->next != NULL);
}

/**
 * 深さを取得（ルートノードは深さ-1）
 */
int MailIndexNode::depth() const
{
	int depth = -1;
	for (const MailIndexNode *n = this; ! n->isRoot(); n = n->parent) {
		depth++;
	}
	return depth;
}

/* --------------------------------------------------
 * メールインデックスツリー
 * -------------------------------------------------- */

/**
 * コンストラクタ
 */
MailIndexTree::MailIndexTree()
{
	this->rootNode = new MailIndexNode(NULL);
}

/**
 * デストラクタ
 */
MailIndexTree::~MailIndexTree()
{
}

/**
 * 格納しているノード数をカウント
 */
const int MailIndexTree::countNode()
{
	using namespace std;

	int num = 0;
	for (map<basic_string<TCHAR>,MailIndexNode*>::iterator i = this->id_table.begin(); i != this->id_table.end(); i++) {
		if (i->second->getMailIndex() != NULL) {
			num++;
		}
	}
	return num;
}

/**
 * メールデータをメールスレッドツリーに挿入(Algolrithm by Jamie Zawinski)
 * http://www.jwz.org/doc/threading.html
 */
void MailIndexTree::insert(MailIndex *midx)
{
	using namespace std;

	/* メールを id_table へ挿入する */
	MailIndexNode *newNode = NULL;
	map<basic_string<TCHAR>,MailIndexNode*>::iterator pos = this->id_table.find(midx->strMsgId);
	if (pos != this->id_table.end()) {
		pos->second->setMailIndex(midx);
	} else {
		this->id_table.insert(map<basic_string<TCHAR>,MailIndexNode *>::value_type(midx->strMsgId, new MailIndexNode(midx)));
	}

	/* メールの参照元をツリーへ挿入する */
	MailIndexNode *prev = this->rootNode;
	list<basic_string<TCHAR>> reflist = midx->getReferences();
	for (list<basic_string<TCHAR>>::const_iterator i = reflist.begin(); i != reflist.end(); i++) {
		if (this->id_table.find(*i) == this->id_table.end())
			this->id_table.insert(map<basic_string<TCHAR>,MailIndexNode*>::value_type(*i, new MailIndexNode(NULL)));
		prev->appendChild(this->id_table[*i]);
		prev = this->id_table[*i];
	}

	/* メールをツリーへ挿入する */
	MailIndexNode *parent = this->rootNode;
	if (reflist.size() > 0) {
		parent = this->id_table[reflist.back()];
	}
	parent->appendChild(this->id_table[midx->strMsgId]);
}

/**
 * ツリーを縮尺する
 */
void MailIndexTree::prune()
{
}
