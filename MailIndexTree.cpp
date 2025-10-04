#include "MailIndexTree.h"

/* --------------------------------------------------
 * ���[���C���f�b�N�X�m�[�h
 * -------------------------------------------------- */

/**
 * �R���X�g���N�^
 */
MailIndexNode::MailIndexNode(MailIndex *midx)
{
	setMailIndex(midx);
	setParent(NULL);
	this->child = NULL;
	this->next = NULL;
}

/**
 * �f�X�g���N�^
 */
MailIndexNode::~MailIndexNode()
{
}

/**
 * �f�[�^��ݒ�
 */
void MailIndexNode::setMailIndex(MailIndex *midx)
{
	this->midx = midx;
}

/**
 * �f�[�^���擾
 */
const MailIndex* MailIndexNode::getMailIndex() const
{
	return (const MailIndex *) this->midx;
}

/**
 * �q�m�[�h��ǉ�
 */
void MailIndexNode::appendChild(MailIndexNode *child)
{
	/* ���Ɏq�i���j�ɑ��݂�����ǉ����Ȃ��i���[�v����j */
	if (hasGrandChild(child))
		return;

	/* �����̐e�q�֌W��j������ */
	if (child->parent != NULL) {
		child->parent->removeChild(child);
		child->parent = NULL;
	}
	
	/* �V���Ȑe�q�E�Z��֌W���쐬���� */
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
 * �q�m�[�h�����O
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
 * �q�m�[�h���擾
 */
const MailIndexNode* MailIndexNode::getChild() const
{
	return this->child;
}

/**
 * �Z��m�[�h���擾
 */
const MailIndexNode* MailIndexNode::getNext() const
{
	return this->next;
}

/**
 * �e�m�[�h��ݒ�
 */
void MailIndexNode::setParent(MailIndexNode *parent)
{
	this->parent = parent;
}

/**
 * �e�m�[�h���擾
 */
const MailIndexNode* MailIndexNode::getParent() const
{
	return this->parent;
}

/**
 * �m�[�h���q�i���܂Ŋ܂ށj�ɑ��݂��邩
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
 * ���[�g�m�[�h�ł��邩
 */
bool MailIndexNode::isRoot() const
{
	return (this->parent == NULL);
}

/**
 * �q�m�[�h�̐擪�ł��邩
 */
bool MailIndexNode::isFirstChild() const
{
	if (this->isRoot())
		return true;
	return (this->parent->getChild() == this);
}

/**
 * �q�m�[�h�̍Ō���ł��邩
 */
bool MailIndexNode::isLastChild() const
{
	if (this->isRoot())
		return true;
	return (this->next == NULL);
}

/**
 * �q�m�[�h�����邩
 */
bool MailIndexNode::hasChild() const
{
	return (this->child != NULL);
}

/**
 * �Z��m�[�h�����邩
 */
bool MailIndexNode::hasNext() const
{
	return (this->next != NULL);
}

/**
 * �[�����擾�i���[�g�m�[�h�͐[��-1�j
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
 * ���[���C���f�b�N�X�c���[
 * -------------------------------------------------- */

/**
 * �R���X�g���N�^
 */
MailIndexTree::MailIndexTree()
{
	this->rootNode = new MailIndexNode(NULL);
}

/**
 * �f�X�g���N�^
 */
MailIndexTree::~MailIndexTree()
{
}

/**
 * �i�[���Ă���m�[�h�����J�E���g
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
 * ���[���f�[�^�����[���X���b�h�c���[�ɑ}��(Algolrithm by Jamie Zawinski)
 * http://www.jwz.org/doc/threading.html
 */
void MailIndexTree::insert(MailIndex *midx)
{
	using namespace std;

	/* ���[���� id_table �֑}������ */
	MailIndexNode *newNode = NULL;
	map<basic_string<TCHAR>,MailIndexNode*>::iterator pos = this->id_table.find(midx->strMsgId);
	if (pos != this->id_table.end()) {
		pos->second->setMailIndex(midx);
	} else {
		this->id_table.insert(map<basic_string<TCHAR>,MailIndexNode *>::value_type(midx->strMsgId, new MailIndexNode(midx)));
	}

	/* ���[���̎Q�ƌ����c���[�֑}������ */
	MailIndexNode *prev = this->rootNode;
	list<basic_string<TCHAR>> reflist = midx->getReferences();
	for (list<basic_string<TCHAR>>::const_iterator i = reflist.begin(); i != reflist.end(); i++) {
		if (this->id_table.find(*i) == this->id_table.end())
			this->id_table.insert(map<basic_string<TCHAR>,MailIndexNode*>::value_type(*i, new MailIndexNode(NULL)));
		prev->appendChild(this->id_table[*i]);
		prev = this->id_table[*i];
	}

	/* ���[�����c���[�֑}������ */
	MailIndexNode *parent = this->rootNode;
	if (reflist.size() > 0) {
		parent = this->id_table[reflist.back()];
	}
	parent->appendChild(this->id_table[midx->strMsgId]);
}

/**
 * �c���[���k�ڂ���
 */
void MailIndexTree::prune()
{
}
