#pragma once

#include <set>
#include <map>
#include <list>
#include "windows.h"
#include "BkExtend.h"

/**
 * ���[���C���f�b�N�X�m�[�h
 */
class MailIndexNode
{
public:
	/* public �֐��Q */
	MailIndexNode(MailIndex *midx);                                  // �R���X�g���N�^
	~MailIndexNode();                                                // �f�X�g���N�^
	void                 setMailIndex(MailIndex *midx);              // �f�[�^��ݒ�
	const MailIndex*     getMailIndex() const;                       // �f�[�^���擾
	void                 appendChild(MailIndexNode *child);          // �q�m�[�h��ǉ�
	void                 removeChild(MailIndexNode *child);          // �q�m�[�h���폜
	const MailIndexNode* getChild() const;                           // �q�m�[�h���擾
	const MailIndexNode* getNext() const;                            // �Z��m�[�h���擾
	void                 setParent(MailIndexNode *parent);           // �e�m�[�h��ݒ�
	const MailIndexNode* getParent() const;                          // �e�m�[�h���擾
	bool                 hasGrandChild(MailIndexNode *target) const; // �m�[�h���q�i���܂Ŋ܂ށj�ɑ��݂��邩
	bool                 isRoot() const;                             // ���[�g�m�[�h�ł��邩
	bool                 isFirstChild() const;                       // �q�m�[�h�̐擪�ł��邩
	bool                 isLastChild() const;                        // �q�m�[�h�̍Ō���ł��邩
	bool                 hasChild() const;                           // �q�m�[�h�����邩
	bool                 hasNext() const;                            // �Z��m�[�h�����邩
	int                  depth() const;                              // �[�����擾

protected:
	/* protected �ϐ��Q */
	MailIndex     *midx;                  // ���[���C���f�b�N�X�iNULL�����j
	MailIndexNode *parent;                // �e�m�[�h�i���[�g��NULL�j
	MailIndexNode *child;                 // �q�m�[�h
	MailIndexNode *next;                  // �Z��m�[�h�i�Ō����NULL�j
};


/**
 * ���[���C���f�b�N�X�c���[
 */
class MailIndexTree
{
public:
	/* public �ϐ��Q */
	MailIndexNode *rootNode;             // ���[�g�m�[�h

	/* public �֐��Q */
	MailIndexTree();                     // �R���X�g���N�^
	~MailIndexTree();                    // �f�X�g���N�^
	void      insert(MailIndex *midx);   // �f�[�^�}��
	const int countNode();               // �m�[�h�����J�E���g
	void      prune();                   // �c���[���k�ڂ���

protected:
	/* protected �ϐ��Q */
	std::map<std::basic_string<TCHAR>,MailIndexNode*> id_table;  // �c���[���Ŏg�p�����S�Ẵm�[�hID�W��
};
