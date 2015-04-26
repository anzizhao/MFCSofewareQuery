
// StructInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StructInfo.h"
#include "StructInfoDlg.h"
//#include "StructSize.h"



// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif


// CStructInfoDlg dialog

CStructInfoDlg::CStructInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStructInfoDlg::IDD, pParent)
	, m_csStructName(_T(""))
	, m_dwStructSize(0)
	, m_byQueryType(FALSE)
	, m_pRcBuf(NULL)
	, m_csInput(_T(""))
	, m_csOutput(_T(""))
	, m_BUpdateData(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	memset(m_szProjectPath, 0, sizeof(m_szProjectPath)); 
	memset(m_szProjectName, 0, sizeof(m_szProjectName)); 
}

CStructInfoDlg::~CStructInfoDlg()
{
	if ( m_pRcBuf )
	{
		delete []m_pRcBuf; 
		m_pRcBuf = NULL; 
	}
}

void CStructInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_STRUCTNAME, m_csStructName);
	DDX_Text(pDX, IDC_EDT_STRUCTSIZE, m_dwStructSize);
	DDX_Radio(pDX, IDC_RADIO_TITLETORESOURCEID, m_byQueryType);
	DDX_Text(pDX, IDC_EDT_RESOURCESEARCH_INPUT, m_csInput);
	DDX_Text(pDX, IDC_EDT_RESOURCESEARCH_INPUT2, m_csOutput);
	DDX_Control(pDX, IDC_COMBO_PROJECT_PATH,  m_cmbPorjectPath);
	DDX_Control(pDX, IDC_COMBO_PROJECT_NAME, m_cmbPorjectName);
}

BEGIN_MESSAGE_MAP(CStructInfoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_EDITCHANGE(IDC_COMBO_PROJECT_PATH, &CStructInfoDlg::onEditChangeProjectPath)
	ON_WM_CLOSE()
//	ON_BN_CLICKED(IDC_BUTTON1, &CStructInfoDlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUT_ADD, &CStructInfoDlg::OnBnClickedButAdd)
ON_CBN_EDITCHANGE(IDC_COMBO_PROJECT_NAME, &CStructInfoDlg::onEditChangeProjectName)
ON_CBN_SELCHANGE(IDC_COMBO_PROJECT_PATH, &CStructInfoDlg::OnCbnSelchangeComboProjectPath)
ON_CBN_SELCHANGE(IDC_COMBO_PROJECT_NAME, &CStructInfoDlg::OnCbnSelchangeComboProjectName)
END_MESSAGE_MAP()


// CStructInfoDlg message handlers

BOOL CStructInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_pRcBuf = new char[RC_BUF_SIZE]; 
	ReadPathDataFile(SAVEFILE); 
	vector<string>::iterator iter = m_vPorjectPath.begin(); 
	while (iter != m_vPorjectPath.end())
	{
		m_cmbPorjectPath.InsertString(0, iter->c_str()); 
		++ iter; 
	}
	iter = m_vPorjectName.begin(); 
	while (iter != m_vPorjectName.end())
	{
		m_cmbPorjectName.InsertString(0, iter->c_str()); 
		++ iter; 
	}
	m_cmbPorjectName.SetCurSel(0); 
	m_cmbPorjectPath.SetCurSel(0); 
//	m_cmbDemoType.SetCurSel(0); 
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStructInfoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStructInfoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CStructInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	//根据不同控件焦点判断是那个在执行
	if(WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam) 
	{ 
		if(GetFocus()== GetDlgItem(IDC_EDT_STRUCTNAME)) 
		{
			QueryStructSize(); 
			return FALSE; 
		}
		else if (GetFocus()== GetDlgItem(IDC_EDT_RESOURCESEARCH_INPUT))
		{
			QueryDemo(); 
			return FALSE; 
		}
		else if (GetFocus()== GetDlgItem(IDC_COMBO_PROJECT_NAME))
		{
			AddProjectPath();
			return FALSE; 
		}
		return FALSE; 
	}

	return CDialog::PreTranslateMessage(pMsg); 
}

BOOL CStructInfoDlg::QueryStructSize()
{
	UpdateData(TRUE);
	//int iStructNumIn = sizeof(SNameInterStruct)/sizeof(SNameInterStruct[0]);
	//for (int i=0; i<iStructNumIn; i++)
	//{
	//	if (m_csStructName == SNameInterStruct[i])
	//	{
	//		//m_dwStructSize = dwSSizeInterStruct[i]; 
	//		UpdateData(FALSE); 
	//		return TRUE; 
	//	}
	//}

	//int iStructNumOut = sizeof(SNameHCNetSDK)/sizeof(SNameHCNetSDK[0]);
	//for (int i=0; i<iStructNumOut; i++)
	//{
	//	if (m_csStructName == SNameHCNetSDK[i])
	//	{
	//		m_dwStructSize = dwSSizeHCNetSDK[i]; 
	//		UpdateData(FALSE); 
	//		return TRUE; 
	//	}
	//}

	//m_dwStructSize = 0; 
	//UpdateData(FALSE); 
	return FALSE;
}


void Test()
{
	AnalyseClassCallRelation dlg; 
	dlg.Test(); 
}

BOOL CStructInfoDlg::QueryDemo()
{
	//char szConfigFilePath[512] = "SConfig.txt"; 
	//ReadConfigFile(szConfigFilePath);
	UpdateData(TRUE);
	CString csTemp; 
	m_cmbPorjectName.GetWindowText(csTemp); 
	sprintf(m_szProjectName, "%s", (LPCTSTR)csTemp); 
	m_cmbPorjectPath.GetWindowText(csTemp); 
	sprintf(m_szProjectPath, "%s", (LPCTSTR)csTemp); 
	switch (m_byQueryType)
	{
	case 0:
		Title2ResourceID(); 
		break; 
	case 1:
		ID2Path(); 
		break; 
	case 2:
		Class2Path(); 
		break; 
	case 3:
		Title2Path();
		break; 
	default:
		break; 
	}
	return TRUE; 
}
//从标题获取资源ID
BOOL CStructInfoDlg::GetIDfromTitle(vector<string> &vID)
{
	vID.clear(); 
	char szLan[512] = {0}; 
	strncpy(szLan, m_csInput, m_csInput.GetLength()); 
	char *pInput = szLan; 
	char *pTemp = ToReadRC()+2; 
	//Todo  资源文件为utf-16 使用两字节存储   
	while( (pTemp = strstr(pTemp, "CAPTION")) != NULL )
	{
		char *pHead = NULL; 
		char *pTail = NULL; 
		WordInLine(pTemp, pHead, pTail);    //单词所在的行 
		if (pHead == pTemp && IsWordInLine(pInput,pHead,pTail))  //查找的单词是否在该行
		{ 
			//查找到对应的，往上找到对应的IDD_DLG_ 就是对应的资源 
			//往上一行行的查找 一般上面2行为对应的资源 ， 空白行为结束 
			//m_csOutput += CString("Caption ") + m_csInput + CString("\r\n");  
			char *pLastHead = NULL; 
			char *pLastTail = NULL; 
			//往上两行   
			LastLine(pHead, pLastHead, pLastTail);
			if (LastLine(pLastHead, pLastHead, pLastTail))
			{
				vID.push_back( GetWord(pLastHead)); 
				//m_csOutput += GetWord(pLastHead)  + CString("\r\n");  
			}

		}
		pTemp = pTail; 
	}
	return TRUE; 
}

//界面标题找资源ID
BOOL CStructInfoDlg::Title2ResourceID()
{
	m_csOutput.Empty(); 
	vector<string> vID; 
	if (! GetIDfromTitle(vID) )
	{
		UpdateData(FALSE); 
		return FALSE; 
	}
	vector<string>::iterator iter = vID.begin(); 
	while( iter != vID.end() )
	{
		m_csOutput += iter->c_str();  
		m_csOutput += CString("\r\n");  
		++iter; 
	}
// 	char szLan[512] = {0}; 
// 	strncpy(szLan, m_csInput, m_csInput.GetLength()); 
// 	char *pInput = szLan; 
// 	char *pTemp = ToReadRC(); 
// 	while( (pTemp = strstr(pTemp, "CAPTION")) != NULL )
// 	{
// 		char *pHead = NULL; 
// 		char *pTail = NULL; 
// 		WordInLine(pTemp, pHead, pTail);    //单词所在的行 
// 		if (pHead == pTemp && IsWordInLine(pInput,pHead,pTail))  //查找的单词是否在该行
// 		{ 
// 			//查找到对应的，往上找到对应的IDD_DLG_ 就是对应的资源 
// 			//往上一行行的查找 一般上面2行为对应的资源 ， 空白行为结束 
// 			//m_csOutput += CString("Caption ") + m_csInput + CString("\r\n");  
// 			char *pLastHead = NULL; 
// 			char *pLastTail = NULL; 
// 			//往上两行   
// 			LastLine(pHead, pLastHead, pLastTail);
// 			if (LastLine(pLastHead, pLastHead, pLastTail))
// 			{
// 				m_csOutput += GetWord(pLastHead)  + CString("\r\n");  
// 			}
// 			
// 		}
// 		pTemp = pTail; 
// 	}
	UpdateData(FALSE); 
	return TRUE; 
}
void CStructInfoDlg::ToCreateAnylseData()
{
	if ( m_BUpdateData)
	{
		m_clsAnaylse.ProjectInfo(m_szProjectName, m_szProjectPath); 
		m_clsAnaylse.Analyse();
		m_BUpdateData = FALSE;
	}	
}

//查询路径  1-通过ID查询 2-通过类名
BOOL CStructInfoDlg::QueryPath(unsigned char byType, CString csInput)
{
	UpdateData(TRUE);
	ToCreateAnylseData();
	m_csOutput.Empty(); 
	char szLan[512] = {0}; 
	if ( csInput != CString() )
	{
		strncpy(szLan, csInput, csInput.GetLength()); 
	}
	else
	{
		strncpy(szLan, m_csInput, m_csInput.GetLength()); 
	}
	
	deque<ClassResourceRelation> qCall; 
	switch (byType)
	{
	case 1:
		m_clsAnaylse.QuerybyID(szLan, qCall);
		break; 
	case 2:
		m_clsAnaylse.QuerybyClass(szLan, qCall);
		break; 
	default:
		return false;
	}
	deque<ClassResourceRelation>::iterator iter = qCall.begin(); 
	while( iter != qCall.end())
	{	
		m_csOutput += iter->struClass.strName.c_str() ; 
		m_csOutput +=  CString('(') ; 
		m_csOutput +=  iter->struResource.strName.c_str(); 
		m_csOutput += CString(')') + CString("\r\n"); 
		++ iter;
	}
	UpdateData(FALSE); 
	return TRUE; 
}

BOOL CStructInfoDlg::ID2Path()
{	
	return QueryPath(1);
}

BOOL CStructInfoDlg::Class2Path()
{
	return QueryPath(2);
}

BOOL CStructInfoDlg::Title2Path()
{
	vector<string> vID; 
	if (! GetIDfromTitle(vID) )
	{
		return FALSE; 
	}
	if ( vID.empty() )
	{
		return FALSE; 
	}
	CString csTemp = vID[0].c_str();
	return QueryPath(1, csTemp); 
	
}

BOOL CStructInfoDlg::WordInLine(char *pWord, char *&pLHead, char *&pLTail)
{
	try
	{
		pLHead = pLTail = NULL; 
		//pWord对m_pRcBuf的偏移 
		DWORD dwOffset = pWord-m_pRcBuf; 
		DWORD i = 0; 
		for (i=dwOffset-1; i>0; i--)
		{
			if (m_pRcBuf[i] == '\n')
			{
				pLHead =  m_pRcBuf + i + 1; //行首地方 
				break; 
			}
		}
		DWORD dwRcBufLen = strlen(m_pRcBuf);  
		i = 1;
		while ( dwOffset+i < dwRcBufLen)
		{
			if (pWord[i] == '\n')
			{
				pLTail =  pWord + i ; //行末地方 
				break; 
			}
			i++; 
		}

	}
	catch (...)
	{
		return FALSE; 
	}
	return TRUE; 

}
//单词是否在该行
BOOL CStructInfoDlg::IsWordInLine(char *pWord ,char *&pLHead, char *&pLTail)
{
	char *pRet = NULL; 
	char pTemp = *pLTail; 
	*pLTail = 0; 
	pRet = strstr(pLHead, pWord);
	*pLTail = pTemp; 
	//返回为0
	return (pRet)?TRUE:FALSE; 
}
//上一行  
BOOL CStructInfoDlg::LastLine(char *pPos, char *&pLHead, char *&pLTail)
{
	try
	{
		pLHead = pLTail = NULL; 
		//pWord对m_pRcBuf的偏移 
		DWORD dwOffset = pPos-m_pRcBuf; 
		DWORD i = 0; 
		for (i=dwOffset-1; i>0; i--)
		{
			if (m_pRcBuf[i] == '\n')
			{
				if (!pLTail)
				{
					pLTail = m_pRcBuf + i;    //上一行的行尾
				}
				else
				{
					pLHead = m_pRcBuf + i + 1;   //上一行的行首
					break; 
				}
			}
		}
		if (i == 0)
		{
			pLHead = m_pRcBuf;
		}

	}
	catch (...)
	{
		return FALSE; 
	}
	return pLTail?TRUE:FALSE; 
}
 //从开始位置 获取单词 
string CStructInfoDlg::GetWord(char *pPos)
{
	try
	{
		DWORD dwRcBufLen = strlen(m_pRcBuf);  
		DWORD dwOffset = pPos-m_pRcBuf; 
		for (int i = 0; i < dwRcBufLen-dwOffset; i++)
		{
			if (*(pPos+i) == ' ')
			{
				string csRet;
				*(pPos+i) = 0; 
				csRet = pPos; 
				//csRet.Format("%s", pPos); 
				*(pPos+i) = ' '; 
				return csRet; 
			}
		}
	}
	catch (...)
	{
		return string(); 
	}
	return string();
}
//读取配置文件
void CStructInfoDlg::ReadPathDataFile(char *pFile)
{
	FILE *fp; 
	fp = fopen(pFile, "rb");
	if ( ! fp)
	{
		return; 
	}
	fseek(fp, 0, SEEK_END); 
	unsigned dwFileLen = ftell(fp);
	rewind(fp);
	char *pBuf = new char[dwFileLen+1]; 
	fread(pBuf, dwFileLen, 1, fp);
	fclose(fp); 

	char *pPos = pBuf; 
	char *pPosEnd = NULL; 
	char *pPROJECT = "<Project>"; 
	while ( pPos = strstr(pPos, pPROJECT))
	{
		pPos += strlen(pPROJECT); 
		if ( !( pPos =strstr(pPos, "<Name>")) )
		{
			continue; 
		}
		pPos += strlen("<Name>"); 
		pPosEnd = strstr(pPos, "</Name>"); 
		if (!pPosEnd)
		{
			continue; 
		}
		char szLan[512]; 
		strncpy(szLan, pPos, pPosEnd-pPos); 
		szLan[pPosEnd-pPos] = 0; 
		m_vPorjectName.push_back(szLan); 

		if ( !( pPos =strstr(pPos, "<Path>")) )
		{
			continue; 
		}
		pPos += strlen("<Path>"); 
		pPosEnd = strstr(pPos, "</Path>"); 
		if (!pPosEnd)
		{
			continue; 
		}
		strncpy(szLan, pPos, pPosEnd-pPos); 
		szLan[pPosEnd-pPos] = 0; 
		m_vPorjectPath.push_back(szLan); 
	}
}
//保存配置文件
void CStructInfoDlg::WritePathDataFile(char *pFile)
{
	FILE *fp; 
	fp = fopen(pFile, "wb");
	if ( ! fp)
	{
		return; 
	}
	vector<string>::iterator iterPN = m_vPorjectName.begin(); 
	vector<string>::iterator iterPP = m_vPorjectPath.begin(); 
	while( iterPN != m_vPorjectName.end() )
	{
		fwrite("<Project>\n", strlen("<Project>\n"), 1, fp); 
		fwrite("<Name>", strlen("<Name>"), 1, fp); 
		fwrite(iterPN->c_str(), strlen(iterPN->c_str()), 1, fp);
		fwrite("</Name>\n", strlen("</Name>\n"), 1, fp); 
		fwrite("<Path>", strlen("<Path>"), 1, fp); 
		fwrite(iterPP->c_str(), strlen(iterPP->c_str()), 1, fp);
		fwrite("</Path>\n", strlen("</Path>\n"), 1, fp); 
		fwrite("</Project>\n", strlen("</Project>\n"), 1, fp); 
		++ iterPN; 
		++ iterPP; 
	}
	fclose(fp); 
}

void CStructInfoDlg::ReadConfigFile(char *pFile)
{
	FILE *fp; 
	fp = fopen(pFile, "rb");
	if ( ! fp)
	{
		return; 
	}
	fseek(fp, 0, SEEK_END); 
	unsigned dwFileLen = ftell(fp);
	rewind(fp);

	char *pBuf = new char[dwFileLen+1]; 
	fread(pBuf, dwFileLen, 1, fp);
	fclose(fp); 
	pBuf[dwFileLen] = 0; 
	char *pPos = NULL; 
	char *pPosEnd = NULL; 
	char *pPROJECTSIGN = "ProjectName="; 
	char *pPROJECTPATH = "ProjectPath=";
	pPos = strstr(pBuf, pPROJECTSIGN);
	
	if (!pPos)
	{
		delete []pBuf;
		return ; 
	}
	pPos += strlen(pPROJECTSIGN);
	int iEndPos = strcspn(pPos," \t\r\n"); 
	strncpy(m_szProjectName, pPos, iEndPos); 
	m_szProjectName[iEndPos] = 0; 
	pPos = strstr(pBuf, pPROJECTPATH);
	
	if (!pPos)
	{
		delete []pBuf;
		return ; 
	}
	pPos += strlen(pPROJECTPATH);
	iEndPos = strcspn(pPos," \t\r\n"); 
	strncpy(m_szProjectPath, pPos, iEndPos); 
	m_szProjectPath[iEndPos] = 0; 
}

char* CStructInfoDlg::ToReadRC()
{
	if ( m_BUpdateData )
	{
		ReadRCFile(); 
	}
	return m_pRcBuf; 
}

BOOL CStructInfoDlg::ReadRCFile()
{

	char szFile[512]; 
	sprintf(szFile, "%s\\%s.rc", m_szProjectPath, m_szProjectName);  
	////读入资源文件
	FILE *fp = fopen(szFile, "rb");
	if (!fp)
	{
		return FALSE; 
	}
	fseek(fp, 0, SEEK_END); 
	DWORD dwFileLen = ftell(fp); 
	rewind(fp);
	fread(m_pRcBuf, 1, dwFileLen, fp);
	m_pRcBuf[dwFileLen] = 0; 
	fclose(fp); 
	return TRUE; 
}
void CStructInfoDlg::onEditChangeProjectName()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); 
	CString csTemp; 
	m_cmbPorjectName.GetWindowText(csTemp); 
	m_cmbPorjectName.ResetContent(); 

	char szLan[512]; 
	char szLan2[512]; 
	sprintf(szLan2,"%s", csTemp);
	vector<string>::iterator iter = m_vPorjectName.begin(); 
	BOOL bShowDrop = FALSE; 
	while (iter != m_vPorjectName.end())
	{
		strncpy(szLan, iter->c_str(), strlen(szLan2)); 
		szLan[strlen(szLan2)] = 0; 
		if ( strcmp(szLan, szLan2) == 0)
		{
			m_cmbPorjectName.InsertString(0, iter->c_str()); 
			bShowDrop = TRUE;
		}
		++ iter; 
	}
	m_cmbPorjectName.ShowDropDown(bShowDrop); 
	m_cmbPorjectName.SetDroppedWidth(m_cmbPorjectName.GetCount()); 
	m_cmbPorjectName.SetWindowText(csTemp); 
	m_cmbPorjectName. SetEditSel(strlen(szLan2), strlen(szLan2));
}



void CStructInfoDlg::onEditChangeProjectPath()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); 
	CString csTemp; 
	m_cmbPorjectPath.GetWindowText(csTemp); 
	m_cmbPorjectPath.ResetContent(); 

	char szLan[512]; 
	char szLan2[512]; 
	sprintf(szLan2,"%s", csTemp);
	vector<string>::iterator iter = m_vPorjectPath.begin(); 
	BOOL bShowDrop = FALSE; 
	while (iter != m_vPorjectPath.end())
	{
		strncpy(szLan, iter->c_str(), strlen(szLan2)); 
		szLan[strlen(szLan2)] = 0; 
		if ( strcmp(szLan, szLan2) == 0)
		{
			m_cmbPorjectPath.InsertString(0, iter->c_str()); 
			bShowDrop = TRUE;
		}
		++ iter; 
	}
	m_cmbPorjectPath.ShowDropDown(bShowDrop); 
	m_cmbPorjectPath.SetDroppedWidth(m_cmbPorjectPath.GetCount()); 
	m_cmbPorjectPath.SetWindowText(csTemp); 
	m_cmbPorjectPath. SetEditSel(strlen(szLan2), strlen(szLan2));

}

void CStructInfoDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	WritePathDataFile(SAVEFILE); 
	CDialog::OnClose();
}

void CStructInfoDlg::AddProjectPath()
{
	UpdateData(TRUE); 
	CString csTemp; 
	m_cmbPorjectName.GetWindowText(csTemp); 
	char szPath[512]; 
	sprintf(szPath,"%s", csTemp);
	m_vPorjectName.push_back(szPath); 
	UpdateData(FALSE); 
}
//void CStructInfoDlg::OnBnClickedButton1()
//{
//	// TODO: Add your control notification handler code here
//
//}

void CStructInfoDlg::OnBnClickedButAdd()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); 
	CString csTemp; 
	m_cmbPorjectName.GetWindowText(csTemp); 
	char szPath[512]; 
	sprintf(szPath,"%s", csTemp);
	m_vPorjectName.push_back(szPath); 
	m_cmbPorjectPath.GetWindowText(csTemp); 
	sprintf(szPath,"%s", csTemp);
	m_vPorjectPath.push_back(szPath); 
}


void CStructInfoDlg::OnCbnSelchangeComboProjectPath()
{
	// TODO: Add your control notification handler code here
	m_BUpdateData = TRUE;
}

void CStructInfoDlg::OnCbnSelchangeComboProjectName()
{
	// TODO: Add your control notification handler code here
	m_BUpdateData = TRUE;
}
