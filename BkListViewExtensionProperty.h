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
	// �R�s�[�t�H�[�}�b�g�f�[�^
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

	/* ���\�b�h�Q */
	static BkListViewExtensionProperty* getInstance();              // �C���X�^���X���擾�i�V���O���g���p�^�[���j
	static INT_PTR showEditDialog(HINSTANCE hInstance, HWND hWnd);  // �ݒ�_�C�A���O�{�b�N�X��\��
	static const PROPERTY getProperty();                            // �ݒ���擾
	static void setProperty(PROPERTY prop);                         // �ݒ��ۑ�
	static void readIniFile();                                      // ini�t�@�C���̓ǂݍ���
	static void saveIniFile();                                      // ini�t�@�C���ւ̏�������
	static BOOL CALLBACK DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);  // �_�C�A���O�v���V�[�W��

private:
	/* �v���p�e�B�Q */
	PROPERTY prop;                                           // �ݒ�f�[�^
	std::basic_string<TCHAR> strIniFilePath;                 // ini�t�@�C���ւ̃p�X

	/* ���\�b�h�Q */
	BkListViewExtensionProperty();                           // �R���X�g���N�^
	~BkListViewExtensionProperty();                          // �f�X�g���N�^
};
