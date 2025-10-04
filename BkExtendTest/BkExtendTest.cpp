// BkExtendTest.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

using namespace std;

CBeckyAPI bka; // You can have only one instance in a project.

BOOL parser(int line, int col, std::string elem)
{
	if (line < 3) {
		MessageBox(NULL, elem.c_str(), _T("elem"), MB_OK);
	}
	return true;
}

/* �����R�[�h�ϊ����\�b�h�e�X�g */
int EncodeTest()
{
	{  // �������� 0 �̎�
		Charset::Encoding encACP = Charset::Encoding::ACP;
		std::string empty;
		TCHAR buf[256];
		_sntprintf_s(buf, _TRUNCATE, "getBytes(empty).size() = %d",
			encACP.getBytes(empty).size());
		MessageBox(NULL, buf, _T("ret.size()"), MB_OK);
		_sntprintf_s(buf, _TRUNCATE, "encACP.getWString(encACP.getBytes(empty)).size() = %d",
			encACP.getWString(encACP.getBytes(empty)).size());
		MessageBox(NULL, buf, _T("ret.size()"), MB_OK);
	}
	return 1;
}

int MIMEHeaderTest()
{
	std::string strMIMEHeader;
	std::basic_string<WCHAR> strDecoded;
	Charset::Encoding encACP = Charset::Encoding::ACP;
	TCHAR buf[1024];

	/* ���s�̏��� */
	std::string strFolding = _T("�s1\r\n  �s2\r\n   �s3\r\n    �s4");
	strMIMEHeader = strFolding;
	bkUnfoldingHeader(strMIMEHeader);
	_sntprintf_s(buf, sizeof(buf), _TRUNCATE, _T("'%s' -> '%s'"),
		strFolding.c_str(), strMIMEHeader.c_str());
	MessageBox(NULL, buf, _T("decode MIME header"), MB_OK);

	/* encoded-wor �Ԃ̋󔒖��� */
	strDecoded = L"";
	strMIMEHeader = _T("=?ISO-2022-JP?B?GyRCJCIbKEI=?= =?ISO-2022-JP?B?GyRCJCQbKEI=?=");  // "����"
	bkDecodeMIMEHeader(strMIMEHeader, strDecoded);
	_sntprintf_s(buf, sizeof(buf), _TRUNCATE, _T("'%s' -> '%s'"), strMIMEHeader.c_str(),
		encACP.getString(encACP.getWBytes(strDecoded)).c_str());
	MessageBox(NULL, buf, _T("decode MIME header"), MB_OK);

	strDecoded = L"";
	strMIMEHeader = _T("=?ISO-2022-JP?B?GyRCJCIbKEI=?=  =?ISO-2022-JP?B?GyRCJCQbKEI=?=");  // "����"
	bkDecodeMIMEHeader(strMIMEHeader, strDecoded);
	_sntprintf_s(buf, sizeof(buf), _TRUNCATE, _T("'%s' -> '%s'"), strMIMEHeader.c_str(),
		encACP.getString(encACP.getWBytes(strDecoded)).c_str());
	MessageBox(NULL, buf, _T("decode MIME header"), MB_OK);

	strDecoded = L"";
	strMIMEHeader = _T(" =?ISO-2022-JP?B?GyRCJCIbKEI=?==?ISO-2022-JP?B?GyRCJCQbKEI=?=");  // " ����"
	bkDecodeMIMEHeader(strMIMEHeader, strDecoded);
	_sntprintf_s(buf, sizeof(buf), _TRUNCATE, _T("'%s' -> '%s'"), strMIMEHeader.c_str(),
		encACP.getString(encACP.getWBytes(strDecoded)).c_str());
	MessageBox(NULL, buf, _T("decode MIME header"), MB_OK);

	strDecoded = L"";
	strMIMEHeader = _T("=?ISO-2022-JP?B?GyRCJCIbKEI=?==?ISO-2022-JP?B?GyRCJCQbKEI=?= ");  // "���� "
	bkDecodeMIMEHeader(strMIMEHeader, strDecoded);
	_sntprintf_s(buf, sizeof(buf), _TRUNCATE, _T("'%s' -> '%s'"), strMIMEHeader.c_str(),
		encACP.getString(encACP.getWBytes(strDecoded)).c_str());
	MessageBox(NULL, buf, _T("decode MIME header"), MB_OK);

	/* ISO-2022-JP �̃f�R�[�h */
	strDecoded = L"";
	strMIMEHeader = _T("=?ISO-2022-JP?B?grGC6oKqk/qWe4zqlVyOpoLFgrcoSVNPLTIwMjItSlAp?=");  // ���ꂪ���{��\���ł�(ISO-2022-JP)
	bkDecodeMIMEHeader(strMIMEHeader, strDecoded);
	_sntprintf_s(buf, sizeof(buf), _TRUNCATE, _T("%s -> %s"), strMIMEHeader.c_str(),
		encACP.getString(encACP.getWBytes(strDecoded)).c_str());
	MessageBox(NULL, buf, _T("decode MIME header"), MB_OK);

	/* UTF-8 �̃f�R�[�h */
	strDecoded = L"";
	strMIMEHeader = _T("=?UTF-8?B?44GT44KM44GM5pel5pys6Kqe6KGo56S644Gn44GZKFVURi04KQ==?=");  // ���ꂪ���{��\���ł�(UTF-8)
	bkDecodeMIMEHeader(strMIMEHeader, strDecoded);
	_sntprintf_s(buf, sizeof(buf), _TRUNCATE, _T("%s -> %s"), strMIMEHeader.c_str(),
		encACP.getString(encACP.getWBytes(strDecoded)).c_str());
	MessageBox(NULL, buf, _T("decode MIME header"), MB_OK);
	return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	bka.InitAPI();

	LPCTSTR pFolder = _T("D:\\usr\\mail\\Becky\\4cbbef16.mb\\!!!!Inbox");
	//ParseFolderIdx(pFolder, parser);
	//EncodeTest();
	//MIMEHeaderTest();

	/* ���X�g�r���[�̍��ڂ𕶎���Ŏ擾 */
	LPSTR format = _T("%D{%Y/%m/%d %H:%M}  -  %R{%Y/%m/%d %H:%M}   -   %s   - %f");
//	LPSTR format = _T("%s -- %S");
	basic_string<TCHAR> strListView;
	TCHAR szOutput[256] = _T("");
	int n = 0;
	if (bkIsThreadMode()) {

#if 0
		LPCTSTR arrMailID[] = {
			_T("4cbbef16.mb\\!!!!Inbox\\?1bf2"), 
			_T("4cbbef16.mb\\!!!!Inbox\\?1bfd"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1c00"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1c01"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1c04"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1c07"),
			NULL
		};
#else
		LPCTSTR arrMailID[] = {
			_T("4cbbef16.mb\\!!!!Inbox\\?1b86a1a6"), 
			_T("4cbbef16.mb\\!!!!Inbox\\?1b86a1a7"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1b86a1a8"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1b86a1a9"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1b86a1aa"),
			NULL
		};
#endif

		/* ���[�����X���b�h�\���ɏ]���Ċi�[ */
		MailIndexTree midxTree;
		while (1) {
			LPCTSTR szMailID = arrMailID[n++];
			if (szMailID == NULL)
				break;
			//MailIndex *midx = bkCreateMailIndexByFile(szMailID);
			MailIndex *midx = bkCreateMailIndexByHeader(szMailID);
			if (midx == NULL) {
				Logger::getInstance().print(basic_string<TCHAR>(_T("bkCreateMailIndexByHeader() failed: ")) + szMailID);
				continue;
			}
			midxTree.insert(midx);
		}

		/* �X���b�h�\�������ꂽ���[�����t�H�[�}�b�g�ɏ]���Đ��` */
		typedef pair<const MailIndexNode *,bool *> MIDXNODE_STACK;
		stack<MIDXNODE_STACK> s;
		s.push(MIDXNODE_STACK(midxTree.rootNode->getChild(), new bool(false)));
		while (! s.empty()) {
			MIDXNODE_STACK top = s.top();
			const MailIndexNode *n = top.first;
			if (! *(top.second)) {
				if (n->getMailIndex() != NULL) {
					_sntprintf(szOutput, sizeof(szOutput) / sizeof(szOutput[0]) - 1, _T("%*s + "), n->depth() * 3, _T(" "));
					strListView += szOutput;
					bksprintf(szOutput, sizeof(szOutput) / sizeof(szOutput[0]) - 1, format, n->getMailIndex());
					strListView += szOutput;
					strListView += _T("\r\n");
				}
				if (n->getChild() != NULL)
					s.push(MIDXNODE_STACK(n->getChild(), new bool(false)));
				*(top.second) = true;
			} else {
				s.pop();
				if (n->getNext() != NULL)
					s.push(MIDXNODE_STACK(n->getNext(), new bool(false)));
			}
		}
		MessageBox(NULL, strListView.c_str(), _T("strListView"), MB_OK);
	} else {
		LPCTSTR arrMailID[] = {
			_T("4cbbef16.mb\\!!!!Inbox\\?1c61"), 
			_T("4cbbef16.mb\\!!!!Inbox\\?1c62"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1c63"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1c65"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1c78"),
			_T("4cbbef16.mb\\!!!!Inbox\\?1c74"),
			_T("4cbbef16.mb\\!!!!Inbox\\?17d"),
			NULL
		};

		while (1) {

			/* �I������Ă��郁�[����ID�������擾 */
			LPCTSTR szMailID = arrMailID[n++];
			if (szMailID == NULL)
				break;

			/* ���[��ID����C���f�b�N�X���擾���t�H�[�}�b�g���� */
			MailIndex *midx = bkCreateMailIndexByHeader(szMailID);
			if (midx == NULL) {
				Logger::getInstance().print(basic_string<TCHAR>(_T("bkCreateMailIndexByHeader() failed: ")) + szMailID);
				MessageBox(NULL, szMailID, _T("bkCreateMailIndexByHeader() failed "), MB_OK);
				continue;
			}
			szOutput[0] = _T('\0');
			bksprintf(szOutput, sizeof(szOutput) / sizeof(szOutput[0]) - 1, format, midx);
			MessageBox(NULL, szOutput, _T("szOutput"), MB_OK);
			strListView += szOutput;
			strListView += _T("\r\n");
			delete(midx);
		}
	}

	//BkListViewExtensionProperty *bklveprop = BkListViewExtensionProperty::getInstance();
	//bklveprop->showEditDialog((HINSTANCE) GetModuleHandle(0), NULL);
	return 0;
}

