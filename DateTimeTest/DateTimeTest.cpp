// DateTimeTest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	char *testValidDate[] = {
		_T("Thu, 7 Jun 2012 11:46:01 -0700"),
		_T("6 May 2012 13:46:41 +0900"),
		_T("5 Feb 2012 13:46 +0900"),
		NULL
	};
	char *testInvalidDate[] = {
		_T("19 Sep 2012 13:46 +090"),
		_T("1 Nov 2012 13:46:61 +0900"),
		_T("Thu 11 Jan 2012 13:46 +0900"),
		NULL
	};
	char **target[] = { testValidDate, testInvalidDate, NULL };
	TCHAR szField[1024];
	TCHAR szBuf[1024];

	for (char ***arrStrDate = target; *arrStrDate != NULL; arrStrDate++) {
		for (char **date = *arrStrDate; *date != NULL; date++) {
			MessageBox(NULL, *date, _T("test target string"), MB_OK);

			DateTime dt;
			struct tm tmDate = {{0}};
			long int tm_gmtoff = 0;
			dt.Parse(*date, DateTime::DTF_RFC2822, &tmDate, tm_gmtoff);

			LPSTR szTimeFormat = _T("%Y/%m/%d %H:%M:%S");
			_tcsftime(szField, sizeof(szField), szTimeFormat, &tmDate);
			MessageBox(NULL, szField, _T("szField"), MB_OK);

			ltoa(tm_gmtoff, szBuf, 10);
			MessageBox(NULL, szBuf, _T("szBuf"), MB_OK);
		}
	}
	return 0;
}

