// ScreenShot.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ScreenShot.h"
#include "ScreenShotDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CScreenShotApp::CScreenShotApp()
{
}

CScreenShotApp theApp;

#pragma comment(lib, "lib/SkinPPWTL.lib")

CString CScreenShotApp::m_appPath="";
FILE *CScreenShotApp::g_logFile=0;
CString CScreenShotApp::g_logFilePath="";
char CScreenShotApp::EventLogFile[256]={0};

bool KCreateFolder(CString folder)
{
	folder += "/";
	folder.Replace("/","\\");
	int index=2;
	BOOL isOk = true;
	while(1)
	{
	//	folder.Delete(0, index+1);
		index = folder.Find("\\",index+1);
		index = index<0? folder.Find("/") : index;
		if(index<0) break;
		CString str = folder.Left(index);
		isOk = (BOOL)CreateDirectory( str, 0 );
		//if(!isOk） 
		//	return false;
	}
	return true;
}
//判断文件/文件夹是否存在
//判断文件：	FindFirstFileExists(lpPath, FALSE);  
//判断文件夹：	FindFirstFileExists(lpPath, FILE_ATTRIBUTE_DIRECTORY);  
BOOL FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)  
{  
	WIN32_FIND_DATA fd;  
	HANDLE hFind = FindFirstFile(lpPath, &fd);  
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;  
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;  
	FindClose(hFind);  
	return RetValue;  
}  



void __cdecl log(const char *format, ...)
{
    char buf[1024]={0}, *p = buf;
    va_list args;
    va_start(args, format);
    p += _vsnprintf(p, sizeof buf - 1, format, args);
    va_end(args);
  //  while ( p > buf && isspace(p[-1]) ) *--p = '\0';
    *p++ = '\n';
  //  *p = '\0';

	time_t timer;//time_t就是long int 类型
	timer = time(NULL);//这一句也可以改成time(&timer); 
	char str1[64]={0};
	struct tm *tmt =0;
	tmt = localtime(&timer);
	
	sprintf(str1,"%d-%d-%d  %d:%d:%d", tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec );
	printf("[%s] \t%s ", str1, buf);
    OutputDebugString(buf);
}

void CScreenShotApp::OpenLog()
{
	g_logFile = freopen(EventLogFile, "a+",stdout);
	log("OpenLog*******************************");
}

void CScreenShotApp::CloseLog(){
	log("CloseLog*******************************");
	fclose(g_logFile);
}
UINT ThreadLogFile(LPVOID pParam){

	char newFile[256] = {0};
	long sleepTime = 60*60;
	char lpFileName[512]={0};
	sprintf(lpFileName,"%s\\ScreenShotConfig.ini",CScreenShotApp::m_appPath);
	sleepTime = GetPrivateProfileInt("config","logUpdateTime",  60*60, lpFileName);// SEC

	while(1){
		
		CScreenShotApp::CloseLog();
		time_t timer;//time_t就是long int 类型
		timer = time(NULL);//这一句也可以改成time(&timer); 

		struct tm *tmt =0;
		tmt = localtime(&timer);
		if(sleepTime<60)
			sprintf(newFile,"%s\\%d-%d-%d_%d.%d.%d.log", CScreenShotApp::g_logFilePath, tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec );
		else if(sleepTime<60*60)
			sprintf(newFile,"%s\\%d-%d-%d_%d.%d.log", CScreenShotApp::g_logFilePath, tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min );
		else if(sleepTime<24*60*60)
			sprintf(newFile,"%s\\%d-%d-%d_%d.log", CScreenShotApp::g_logFilePath, tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour );
		else
			sprintf(newFile,"%s\\%d-%d-%d.log", CScreenShotApp::g_logFilePath, tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour );
		rename(CScreenShotApp::EventLogFile, newFile);

		CScreenShotApp::OpenLog();
		Sleep(sleepTime*1000);
	}

	return 0;
}
BOOL CScreenShotApp::InitInstance()
{
	skinppLoadSkin(_T("skin/AquaOS.ssk"));

	CString g_szOcxPath = this->m_pszHelpFilePath;
	CString g_szOcxName = this->m_pszExeName;

	g_szOcxName += ".HLP";
	int nTmp = g_szOcxName.GetLength();
	nTmp = g_szOcxPath.GetLength() - nTmp;
	g_szOcxPath = g_szOcxPath.Left(nTmp-1);

	memset(EventLogFile, 0 , 256);
//	CString g_szPath  = g_GetIniPath();
	m_appPath = g_szOcxPath;
	g_logFilePath.Format("%s\\log", g_szOcxPath);
	KCreateFolder(g_logFilePath);
	sprintf(EventLogFile, "%s\\Log.log", g_logFilePath );

//	OpenLog();
	printf("[g_szPath]=%s\n", g_szOcxPath);//this的路径

	AfxBeginThread(ThreadLogFile, this);

	AfxEnableControlContainer();



	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	CScreenShotDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
