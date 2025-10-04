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
 * Becky!���[���C���f�b�N�X�i1�ʁj
 *
 * <�t�H�[�}�b�g> ��Becky! ver 2.57.01 ����
 * 1���[��1�s�ŋL�q�����B1�s���Ƀ��[���ꗗ�\���p�̍��ڂ�����A'\x01' �ŋ�؂���B
 *  cf. http://www.rimarts.com/b2board/b2board.cgi?ol=200602&tree=r30943
 */
class MailIndex
{
public:
	enum MSGIDFMT { MSGIDFMT_DEFAULT, MSGIDFMT_NOLTFT };
	enum SBJFMT   { SBJFMT_DEFAULT, SBJFMT_NOREFW, SBJFMT_NOMLSEQ };
	enum FROMFMT  { FROMFMT_DEFAULT, FROMFMT_NAME, FROMFMT_ADDRESS };
	enum DATETYPE { DATETYPE_SEND, DATETYPE_RECV, DATETYPE_DNLD };

	/* public�ϐ��Q */
	std::basic_string<TCHAR> strBkMailID;    // Becky�Ǘ���̃��[��ID
	DWORD dwBodyPtr;                         // ���̃��[���A�C�e����bmf�t�@�C�����̐擪����̈ʒu
	DWORD dwMsgID;                           // ���̃��[���A�C�e�����t�H���_���Ń��j�[�N�Ɏ��ʂ���ׂ�DWORD�l
	DWORD dwFileName;                        // bmf�t�@�C���̃t�@�C��������
	std::basic_string<TCHAR> strSubject;     // ���[���̌���
	std::basic_string<TCHAR> strTo;          // ���[���̍��o�l
	std::basic_string<TCHAR> strFrom;        // ���[���̈���
	std::basic_string<TCHAR> strMsgId;       // ���[����Message-Id�t�B�[���h
	std::basic_string<TCHAR> strReferences;  // ���[���̎Q�Ɛ��Message-Id�iIn-Reply-To, Reference�t�B�[���h����擾�j
	time_t tSend;                            // ���[���̑��M�����iC�����time_t�l�j�iDate�t�B�[���h���擾�j
	time_t tRecv;                            // ���[���̔z�M�����iC�����time_t�l�j�iReceived �t�B�[���h���擾�j
	time_t tDnld;                            // ���[���̎�M�����iC�����time_t�l�j�i��M���Ɍ���j
	DWORD dwSize;                            // ���[���̃T�C�Y�i�o�C�g���j
	DWORD dwStatus;                          // ���[���̃X�e�[�^�X�t���O
	                                         // ���ʃr�b�g���ȉ��̈Ӗ��������܂��B
	                                         // 0x00000001 ����
	                                         // 0x00000002 �]���ς�
	                                         // 0x00000004 �ԐM�ς�
	                                         // 0x00000008 �Y�t����iContent-Type��multipart�w�b�_�ł���j
	                                         // 0x00000020 �X���b�h�\���ŁA���ʂ̃��b�Z�[�W�������ԁi�X���b�h�̍ŏ�ʃ��b�Z�[�W�ł̂ݗL���j
	                                         // 0x00000040 �X���b�h�\���ŁA���ʂɃ��b�Z�[�W������
	                                         // 0x00000080 �X���b�h�\���ŁA���ʂɖ��ǃ��b�Z�[�W������
	                                         // 0x00000100 �������b�Z�[�W(message/partial)�̈ꕔ
	                                         // 0x00000200 Resent�w�b�_�ɂ��]�����ꂽ���b�Z�[�W
	                                         // 0x00000400 MDN�����ς݁i�J���ʒm�̑��M�ɓ��ӂ��Ă����Ȃ��Ă��r�b�g�����j
	                                         // ����ȊO�̃r�b�g�͖��g�p���\��ς݂̂��߁A��ɂO
	                                         // 0x00001000 �t���O��
	                                         // 0x00002000 HTML�`��
	                                         // 0x00010000 ����Ɏ����̃��[���A�h���X���܂܂��iv2.24)
	                                         // 0x00020000 Cc�Ɏ����̃��[���A�h���X���܂܂��iv2.24)
	COLORREF nColor;                         // �J���[���x����COLORREF�l
	short nPriority;                         // �T�i�K�̏d�v�x
	DWORD dwParentID;                        // �X���b�h�\���̍ۂ̐e�A�C�e����dwMsgID
	std::basic_string<TCHAR> strCharSet;     // ���̃��[���̃L�����N�^�Z�b�g�i��ł��j
	std::basic_string<TCHAR> strTemp;        // �e���|����������i���e�͕s��A�ʏ��j
	std::basic_string<TCHAR> strExtAtch;     // (v2.05���j�Y�t�t�@�C����ʃt�@�C���ɕۑ����Ă���ꍇ�A
	                                         // ���̓Y�t�t�@�C���̃t�@�C���������B��������ꍇ�� "/" ��
	                                         // ��؂���B
	std::list<std::basic_string<TCHAR>> listExt;  // ���ڐ������������̊g���p�̈�

	/* public�֐��Q */
	MailIndex();                                       // �R���X�g���N�^
	MailIndex(LPCTSTR pbkMailID, LPCTSTR pMailIndex);  // �R���X�g���N�^
	MailIndex(const MailIndex& copyFrom);              // �R�s�[�R���X�g���N�^
	~MailIndex();                                      // �f�X�g���N�^
	std::list<std::basic_string<TCHAR>> getReferences() const;  // ���[���̎Q�Ɛ��Message-Id���X�g
	std::basic_string<TCHAR> getACPSubject() const;
	std::basic_string<WCHAR> getWSubject() const;
	std::basic_string<TCHAR> getFormatedTime(LPCTSTR szFormat, 
		                                     MailIndex::DATETYPE type = MailIndex::DATETYPE_SEND) const;
	std::basic_string<TCHAR> getMessageID(MailIndex::MSGIDFMT format) const;
	std::basic_string<TCHAR> getSubject(MailIndex::SBJFMT format = MailIndex::SBJFMT_DEFAULT,
		                                Charset::Encoding encode = Charset::Encoding::ACP) const;
	std::basic_string<TCHAR> getFrom(MailIndex::FROMFMT format) const;

private:
	/* private �֐��Q */
	void initialize();  // ������
};
