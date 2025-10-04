#include <windows.h>
#include <tchar.h>
#include "../BeckyApi.h"
#include "../BkExtend.h"

#ifdef __cplusplus
extern "C"{
#endif

LPCTSTR WINAPI BKA_GetCurrentFolder()
{
	static LPCTSTR p = _T("C:\\Users\\tatsurou\\usr\\mail\\Becky\\4cbbef16.mb\\!!!!Inbox");
	return p;
}

LPCTSTR WINAPI BKA_GetDataFolder()
{
	static LPCTSTR p = _T("C:\\Users\\tatsurou\\usr\\mail\\Becky\\");
	return p;
}

LPSTR WINAPI BKA_GetSource(LPCTSTR szbkMailID)
{
	LPCTSTR p = 
		"Return-Path: <bugzilla-daemon@mozilla.gr.jp>\r\n"
		"X-Original-To: tatsurou@zuppa.mine.nu\r\n"
		"Delivered-To: tatsurou@zuppa.mine.nu\r\n"
		"Received: from rat1.mozilla.gr.jp (rat1.mozilla.gr.jp [133.25.90.128])\r\n"
		"	by mail.zuppa.mine.nu (Postfix) with ESMTP id CBC4523F2CF\r\n"
		"	for <tatsurou@zuppa.mine.nu>; Sat,  1 Jan 2000 13:42:37 +0900 (JST)\r\n"
		"Received: (qmail 32140 invoked by uid 400); 5 Nov 2008 15:32:11 +0900\r\n"
		"Date: 5 Nov 2008 15:32:11 +0900\r\n"
		"Message-ID: <20081105063211.32139.qmail@rat1.mozilla.gr.jp>\r\n"
		"From: bugzilla-daemon@mozilla.gr.jp\r\n"
		"To: tatsurou@zuppa.mine.nu\r\n"
		"Subject:\r\n"
		" Bugzilla:=?UTF-8?Q?=20=E3=82=A2=E3=82=AB=E3=82=A6=E3=83=B3=E3=83=88=E4=BD=9C=E6=88=90=E3=81=AE=E7=A2=BA=E8=AA=8D?=\r\n"
		"X-Bugzilla-Type: admin\r\n"
		"Content-Type: text/plain; charset=\"UTF-8\"\r\n"
		"Content-Transfer-Encoding: quoted-printable\r\n"
		"MIME-Version: 1.0\r\n"
		"X-Becky-Status: 010001,000000,3,386d85bd,4d4d87b5,1b8673f4,ffffffff\r\n"
		"\r\n"
		"Bugzilla =E3=81=AF=E3=81=82=E3=81=AA=E3=81=9F=E3=81=AE=E3=83=A1=E3=83=BC=E3=\r\n"
		"=83=AB=E3=82=A2=E3=83=89=E3=83=AC=E3=82=B9 (tatsurou@zuppa.mine.nu) =E3=82=\r\n"
		"=92=E5=88=A9=E7=94=A8=E3=81=97=E3=81=9F\r\n"
		"=E6=96=B0=E8=A6=8F=E3=82=A2=E3=82=AB=E3=82=A6=E3=83=B3=E3=83=88=E4=BD=9C=E6=\r\n"
		"=88=90=E3=81=AE=E8=A6=81=E8=AB=8B=E3=82=92=E5=8F=97=E3=81=91=E5=8F=96=E3=82=\r\n"
		"=8A=E3=81=BE=E3=81=97=E3=81=9F=E3=80=82\r\n"
		"\r\n"
		"=E3=81=93=E3=81=AE=E3=83=A1=E3=83=BC=E3=83=AB=E3=82=A2=E3=83=89=E3=83=AC=E3=\r\n"
		"=82=B9=E3=81=A7=E3=81=AE=E3=82=A2=E3=82=AB=E3=82=A6=E3=83=B3=E3=83=88=E4=BD=\r\n"
		"=9C=E6=88=90=E3=82=92=E7=B6=9A=E8=A1=8C=E3=81=99=E3=82=8B=E3=81=AB=E3=81=AF=\r\n"
		"=E3=80=81=E6=AC=A1=E3=81=AE URL =E3=82=92\r\n"
		"November  8th, 2008 at 15:32 JST =E3=81=BE=E3=81=A7=E3=81=AB=E8=A8=AA=E5=95=\r\n"
		"=8F=E3=81=97=E3=81=A6=E3=81=8F=E3=81=A0=E3=81=95=E3=81=84=E3=80=82\r\n"
		"\r\n"
		"http://bugzilla.mozilla.gr.jp/token.cgi?t=3DrjnJFJInjq&a=3Drequest_new_acco=\r\n"
		"unt\r\n"
		"\r\n"
		"=E3=82=82=E3=81=97=E3=80=81=E3=81=93=E3=81=AE=E3=83=A1=E3=83=BC=E3=83=AB=E3=\r\n"
		"=82=92 November  8th, 2008 at 15:32 JST =E3=82=88=E3=82=8A=E5=89=8D=E3=81=\r\n"
		"=AB\r\n"
		"=E5=8F=97=E3=81=91=E5=8F=96=E3=82=89=E3=81=AA=E3=81=8B=E3=81=A3=E3=81=9F=E3=\r\n"
		"=81=8B=E3=80=81=E3=81=BB=E3=81=8B=E3=81=AE=E3=83=A1=E3=83=BC=E3=83=AB=E3=82=\r\n"
		"=A2=E3=83=89=E3=83=AC=E3=82=B9=E3=81=A7=E3=82=A2=E3=82=AB=E3=82=A6=E3=83=B3=\r\n"
		"=E3=83=88=E3=82=92=E4=BD=9C=E6=88=90=E3=81=97=E3=81=9F=E3=81=84=E5=A0=B4=E5=\r\n"
		"=90=88=E3=81=AF=E3=80=81\r\n"
		"=E6=AC=A1=E3=81=AE URL =E3=81=8B=E3=82=89=E5=86=8D=E8=A9=A6=E8=A1=8C=E3=81=\r\n"
		"=A7=E3=81=8D=E3=81=BE=E3=81=99=E3=80=82\r\n"
		"\r\n"
		"http://bugzilla.mozilla.gr.jp/createaccount.cgi\r\n"
		"\r\n"
		"=E3=83=97=E3=83=A9=E3=82=A4=E3=83=90=E3=82=B7=E3=83=BC=E4=BF=9D=E8=AD=B7=E3=\r\n"
		"=81=AE=E3=81=9F=E3=82=81=E3=81=AB: Bugzilla =E3=81=AF=E9=96=8B=E3=81=8B=E3=\r\n"
		"=82=8C=E3=81=9F bug =E8=BF=BD=E8=B7=A1=E3=82=B7=E3=82=B9=E3=83=86=E3=83=A0=\r\n"
		"=E3=81=A7=E3=81=99=E3=80=82\r\n"
		"=E3=83=A1=E3=83=BC=E3=83=AB=E3=82=A2=E3=83=89=E3=83=AC=E3=82=B9=E3=82=92=E5=\r\n"
		"=90=AB=E3=82=80=E3=81=BB=E3=81=A8=E3=82=93=E3=81=A9=E5=85=A8=E3=81=A6=E3=81=\r\n"
		"=AE bug =E3=81=B8=E3=81=AE=E5=A4=89=E6=9B=B4=E3=81=AF=E3=80=81=E8=AA=B0=E3=\r\n"
		"=81=A7=E3=82=82=E8=A6=8B=E3=82=89=E3=82=8C=E3=81=BE=E3=81=99=E3=80=82\r\n"
		"=E4=BA=88=E5=82=99=E3=81=AE=E3=83=A1=E3=83=BC=E3=83=AB=E3=82=A2=E3=83=89=E3=\r\n"
		"=83=AC=E3=82=B9=E3=80=81=E3=82=82=E3=81=97=E3=81=8F=E3=81=AF=E3=80=81Gmail,=\r\n"
		" Yahoo, Hotmail =E3=81=AA=E3=81=A9=E3=81=A8=E3=81=84=E3=81=A3=E3=81=9F=E3=\r\n"
		"=83=95=E3=83=AA=E3=83=BC=E3=83=A1=E3=83=BC=E3=83=AB=E3=82=A2=E3=82=AB\r\n"
		"=E3=82=A6=E3=83=B3=E3=83=88=E3=82=92=E7=99=BB=E9=8C=B2=E3=81=99=E3=82=8B=E3=\r\n"
		"=81=93=E3=81=A8=E3=82=92=E3=80=81spam =E3=81=AE=E9=98=B2=E6=AD=A2=E3=81=AE=\r\n"
		"=E8=A6=B3=E7=82=B9=E3=81=8B=E3=82=89=E3=81=8A=E5=8B=A7=E3=82=81=E3=81=97=E3=\r\n"
		"=81=BE=E3=81=99=E3=80=82\r\n"
		"\r\n"
		"=E3=82=A2=E3=82=AB=E3=82=A6=E3=83=B3=E3=83=88=E3=82=92=E4=BD=9C=E6=88=90=E3=\r\n"
		"=81=97=E3=81=AA=E3=81=84=E3=81=93=E3=81=A8=E3=82=92=E6=9C=9B=E3=82=80=E3=81=\r\n"
		"=AA=E3=82=89=E3=80=81=E3=82=82=E3=81=97=E3=81=8F=E3=81=AF=E8=A6=81=E8=AB=8B=\r\n"
		"=E3=81=8C=E4=BD=95=E3=82=89=E3=81=8B=E3=81=AE=E9=96=93=E9=81=95=E3=81=84=E3=\r\n"
		"=81=A7=E3=81=82=E3=82=8B=E3=81=AA=E3=82=89=E3=80=81\r\n"
		"=E4=BD=95=E3=82=82=E3=81=97=E3=81=AA=E3=81=84=E3=81=8B=E6=AC=A1=E3=81=AE UR=\r\n"
		"L =E3=82=92=E8=A8=AA=E5=95=8F=E3=81=97=E3=81=A6=E3=81=8F=E3=81=A0=E3=81=95=\r\n"
		"=E3=81=84=E3=80=82\r\n"
		"\r\n"
		"http://bugzilla.mozilla.gr.jp/token.cgi?t=3DrjnJFJInjq&a=3Dcancel_new_accou=\r\n"
		"nt\r\n"
		"\r\n"
		"=E4=B8=8A=E3=81=AE=E3=83=AA=E3=83=B3=E3=82=AF=E3=81=8C=E5=8B=95=E4=BD=9C=E3=\r\n"
		"=81=97=E3=81=AA=E3=81=84=E3=81=AA=E3=82=89=E3=80=81=E3=82=82=E3=81=97=E3=81=\r\n"
		"=8F=E3=81=AF=E3=82=A2=E3=82=AB=E3=82=A6=E3=83=B3=E3=83=88=E3=81=AB=E9=96=A2=\r\n"
		"=E3=81=97=E3=81=A6=E4=BD=95=E3=81=8B=E5=95=8F=E9=A1=8C=E3=81=8C=E3=81=82=E3=\r\n"
		"=82=8B=E3=81=AA=E3=82=89=E3=80=81\r\n"
		"=E7=AE=A1=E7=90=86=E8=80=85 bugzilla-daemon@mozilla.gr.jp =E3=81=AB=E5=95=\r\n"
		"=8F=E3=81=84=E5=90=88=E3=82=8F=E3=81=9B=E3=81=A6=E3=81=BF=E3=81=A6=E3=81=8F=\r\n"
		"=E3=81=A0=E3=81=95=E3=81=84=E3=80=82\r\n";
	LPSTR lp = (LPSTR) malloc(strlen(p) + 1);
	strcpy(lp, p);
	return lp;
}

void WINAPI BKA_Free(LPVOID lp)
{
	free(lp);
}

#ifdef __cplusplus
}
#endif

BOOL CBeckyAPI::InitAPI()
{
	this->GetCurrentFolder = BKA_GetCurrentFolder;
	this->GetDataFolder = BKA_GetDataFolder;
	this->GetSource = BKA_GetSource;
	this->Free = BKA_Free;
	_CrtSetReportMode(_CRT_ASSERT, 0);
	_set_invalid_parameter_handler(Logger::invalidParameterHandler);
	return TRUE;
}

