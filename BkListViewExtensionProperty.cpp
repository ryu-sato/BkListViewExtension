#include "BkListViewExtensionProperty.h"
#include "BkExtend.h"

/**
 * �R���X�g���N�^
 */
BkListViewExtensionProperty::BkListViewExtensionProperty()
{
	/* ini�t�@�C���̃p�X�𐶐� */
	TCHAR szIniFilePath[_MAX_PATH];
	HMODULE hmodule = GetModuleHandle("BkListViewExtension.dll");
	GetModuleFileName(hmodule, szIniFilePath, _MAX_PATH);
	LPSTR lpExt = strrchr(szIniFilePath, '.');
	if (lpExt) {
		lstrcpy(lpExt, ".ini");
	} else {
		lstrcat(szIniFilePath, "_.ini");
	}
	this->strIniFilePath = szIniFilePath;

	readIniFile();
}

/**
 * �f�X�g���N�^
 */
BkListViewExtensionProperty::~BkListViewExtensionProperty(void)
{
}

/**
 * �C���X�^���X�̎擾�i�V���O���g���p�^�[���j
 */
BkListViewExtensionProperty* BkListViewExtensionProperty::getInstance()
{
	static BkListViewExtensionProperty instance;
	return &instance;
}

/**
 * �ݒ�_�C�A���O�{�b�N�X��\��
 */
INT_PTR BkListViewExtensionProperty::showEditDialog(HINSTANCE hInstance, HWND hWnd)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_PROPERTY), hWnd, (DLGPROC) DialogProc);
}

/**
 * �ݒ���擾
 */
const BkListViewExtensionProperty::PROPERTY BkListViewExtensionProperty::getProperty()
{
	return getInstance()->prop;
}

/**
 * �ݒ��ۑ�
 */
void BkListViewExtensionProperty::setProperty(BkListViewExtensionProperty::PROPERTY prop)
{
	getInstance()->prop = prop;
}

/**
 * �_�C�A���O�v���V�[�W��
 */
BOOL CALLBACK BkListViewExtensionProperty::DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BkListViewExtensionProperty::PROPERTY dlgprop;  // �_�C�A���O�{�b�N�X��̐ݒ�l

	switch (uMsg) {
	case WM_INITDIALOG: {
		/* ���ݒl���_�C�A���O�̏����l�֔��f */
		dlgprop = BkListViewExtensionProperty::getProperty();

		CheckRadioButton(hWndDlg, IDC_CPFMT_THOFF_RULE1, IDC_CPFMT_THOFF_RULE2, IDC_CPFMT_THOFF_RULE1 + dlgprop.thoff.rule);
		SendMessage(hWndDlg, WM_COMMAND, IDC_CPFMT_THOFF_RULE1 + dlgprop.thoff.rule, 0);
		SetDlgItemText(hWndDlg, IDC_CPFMT_THOFF_FLWBK_SEPSTR, dlgprop.thoff.strSeperator);
		SetDlgItemText(hWndDlg, IDC_CPFMT_THOFF_FORMAT, dlgprop.thoff.strFormat);

		CheckRadioButton(hWndDlg, IDC_CPFMT_THON_RULE1, IDC_CPFMT_THON_RULE2, IDC_CPFMT_THON_RULE1 + dlgprop.thon.rule);
		SendMessage(hWndDlg, WM_COMMAND, IDC_CPFMT_THON_RULE1 + dlgprop.thon.rule, 0);
		SetDlgItemText(hWndDlg, IDC_CPFMT_THON_FLWBK_SEPSTR, dlgprop.thon.strSeperator);
		SetDlgItemText(hWndDlg, IDC_CPFMT_THON_FORMAT, dlgprop.thon.strFormat);
		SetDlgItemText(hWndDlg, IDC_CPFMT_THON_BULLET_NORMAL, dlgprop.strBulletOfNormalItem);
		SetDlgItemText(hWndDlg, IDC_CPFMT_THON_BULLET_JOINT, dlgprop.strBulletForJoint);
		SetDlgItemText(hWndDlg, IDC_CPFMT_THON_BULLET_LAST, dlgprop.strBulletOfLastItem);
		return TRUE;
	}
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case IDC_CPFMT_THOFF_RULE1:
		case IDC_CPFMT_THOFF_RULE2: {
			/* ���ݒl��ۑ� */
			BOOL onRule1 = (LOWORD(wParam) == IDC_CPFMT_THOFF_RULE1);
			dlgprop.thoff.rule = (onRule1 ? 1 : 0);
			
			/* ���[���̕ύX�ɉ����Đݒ�s�\�ȃI�v�V�����𖳌������� */
			EnableWindow(GetDlgItem(hWndDlg, IDC_CPFMT_THOFF_FLWBK_SEPSTR), onRule1);
			EnableWindow(GetDlgItem(hWndDlg, IDC_CPFMT_THOFF_FORMAT), !onRule1);
			
			/* �T���v����������X�V */
			SendMessage(hWndDlg, WM_COMMAND, IDC_THOFF_RESULT_SAMPLE, 0);
			break;
		}
		case IDC_CPFMT_THON_RULE1:
		case IDC_CPFMT_THON_RULE2: {
			/* ���ݒl��ۑ� */
			BOOL onRule1 = (LOWORD(wParam) == IDC_CPFMT_THON_RULE1);
			dlgprop.thon.rule = (onRule1 ? 1 : 0);

			/* ���[���̕ύX�ɉ����Đݒ�s�\�ȃI�v�V�����𖳌������� */
			EnableWindow(GetDlgItem(hWndDlg, IDC_CPFMT_THON_FLWBK_SEPSTR), onRule1);
			EnableWindow(GetDlgItem(hWndDlg, IDC_CPFMT_THON_FORMAT), !onRule1);
			EnableWindow(GetDlgItem(hWndDlg, IDC_CPFMT_THON_BULLET_NORMAL), !onRule1);
			EnableWindow(GetDlgItem(hWndDlg, IDC_CPFMT_THON_BULLET_JOINT), !onRule1);
			EnableWindow(GetDlgItem(hWndDlg, IDC_CPFMT_THON_BULLET_LAST), !onRule1);
			
			/* �T���v����������X�V */
			SendMessage(hWndDlg, WM_COMMAND, IDC_THON_RESULT_SAMPLE, 0);
			break;
		}
		case IDC_CPFMT_THOFF_FLWBK_SEPSTR:
		case IDC_CPFMT_THON_FLWBK_SEPSTR:
		case IDC_CPFMT_THOFF_FORMAT:
		case IDC_CPFMT_THON_FORMAT:
			/* ���ݒl��ۑ� */
			switch (LOWORD(wParam)) {
			case IDC_CPFMT_THOFF_FLWBK_SEPSTR:
				GetDlgItemText(hWndDlg, LOWORD(wParam), dlgprop.thoff.strSeperator, sizeof(dlgprop.thoff.strSeperator));
				break;
			case IDC_CPFMT_THON_FLWBK_SEPSTR:
				GetDlgItemText(hWndDlg, LOWORD(wParam), dlgprop.thon.strSeperator, sizeof(dlgprop.thon.strSeperator));
				break;
			case IDC_CPFMT_THOFF_FORMAT:
				GetDlgItemText(hWndDlg, LOWORD(wParam), dlgprop.thoff.strFormat, sizeof(dlgprop.thoff.strFormat));
				break;
			case IDC_CPFMT_THON_FORMAT:
				GetDlgItemText(hWndDlg, LOWORD(wParam), dlgprop.thon.strFormat, sizeof(dlgprop.thon.strFormat));
				break;
			}

			/* �t�H�[�}�b�g������ɉ����ăT���v����������X�V */
			if (HIWORD(wParam) == EN_CHANGE) {
				switch (LOWORD(wParam)) {
				case IDC_CPFMT_THOFF_FLWBK_SEPSTR:
				case IDC_CPFMT_THOFF_FORMAT:
					SendMessage(hWndDlg, WM_COMMAND, IDC_THOFF_RESULT_SAMPLE, 0);
					break;
				case IDC_CPFMT_THON_FLWBK_SEPSTR:
				case IDC_CPFMT_THON_FORMAT:
					SendMessage(hWndDlg, WM_COMMAND, IDC_THON_RESULT_SAMPLE, 0);
					break;
				}
			}
			break;
		case IDC_CPFMT_THON_BULLET_NORMAL:
			GetDlgItemText(hWndDlg, IDC_CPFMT_THON_BULLET_NORMAL, dlgprop.strBulletOfNormalItem, sizeof(dlgprop.strBulletOfNormalItem));
			SendMessage(hWndDlg, WM_COMMAND, IDC_THON_RESULT_SAMPLE, 0);
			break;
		case IDC_CPFMT_THON_BULLET_JOINT:
			GetDlgItemText(hWndDlg, IDC_CPFMT_THON_BULLET_JOINT, dlgprop.strBulletForJoint, sizeof(dlgprop.strBulletForJoint));
			SendMessage(hWndDlg, WM_COMMAND, IDC_THON_RESULT_SAMPLE, 0);
			break;
		case IDC_CPFMT_THON_BULLET_LAST:
			GetDlgItemText(hWndDlg, IDC_CPFMT_THON_BULLET_LAST, dlgprop.strBulletOfLastItem, sizeof(dlgprop.strBulletOfLastItem));
			SendMessage(hWndDlg, WM_COMMAND, IDC_THON_RESULT_SAMPLE, 0);
			break;
		case IDC_THON_RESULT_SAMPLE:
		case IDC_THOFF_RESULT_SAMPLE: {
			MailIndex midx;
			midx.strSubject = dlgprop.smpmidx.strSubject;
			midx.strFrom = dlgprop.smpmidx.strFrom;
			midx.strTo = dlgprop.smpmidx.strTo;
			midx.strMsgId = dlgprop.smpmidx.strMessgeID;
			midx.tDnld = dlgprop.smpmidx.tDnld;
			midx.tSend = dlgprop.smpmidx.tSend;
			midx.tRecv = dlgprop.smpmidx.tRecv;
			if (LOWORD(wParam) == IDC_THOFF_RESULT_SAMPLE) {
				TCHAR szResult[1024];
				bksprintf(szResult, sizeof(szResult) / sizeof(szResult[0]), dlgprop.thoff.strFormat, &midx);
				SetDlgItemText(hWndDlg, IDC_THOFF_RESULT_SAMPLE, szResult);
			} else if (LOWORD(wParam) == IDC_THON_RESULT_SAMPLE) {
				std::basic_string<TCHAR> strResult;
				TCHAR szBuf[1024];
				bksprintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), dlgprop.thon.strFormat, &midx);
				strResult.append(szBuf).append(_T("\r\n"));
				strResult.append(dlgprop.strBulletOfNormalItem).append(szBuf).append(_T("\r\n"));
				strResult.append(dlgprop.strBulletForJoint).append(dlgprop.strBulletOfNormalItem).append(szBuf).append(_T("\r\n"));
				strResult.append(dlgprop.strBulletForJoint).append(dlgprop.strBulletOfLastItem).append(szBuf).append(_T("\r\n"));
				strResult.append(dlgprop.strBulletOfLastItem).append(szBuf).append(_T("\r\n"));
				SetDlgItemText(hWndDlg, IDC_THON_RESULT_SAMPLE, strResult.c_str());
			}
			break;
		}
		case IDOK: {
			/* �ݒ�ύX��������ďI������ */
			BkListViewExtensionProperty::getInstance()->setProperty(dlgprop);
			EndDialog(hWndDlg, IDOK);
			return TRUE;
		}
		case IDCANCEL:
			/* �ݒ�ύX�𖳎����ďI������ */
			EndDialog(hWndDlg, IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

/**
 * ini�t�@�C���̓ǂݍ���
 */
void BkListViewExtensionProperty::readIniFile()
{
#define READINI_INT(mem,key,def,maxval) { \
	LPCTSTR szIni = getInstance()->strIniFilePath.c_str(); \
	int n = GetPrivateProfileInt(_T("PROPERTY"), key, def, szIni); \
	getInstance()->prop.mem = min(n, maxval); \
}
#define READINI_STR(mem,key,def,maxval) { \
	LPCTSTR szIni = getInstance()->strIniFilePath.c_str(); \
	GetPrivateProfileString(_T("PROPERTY"), key, def, getInstance()->prop.mem, sizeof(getInstance()->prop.mem), szIni); \
}

	READINI_INT(thoff.rule,            _T("CPFMT_THOFF_RULE"),            0,                                  1);
	READINI_STR(thoff.strSeperator,    _T("CPFMT_THOFF_FLWBK_SEPSTR"),    _T("\\t"),                          256);
	READINI_STR(thoff.strFormat,       _T("CPFMT_THOFF_FORMAT"),          _T("%D{%Y/%m/%d %H:%M}   %s   %f"), 256);
	READINI_INT(thon.rule,             _T("CPFMT_THOFF_RULE"),            0,                                  1);
	READINI_STR(thon.strSeperator,     _T("CPFMT_THON_FLWBK_SEPSTR"),     _T("\\t"),                          256);
	READINI_STR(thon.strFormat,        _T("CPFMT_THON_FORMAT"),           _T("%D{%Y/%m/%d %H:%M}   %s   %f"), 256);
	READINI_STR(strBulletOfNormalItem, _T("CPFMT_THON_BULLET_NORMAL"),    _T("����"),                         32);
	READINI_STR(strBulletForJoint,     _T("CPFMT_THON_BULLET_JOINT"),     _T("���@"),                         32);
	READINI_STR(strBulletOfLastItem,   _T("CPFMT_THON_BULLET_LAST"),      _T("����"),                         32);
	READINI_STR(smpmidx.strSubject,    _T("CPFMT_SAMPLE_MAIL_SUBJECT"),   _T("Re: self introduction"), 256);
	READINI_STR(smpmidx.strFrom,       _T("CPFMT_SAMPLE_MAIL_FROM"),      _T("tatsurou <tatsurou@sample.com>"), 256);
	READINI_STR(smpmidx.strTo,         _T("CPFMT_SAMPLE_MAIL_TO"),        _T("user <user@sample.com>"), 256);
	READINI_STR(smpmidx.strMessgeID,   _T("CPFMT_SAMPLE_MAIL_MESSAGEID"), _T("<200001020304.abcdefgh001122@sample.com>"), 256);
	READINI_INT(smpmidx.tDnld,         _T("CPFMT_SAMPLE_MAIL_DNLD"),      0x386ec850, (size_t)~0);
	READINI_INT(smpmidx.tSend,         _T("CPFMT_SAMPLE_MAIL_SEND"),      0x386ec748, (size_t)~0);
	READINI_INT(smpmidx.tRecv,         _T("CPFMT_SAMPLE_MAIL_RECV"),      0x386ec848, (size_t)~0);
}

/**
 * ini�t�@�C���ւ̏�������
 */
void BkListViewExtensionProperty::saveIniFile()
{
#define WRITEINI_INT(mem,key) { \
	LPCTSTR szIni = getInstance()->strIniFilePath.c_str(); \
	std::stringstream ss; \
	ss << getInstance()->prop.mem; \
	WritePrivateProfileString(_T("PROPERTY"), key, ss.str().c_str(), szIni); \
}
#define WRITEINI_STR(mem,key) { \
	LPCTSTR szIni = getInstance()->strIniFilePath.c_str(); \
	WritePrivateProfileString(_T("PROPERTY"), key, getInstance()->prop.mem, szIni); \
}

	WRITEINI_INT(thoff.rule,            _T("CPFMT_THOFF_RULE"));
	WRITEINI_STR(thoff.strSeperator,    _T("CPFMT_THOFF_FLWBK_SEPSTR"));
	WRITEINI_STR(thoff.strFormat,       _T("CPFMT_THOFF_FORMAT"));
	WRITEINI_INT(thon.rule,             _T("CPFMT_THON_RULE"));
	WRITEINI_STR(thon.strSeperator,     _T("CPFMT_THON_FLWBK_SEPSTR"));
	WRITEINI_STR(thon.strFormat,        _T("CPFMT_THON_FORMAT"));
	WRITEINI_STR(strBulletOfNormalItem, _T("CPFMT_THON_BULLET_NORMAL"));
	WRITEINI_STR(strBulletForJoint,     _T("CPFMT_THON_BULLET_JOINT"));
	WRITEINI_STR(strBulletOfLastItem,   _T("CPFMT_THON_BULLET_LAST"));
	WRITEINI_STR(smpmidx.strSubject,    _T("CPFMT_SAMPLE_MAIL_SUBJECT"));
	WRITEINI_STR(smpmidx.strFrom,       _T("CPFMT_SAMPLE_MAIL_FROM"));
	WRITEINI_STR(smpmidx.strTo,         _T("CPFMT_SAMPLE_MAIL_TO"));
	WRITEINI_STR(smpmidx.strMessgeID,   _T("CPFMT_SAMPLE_MAIL_MESSAGEID"));
	WRITEINI_INT(smpmidx.tDnld,         _T("CPFMT_SAMPLE_MAIL_DATE"));
	WRITEINI_INT(smpmidx.tSend,         _T("CPFMT_SAMPLE_MAIL_SEND"));
	WRITEINI_INT(smpmidx.tRecv,         _T("CPFMT_SAMPLE_MAIL_RECV"));
}
