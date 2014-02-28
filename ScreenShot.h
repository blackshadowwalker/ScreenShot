// ScreenShot.h : main header file for the SCREENSHOT application
//

#if !defined(AFX_SCREENSHOT_H__C18E32A5_CBD0_43E0_B3F6_272341A9DCE0__INCLUDED_)
#define AFX_SCREENSHOT_H__C18E32A5_CBD0_43E0_B3F6_272341A9DCE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "SkinPPWTL.h"

/////////////////////////////////////////////////////////////////////////////
// CScreenShotApp:
// See ScreenShot.cpp for the implementation of this class
//
char *formatTime(time_t* t);
bool KCreateFolder(CString folder);

void __cdecl log(const char *format, ...);


class CScreenShotApp : public CWinApp
{
public:
	CScreenShotApp();
	void static OpenLog();
	void static CloseLog();
	static char EventLogFile[256];
	static CString m_appPath;
	static CString g_logFilePath;
	static FILE *g_logFile;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreenShotApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation


};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREENSHOT_H__C18E32A5_CBD0_43E0_B3F6_272341A9DCE0__INCLUDED_)
