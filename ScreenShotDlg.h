// ScreenShotDlg.h : header file
//

#if !defined(AFX_SCREENSHOTDLG_H__313EDF14_5B74_4BB4_AD65_2AE810F39AE5__INCLUDED_)
#define AFX_SCREENSHOTDLG_H__313EDF14_5B74_4BB4_AD65_2AE810F39AE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CScreenShotDlg dialog

class CScreenShotDlg : public CDialog
{
// Construction
public:
	bool timerStarted;
	bool ReadConfig();
	bool SaveConfig();
	CString m_dir;
	long	sleepTime;
	long	reminedTime;
	bool	bExit;
	CScreenShotDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CScreenShotDlg)
	enum { IDD = IDD_SCREENSHOT_DIALOG };
	CComboBox	m_com;
	int		m_time;
	CString	m_status;
	CString	m_statusout;
	long	m_logTimer;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreenShotDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CScreenShotDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtShot();
	afx_msg void OnBtBrowser();
	afx_msg void OnBtSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREENSHOTDLG_H__313EDF14_5B74_4BB4_AD65_2AE810F39AE5__INCLUDED_)
