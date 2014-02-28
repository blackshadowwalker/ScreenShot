// ScreenShotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScreenShot.h"
#include "ScreenShotDlg.h"
#include "afxmt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEvent g_event;
HANDLE hMutex;
HANDLE hMutexExit1;
HANDLE hMutexExit2;
HANDLE hMutexGrab=0;



//ѡ���ļ��� �Ի���
#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0x0040
#endif
CString SelectFolder(CWnd *hWnd)
{
	HWND hwnd= hWnd->GetSafeHwnd();   //�õ����ھ��
	CString filePath= "";		//�õ��ļ�·��			
	LPMALLOC pMalloc;
	if (::SHGetMalloc(&pMalloc) == NOERROR)		//ȡ��IMalloc�������ӿ�
	{   
		BROWSEINFO		bi;
		TCHAR			pszBuffer[MAX_PATH];
		LPITEMIDLIST	pidl;   
		
		bi.hwndOwner		= hwnd;
		bi.pidlRoot			= NULL;
		bi.pszDisplayName   = pszBuffer;
		bi.lpszTitle		= _T("Select Folder"); //ѡ��Ŀ¼�Ի�����ϲ��ֵı���
		//����½��ļ��а�ť BIF_NEWDIALOGSTYLE
		bi.ulFlags			=  BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;
		bi.lpfn				= NULL;
		bi.lParam			= 0;
		bi.iImage			= 0;
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)  //ȡ��IMalloc�������ӿ�
		{   
			if (::SHGetPathFromIDList(pidl, pszBuffer)) //���һ���ļ�ϵͳ·��
			{
				filePath = pszBuffer;
			}
			else filePath="";
			pMalloc->Free(pidl);	//�ͷ��ڴ�
			
		//	MessageBox(m_strPath);
		}
		pMalloc->Release();			//�ͷŽӿ�
	}

	return filePath;
}
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreenShotDlg dialog

CScreenShotDlg::CScreenShotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenShotDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScreenShotDlg)
	m_time = 3;
	m_status = _T("");
	m_statusout = _T("");
	m_dir = _T("");
	timerStarted = false;
	m_logTimer = 3600;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScreenShotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreenShotDlg)
	DDX_Control(pDX, IDC_COMBO1, m_com);
	DDX_Text(pDX, IDC_TIME, m_time);
	DDV_MinMaxInt(pDX, m_time, 1, 9999);
	DDX_Text(pDX, IDC_STATUS, m_status);
	DDX_Text(pDX, IDC_STATUS_OUT, m_statusout);
	DDX_Text(pDX, IDC_DIR, m_dir);
	DDX_Text(pDX, IDC_LOG_TIMER, m_logTimer);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScreenShotDlg, CDialog)
	//{{AFX_MSG_MAP(CScreenShotDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BT_SHOT, OnBtShot)
	ON_BN_CLICKED(IDC_BT_BROWSER, OnBtBrowser)
	ON_BN_CLICKED(IDC_BT_SAVE, OnBtSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreenShotDlg message handlers

BOOL CScreenShotDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ReadConfig();
	m_com.SetCurSel(1);
	

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (!AllocConsole())
		AfxMessageBox("Failed to create the console!", MB_ICONEXCLAMATION);
	freopen("CONOUT$", "w+t", stdout);    // ����д
	printf("Created the console");
	
	return TRUE;  
}

void CScreenShotDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


void CScreenShotDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CScreenShotDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int SaveBitmapToFile(HBITMAP hBitmap, char* lpFileName=0)
{
	//lpFileName Ϊλͼ�ļ���
	HDC     hDC;         
	//�豸������
	int     iBits;      
	//��ǰ��ʾ�ֱ�����ÿ��������ռ�ֽ���
	WORD    wBitCount;   
	//λͼ��ÿ��������ռ�ֽ���
	//�����ɫ���С�� λͼ�������ֽڴ�С ��  λͼ�ļ���С �� д���ļ��ֽ���
	DWORD           dwPaletteSize=0,dwBmBitsSize,dwDIBSize, dwWritten;
	BITMAP          Bitmap;        
	//λͼ���Խṹ
	BITMAPFILEHEADER   bmfHdr;        
	//λͼ�ļ�ͷ�ṹ
	BITMAPINFOHEADER   bi;            
	//λͼ��Ϣͷ�ṹ 
	LPBITMAPINFOHEADER lpbi;          
	//ָ��λͼ��Ϣͷ�ṹ
	HANDLE          fh, hDib, hPal;
	HPALETTE     hOldPal=NULL;
	//�����ļ��������ڴ�������ɫ����

	//����λͼ�ļ�ÿ��������ռ�ֽ���
	hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * 
		GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 32;
	//�����ɫ���С
	if (wBitCount <= 8)
		dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);

	//����λͼ��Ϣͷ�ṹ
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize            = sizeof(BITMAPINFOHEADER);
	bi.biWidth           = Bitmap.bmWidth;
	bi.biHeight          = Bitmap.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount         = wBitCount;
	bi.biCompression      = BI_RGB;
	bi.biSizeImage        = 0;
	bi.biXPelsPerMeter     = 0;
	bi.biYPelsPerMeter     = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant      = 0;

	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;
	printf("Bitmap[%d x %d , dwBmBitsSize=%d ]", Bitmap.bmWidth, Bitmap.bmHeight, dwBmBitsSize);
	//Ϊλͼ���ݷ����ڴ�

	/*xxxxxxxx����λͼ��С�ֽ�һ��(����һ����������)xxxxxxxxxxxxxxxxxxxx 
	//ÿ��ɨ������ռ���ֽ���Ӧ��Ϊ4���������������㷨Ϊ:
	int biWidth = (Bitmap.bmWidth*wBitCount) / 32;
	if((Bitmap.bmWidth*wBitCount) % 32)
	biWidth++; //�����������ļ�1
	biWidth *= 4;//���������õ���Ϊÿ��ɨ���е��ֽ�����
	dwBmBitsSize = biWidth * Bitmap.bmHeight;//�õ���С
	xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/


	hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// �����ɫ��   
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal=SelectPalette(hDC,(HPALETTE)hPal,FALSE);
		RealizePalette(hDC);
	}
	// ��ȡ�õ�ɫ�����µ�����ֵ
	GetDIBits(hDC,hBitmap,0,(UINT)Bitmap.bmHeight,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize, (BITMAPINFO *)lpbi,DIB_RGB_COLORS);
	//�ָ���ɫ��   
	if (hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	//����λͼ�ļ�    
	fh=CreateFile(lpFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh==INVALID_HANDLE_VALUE)
		return FALSE;
	// ����λͼ�ļ�ͷ
	bmfHdr.bfType = 0x4D42;  // "BM"
	dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
	// д��λͼ�ļ�ͷ
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// д��λͼ�ļ���������
	WriteFile(fh, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize , &dwWritten, NULL); 
	//���   
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	if(hBitmap!=0)
		DeleteObject(hBitmap);
	hBitmap = 0;
	return TRUE;
}

HBITMAP hBitmap=0,hOldBitmap=0;
HBITMAP CopyScreenToBitmap(LPRECT lpRect)
{
	HDC hScrDC, hMemDC;      
	// ��Ļ���ڴ��豸������
	   
	// λͼ���
	int       nX, nY, nX2, nY2;      
	// ѡ����������
	int       nWidth, nHeight;      
	// λͼ��Ⱥ͸߶�
	int       xScrn, yScrn;         
	// ��Ļ�ֱ���
	// ȷ��ѡ������Ϊ�վ���
	if (IsRectEmpty(lpRect))
		return NULL;
	//Ϊ��Ļ�����豸������
	hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	//Ϊ��Ļ�豸�����������ݵ��ڴ��豸������
	hMemDC = CreateCompatibleDC(hScrDC);
	// ���ѡ����������
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;
	// �����Ļ�ֱ���
	xScrn = GetDeviceCaps(hScrDC, HORZRES);
	yScrn = GetDeviceCaps(hScrDC, VERTRES);
	//ȷ��ѡ�������ǿɼ���
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > xScrn)
		nX2 = xScrn;
	if (nY2 > yScrn)
		nY2 = yScrn;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	// ����һ������Ļ�豸��������ݵ�λͼ
	if(hBitmap!=0)
		DeleteObject(hBitmap);
	hBitmap=CreateCompatibleBitmap(hScrDC,nWidth,nHeight);
	// ����λͼѡ���ڴ��豸��������
	hOldBitmap=(HBITMAP)SelectObject(hMemDC,hBitmap);
	// ����Ļ�豸�����������ڴ��豸��������
	BitBlt(hMemDC,0,0, nWidth,nHeight,hScrDC, nX, nY, SRCCOPY);
	//�õ���Ļλͼ�ľ��
	hBitmap=(HBITMAP)SelectObject(hMemDC,hOldBitmap);
	//��� 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	// ����λͼ���
	return hBitmap;
}


void CScreenShotDlg::OnBtBrowser() 
{
	UpdateData(true);
	CString dir = SelectFolder(this);	
	if(dir!=""){
		m_dir = dir;
		UpdateData(false);
		SaveConfig();
	}	
}

bool CScreenShotDlg::ReadConfig()
{
	char lpFileName[512]={0};
	char retString[512]={0};
	sprintf(lpFileName,"%s\\ScreenShotConfig.ini",CScreenShotApp::m_appPath);
	
	int retNum = GetPrivateProfileString("config","dir","", retString, 512, lpFileName);
	if(retString!="")
		m_dir.Format("%s",retString);
	m_logTimer = GetPrivateProfileInt("config","logUpdateTime",  3600, lpFileName);// SEC
	UpdateData(false);

	return true;
}

char *formatTime2FileName(time_t* t, char * timeString=0)
{
	if(timeString==0)
		return 0;
	struct tm *tmt =0;
	tmt = localtime(t);
	memset(timeString, 0, 32);
	sprintf(timeString,"%04d%02d%02d_%02d%02d%02d", tmt->tm_year+1900, tmt->tm_mon+1, tmt->tm_mday, tmt->tm_hour, tmt->tm_min, tmt->tm_sec );
	return timeString;
}

HBITMAP hMap;
char temp[512]={0};

UINT ThreadScreenShotTime(LPVOID pParam){
	CScreenShotDlg * dlg = (CScreenShotDlg*)pParam;
	long hour=0;
	long minite=0;
	long sec = 0;

	while(dlg->timerStarted){
		if(dlg->bExit)
			break;
		dlg->reminedTime -= 100;
		if(dlg->reminedTime<1){
			log("SetEvent(hMutexGrab) ");
			SetEvent(g_event);//release signal
		}
		else{
			sec =  dlg->reminedTime/1000%60;
			minite = dlg->reminedTime/1000/60%60;
			hour = (dlg->reminedTime)/1000/3600;
			sprintf(temp,"��ͼ�ȴ�: %d Сʱ %d ���� %d��", hour, minite, sec);
			dlg->m_status.Format(temp);
			dlg->UpdateData(false);
		}
		Sleep(100);
	}
	SetEvent(hMutexExit1);
	
	return 0;
}


UINT ThreadScreenShot(LPVOID pParam){
	CScreenShotDlg * dlg = (CScreenShotDlg*)pParam;
	int error=0;
	CRect rect;
	time_t timer;//time_t����long int ����
	struct tm *tblock=0;
	char filename[512]={0};
	char *timeString= new char[32];

	while(dlg->timerStarted){

		if(dlg->bExit)
			break;
		error = WaitForSingleObject(g_event,1000);
		if(error!=WAIT_OBJECT_0){
		//	log("WaitForSingleObject(g_event) [%d] %d ", error, GetLastError());
			continue;
		}
		WaitForSingleObject(hMutex,-1);//waiting signal

		rect.left = 0;
		rect.top = 0;
		rect.right = GetSystemMetrics(SM_CXSCREEN);
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	
		hMap =  CopyScreenToBitmap(rect); 

		timer = time(NULL);//��һ��Ҳ���Ըĳ�time(&timer);
		tblock = localtime(&timer);
		sprintf(temp, "%s.bmp", formatTime2FileName(&timer, timeString));
		sprintf(filename, "%s\\%s", dlg->m_dir, temp);
	
		SaveBitmapToFile(hMap, filename);

		sprintf(filename,"��ͼ�ɹ�: %s", temp);
		dlg->m_statusout.Format(filename);
		log(" %s ", filename);
		dlg->reminedTime = dlg->sleepTime;
		dlg->UpdateData(false);

		ReleaseMutex(hMutex);//release signal
	}
	delete timeString;
	SetEvent(hMutexExit2);

	return 0;
}

char *timeType[5]={"��","����","Сʱ","����"};

void CScreenShotDlg::OnBtShot() 
{
	UpdateData(true);
	long index = m_com.GetCurSel();
	if(index<0){
		MessageBox("��ѡ��ʱ�䵥λ!");
		return ;
	}
	if(m_dir==""){
		MessageBox("��ѡ��ͼƬ�洢·��!");
		return ;
	}

	timerStarted = !timerStarted;
	long time =  0;
	if(index==0)
		time = m_time*1000;
	else if(index==1)
		time = m_time*60*1000;
	else if(index==2)
		time = m_time*60*60*1000;
	else
		time = m_time;
	if(timerStarted){
		KCreateFolder(m_dir);
		sleepTime = time;
		reminedTime = time;
		bExit = false;

		AfxBeginThread(ThreadScreenShot, this);
		AfxBeginThread(ThreadScreenShotTime, this);
		GetDlgItem(IDC_BT_SHOT)->SetWindowText("ֹͣ");
		m_status.Format("����������Ļ��ͼ!");
	}
	else{
		bExit = true; 
		GetDlgItem(IDC_BT_SHOT)->SetWindowText("����");
		m_status.Format("��ֹͣ������Ļ��ͼ!");
	}
	UpdateData(false);
}

BOOL CScreenShotDlg::DestroyWindow() 
{
	bExit = true; 
	WaitForSingleObject(hMutexExit1, -1);
	WaitForSingleObject(hMutexExit2, -1);

	FreeConsole();

	return CDialog::DestroyWindow();
}



bool CScreenShotDlg::SaveConfig()
{
	UpdateData(true);
	char lpFileName[512]={0};
	char retString[512]={0};
	sprintf(lpFileName,"%s\\ScreenShotConfig.ini",CScreenShotApp::m_appPath);
	WritePrivateProfileString("config","dir", m_dir, lpFileName);
	sprintf(retString, "%d", m_logTimer);
	WritePrivateProfileString("config","logUpdateTime", retString, lpFileName);
	return true;
}

void CScreenShotDlg::OnBtSave() 
{
	SaveConfig();
	MessageBox("����ɹ�!","ϵͳ��ʾ");
}
