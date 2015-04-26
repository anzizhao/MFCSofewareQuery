
#include "stdafx.h"
#include "CreateClassData.h"
#include <assert.h>
#include <cstring>
//��s�ַ������ҳ�str�ַ���
char *strrstr(char *s, const char *str)
{
    char *p; 
    int len = strlen(s);
    for (p = s + len - 1; p >= s; p--) {
        if ((*p == *str) && (memcmp(p, str, strlen(str)) == 0)) 
            return p;
    }   
    return NULL;
}

//����������
void AnalyseClassCallRelation::Test()
{
	m_strProjectPath = "B:\\TestData\\ClientDemo";    //����Ŀ¼
	m_strProjectName = "ClientDemo";  //��Ŀ����
	Analyse();
	deque<ClassResourceRelation> qCall; 
	QuerybyID("IDD_DLG_REOMTE_TEMPEXCEPTION", qCall);
//	vector<ClassSource> vFileList ;
//	GetFileList(vFileList); 
//	GetClwFile(); 
	return ; 
}
AnalyseClassCallRelation::AnalyseClassCallRelation()
	:m_strProjectPath(".//"),
	m_pMen(NULL)
	,m_pMen2(NULL)
	,m_dwNowMaxSize(0)
	,m_dwNowMaxSize2(0)
{

}

AnalyseClassCallRelation::~AnalyseClassCallRelation()
{
	if (m_pMen)
	{
		delete []m_pMen;
		m_pMen = NULL; 
	}
	if(m_pMen2)
	{
		delete []m_pMen2;
		m_pMen2 = NULL; 
	}

}
//��Ŀ��Ϣ
bool AnalyseClassCallRelation::ProjectInfo(const string  &strProjectName, const string  &strProjectPath)
{
	if (! IsFolderExist(strProjectPath))
	{
		return false; 
	}
	m_vCallData.clear(); 
	m_vBeCallData.clear(); 
	m_vRelationData.clear(); 
	m_strProjectName = strProjectName; 
	m_strProjectPath = strProjectPath; 
	return true; 
}

//Ŀ¼�Ƿ����
bool AnalyseClassCallRelation::IsFolderExist(const string  &strDirPath)
{
	//�жϷ���Ŀ¼�Ƿ����
	WIN32_FIND_DATA  wfd;
    bool rValue = false;
    HANDLE hFind = FindFirstFile(strDirPath.c_str(), &wfd);
    if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        rValue = true;
		m_strProjectPath = strDirPath; 
    }
    FindClose(hFind);
    return rValue;
}

//����clw�ļ�
bool AnalyseClassCallRelation::GetClwFile()
{

	string strClwFilePath; 
	strClwFilePath = m_strProjectPath + "\\" + m_strProjectName + ".clw"; 
	string strContent; 
	if (! GetFile(strClwFilePath, strContent))
	{
		return false; 
	}
	m_vRelationData.clear();
	int iFileLen = strContent.length(); 
	char *pTemp = &strContent[0]; 
	string::size_type DlgPos = 0; 
	//Ѱ��[DLG: ��Ѱ�Ҷ�Ӧ��[CLS:
	const string  DLGFLAG("[DLG:");  //dlg ��Դ���
	const string  DLGCLASSFLAG("Class="); //dlg�µ�class
	const string  CLASSFLAG("[CLS:"); //class���
	const string  HEADERFILEFLAG("HeaderFile="); //HeaderFile���
	const string  IMPFILE("ImplementationFile="); //ImplementationFile���
	while( (DlgPos = strContent.find(DLGFLAG, DlgPos)) != string::npos )
	{
		//��ȡ��ԴID 
		ClwResouceItem clsResItem; 
		string::size_type TempPos = 0; 
		TempPos = strContent.find(']', DlgPos); 
		assert(TempPos != string::npos);
		clsResItem.strName = strContent.substr(DlgPos+DLGFLAG.length(), TempPos-DlgPos-DLGFLAG.length());
		string::size_type TempPosS = 0;
		TempPos = strContent.find(DLGCLASSFLAG, DlgPos);
		assert(TempPos != string::npos);
		DlgPos += DLGFLAG.length();   //һ���������  ׼����һ��

		TempPosS = strContent.find_first_of(" \r\n", TempPos);  
		assert(TempPosS != string::npos);
		ClwClassItem clsClassItem; 
		clsClassItem.strName =  strContent.substr(TempPos+DLGCLASSFLAG.length(), TempPosS-TempPos-DLGCLASSFLAG.length());

		//Ѱ�Ҷ�Ӧ��class ��ʽ�̶���ͷ�ļ� ����ʵ���ļ�
		if (clsClassItem.strName == "?")
		{
			DlgPos += DLGFLAG.length();   //clw�ļ�������  ��������  ��������Ƴ���������
			continue; 
		}
		string strClass; 
		strClass = string("[CLS:")+clsClassItem.strName+string("]");
		TempPos = strContent.find(strClass);
		//clw��һ��׼ȷ  ��ʱ������Դ  û���ļ�
		if ( TempPos == string::npos )
		{
			continue; 
		}
		TempPos = strContent.find(HEADERFILEFLAG, TempPos);
		assert(TempPos != string::npos);
		TempPosS = strContent.find_first_of(" \n\r", TempPos); 
		clsClassItem.strHeaderFile = strContent.substr(TempPos+HEADERFILEFLAG.length(), TempPosS-TempPos-HEADERFILEFLAG.length());

		TempPos = strContent.find(IMPFILE, TempPos);
		assert(TempPos != string::npos);
		TempPosS = strContent.find_first_of(" \n\r", TempPos); 
		clsClassItem.strImplementationFile = strContent.substr(TempPos+IMPFILE.length(), TempPosS-TempPos-IMPFILE.length());
		ClassResourceRelation struRelation; 
		struRelation.struClass = clsClassItem; 
		struRelation.struResource = clsResItem; 
		m_vRelationData.push_back(struRelation); 

		DlgPos += DLGFLAG.length();   //һ���������  ׼����һ��
	}
	return true; 
}
//��ʼ����   
bool AnalyseClassCallRelation::Analyse()
{
	//�Ƿ��ȡ��ȡclw�ļ� 
	//����clw�ļ���ȡ������Դ�Ĺ�ϵ
	/*
	if ( m_vRelationData.empty() )
	{
		GetClwFile(); 
	}
	*/
	//��ȡ�ļ��б� 
	vector<ClassSource> vFileList; 
	if (! GetFileList(vFileList))
	{
		return false; 
	}
	//�ȷ���7K���ڴ���������ļ�
	GetMemory(40*1024);
	//���ļ��б����ļ�������
	vector<string> vClass;
	vector<ClassSource>::iterator iter = vFileList.begin(); 
	FILE *fp; 
	fp = fopen("Test.data", "w"); 
	while (iter != vFileList.end() )
	{
		//��ȡͷ�ļ���Ϣ�� �ó����ļ���Ҫ�������� 
		vClass.clear();
		string strFileName = iter->strName + ".h"; 

		DWORD dwFileStartTime = GetTickCount();  
		if (! GetClassFromHeader(strFileName, vClass) )
		{
			++ iter ;
			continue; 
		}
		//�ж��Ƿ�Ϊclw�е���
		vector<string>::iterator strIter = vClass.begin(); 
		while (strIter != vClass.end() )
		{

//			if (IsClwClass(*strIter))
			{
				//��clw�е��࣬������ ������ʵ���ļ� ͷ�ļ���ʵ���ļ�ֻ�к�׺����
				Analyse(iter->strName, *strIter); 
			}
			++ strIter; 
		}
		
		++ iter ;
		DWORD dwFileEndTime = GetTickCount(); 
		fprintf(fp, "%s %d\n", strFileName.c_str(), dwFileEndTime-dwFileStartTime); 

	}
	fclose(fp); 
	return true; 
}
void AnalyseClassCallRelation::Analyse(const string  &strFileName, const string &strClassName)
{
	//������ �ҵ���Ӧͷ�ļ�����Դ
	string strSourceFile;
	strSourceFile = m_strProjectPath + string("\\") + strFileName + string(".cpp");
	string strContent; 
	if (! GetFile(strSourceFile, strContent))
	{
		return ; 
	}
	//ģʽ ����.DoModal��������  ���ҵ���Ӧ����  д������
	ClassItem  struCallClass;
	struCallClass.strClassName = strClassName;
	char *pBuf = GetMemory(strContent.length()+1);
	strcpy(pBuf, strContent.c_str()); 
	char *pTemp = pBuf; 
	unsigned int dwFileLen = strContent.length(); 
	char *pWHead = NULL; 
	char *pWTail = NULL; 
	string strClassObj; 
	string strClass; 
	bool bRet; 
	while( (pTemp = strstr(pTemp, ".DoModal()")) != NULL )
	{
		bRet = GetWord(pTemp, pBuf, pBuf+dwFileLen, pWHead, pWTail); 
		assert( bRet ); 
		strClassObj = GetStr(pWHead, pTemp-1);
		
		if( FindClassObjDefine(strClassObj.c_str(), pWHead,pBuf, pBuf+dwFileLen,  strClass) )
		{
			CallItem struBeCallItem ; 
			struBeCallItem.strClassName = strClass; 
			struCallClass.vecItems.push_back(struBeCallItem); 

			//�����ù�ϵ
			CallItem struCallItem ;  
			struCallItem.strClassName = strClassName;
			int iItem; 
			if ( GetItemFromBeCallData(strClass, &iItem) )
			{
				//���ݿ�����
				ClassItem &clsItem = m_vBeCallData[iItem];			
				clsItem.vecItems.push_back(struCallItem); 
			}
			else
			{
				//�µı����ù�ϵ
				ClassItem clsItem; 
				clsItem.strClassName = strClass; 
				clsItem.vecItems.push_back(struCallItem); 
				m_vBeCallData.push_back(clsItem); 
			}
		}
		pTemp = pWTail + 1; 
	}
	//������ģʽ
	pTemp = pBuf; 
	char *pPos = NULL; 
	char *pnewLine = NULL;
	char *pCreateSign = "Create"; 
	while( (pTemp = strstr(pTemp, pCreateSign)) != NULL )
	{
		//�жϺ���һ���ǲ��ǿո񻹣�
		char cTemp = *(pTemp+strlen(pCreateSign)); 
		switch (cTemp)
		{
		case ' ':
		case '(':
		case '\t':
			break; 
		default:
			pTemp += strlen(pCreateSign); 
			continue; 
			break;
		}
		pnewLine = strchr(pTemp, '\n'); 
		pPos = strstr(pTemp, "IDD"); 
		if (!pPos || pnewLine < pPos )
		{
			//��ͬһ�� û��IDD����  ������ �������
			pTemp += strlen(pCreateSign); 
			continue; 
		}
		//��һ�������  m_subDVRNVRCfg.Create(CSubDlgDVRNVRCfg::IDD,this);
		//��ȡ��ԴID
		string strID; 
		bRet = GetWord(pPos, pBuf, pBuf+dwFileLen, pWHead, pWTail); 
		assert( bRet ); 
		strID = GetStr(pWHead, pWTail);
		//��ȡ�����
		//��ȡ���� 
		GetLineHead(pPos, pBuf, pBuf+dwFileLen, pPos);
		SkipSpace(pPos, pBuf+dwFileLen, pPos);   //�����հ�
		bRet = GetPureWord(pPos, pBuf, pBuf+dwFileLen, pWHead, pWTail);
		assert( bRet );
		strClassObj = GetStr(pWHead, pWTail);
		if( FindClassObjDefine(strClassObj.c_str(), strFileName,  strClass) )
		{
			CallItem struBeCallItem ; 
			struBeCallItem.strClassName = strClass; 
			struCallClass.vecItems.push_back(struBeCallItem); 

			//�����ù�ϵ
			CallItem struCallItem ;  
			struCallItem.strClassName = strClassName;
			int iItem; 
			//�ӱ��������ݿ�鿴�Ƿ��и����¼
			if ( GetItemFromBeCallData(strClass, &iItem) )
			{
				//���ݿ�����
				ClassItem &clsItem = m_vBeCallData[iItem];
				//�鿴�Ƿ��¼�õ��ù�ϵ
				vector<CallItem>::iterator iter = clsItem.vecItems.begin(); 
				while (iter != clsItem.vecItems.end() )
				{
					if (iter->strClassName == strClass)
					{
						break; 
					}
					++ iter; 
				}
				if (iter == clsItem.vecItems.end())
				{
					//û�иĵ��ü�¼ ���
					clsItem.vecItems.push_back(struCallItem); 
				}
				
			}
			else
			{
				//�µı����ù�ϵ
				ClassItem clsItem; 
				clsItem.strClassName = strClass; 
				clsItem.vecItems.push_back(struCallItem); 
				m_vBeCallData.push_back(clsItem); 
			}
		}
		pTemp += strlen(pCreateSign); 
	}
	m_vCallData.push_back(struCallClass); 
}
//��ȡposλ�����ڵĴ����ĵ��� 
bool AnalyseClassCallRelation::GetPureWord(char *pPos, char *pMenStart, char *pMenEnd, char *&pWHead, char *&pWTail )
{
	char *pTemp = pPos; 
	try
	{
		////��ȡ����λ�õ�����
		//GetLineHead(pTemp, pMenStart, pMenEnd, pTemp);
		//SkipSpace(pTemp, pMenEnd, pTemp);   //�����հ�
		//��ʼλ��
		while(pTemp>=pMenStart && *pTemp != ' '&& *pTemp != '\t' && *pTemp != '\r' 
			&& *pTemp != '\n' && *pTemp != '(')
		{
			-- pTemp;
		}

		//��ȡ���ʿ�ʼλ��
		char *pPureWordElem = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890_"; 
		int ioffset = strcspn(pTemp, pPureWordElem); 
		pWHead = pTemp + ioffset; 
		pTemp = pWHead + 1; 
		while( strchr(pPureWordElem, *pTemp) )
		{
			++ pTemp; 
		}
		pWTail = pTemp - 1; 
	}
	catch(...)
	{
		return false; 
	}
	return true; 
}

//�����հ�
bool AnalyseClassCallRelation::SkipSpace(char *pPos, char *pMenEnd, char *&pWHead)
{
	char *pTemp = pPos; 
	try
	{
		while(pTemp <= pMenEnd && ( *pTemp == ' '|| *pTemp == '\t' || *pTemp == '\r' || *pTemp == '\n'))
		{
			++ pTemp;
		}
		if (pTemp >  pMenEnd )
		{
			return false; 
		}
		pWHead = pTemp; 
	}
	catch(...)
	{
		return false; 
	}
	return true; 
}


//��ȡ����λ�õ�����
bool AnalyseClassCallRelation::GetLineHead(char *pPos, char *pMenStart, char *pMenEnd, char *&pLineHead)
{
	char *pTemp = pPos; 
	try
	{
		while(pTemp>pMenStart && *pTemp !='\n' )
		{
			-- pTemp;
		}
		pLineHead = ( pTemp <= pMenStart )?pMenStart:pTemp+1; 
	}
	catch(...)
	{
		pLineHead = pMenStart; 
	}
	return true; 
}

 //��ȡposλ�����ڵĵ��� 
bool AnalyseClassCallRelation::GetWord(char *pPos, char *pMenStart, char *pMenEnd, char *&pWHead, char *&pWTail)
{
	char *pTemp = pPos; 
	try
	{
		while(pTemp>=pMenStart && *pTemp != ' '&& *pTemp != '\t' && *pTemp != '\r' 
			&& *pTemp != '\n' && *pTemp != '(')
		{
			-- pTemp;
		}
		pWHead = (pTemp>=pMenStart)? pTemp+1:pMenStart;   //���ʵĿ�ʼ
		pTemp = pPos;
		while(pTemp <= pMenEnd && *pTemp != ' '&& *pTemp != '\t' && *pTemp != '\r'
			&& *pTemp != '\n'  && *pTemp != ')'  && *pTemp != ',')
		{
			++ pTemp;
		}
		pWTail = (pTemp <= pMenEnd)?pTemp-1:pMenEnd; 
	}
	catch(...)
	{
		return false; 
	}
	return true; 
}
//��ȡһ���ڴ���ַ�����ʽ	
string AnalyseClassCallRelation::GetStr(char *pStart, char *pEnd)
{
	char Temp = *pEnd; 
	*pEnd = 0; 
	string strRet; 
	strRet += pStart; 
	strRet += Temp; 
	*pEnd = Temp; 
	return strRet;  
}
//���ļ��ҵ������ඨ��
bool AnalyseClassCallRelation::FindClassObjDefine(const char *pObj, const string &strFileName, string &strObjDefine,  bool bStartHeaderFile )
{
	string strSuffix = (bStartHeaderFile)?string(".h"):string(".cpp"); 
	string strFilePath =  m_strProjectPath + string("\\") + strFileName + strSuffix;
	//ʵ��ͷ�ļ������ַ��� 
	char *pBuf; 
	if (! GetFile(strFilePath, pBuf, 1))
	{
		return false; 
	}
	unsigned int dwFileLen = strlen(pBuf); 
	//�ں����ҵ���Ӧ������ 
	char *pWHead = NULL; 
	char *pWTail = NULL;
	char *pTemp = pBuf;
	while((pTemp = strstr(pTemp, pObj)) != NULL )
	{		
		GetPureWord(pTemp, pBuf, pBuf+dwFileLen, pWHead,  pWTail);
		string strCompleteWord =  GetStr(pWHead, pWTail);
		string strObj = pObj; 
		if( GetStr(pWHead, pWTail) == strObj || strCompleteWord == strObj+';' )
		{
			//�ҵ��ඨ��
			strObjDefine = LastWord(pTemp, pBuf); 
			return true; 
		}
		pTemp += strlen(pObj);
	}
	//��ʵ���ļ��в���

	return false; 
}


//��ָ��λ��֮��  Ѱ���ඨ���
bool AnalyseClassCallRelation::FindClassObjDefine(const char *pObj, char *pPos, char *pMenStart, char *pMenEnd, string &strObjDefine)
{
	//�ں����ҵ���Ӧ������ 
	char *pTemp = pPos; 
	char *pSearch = NULL;
	char cTemp = *pTemp;
	*pTemp = 0; 
	while((pSearch = strrstr(pMenStart, pObj)) != NULL )
	{
		
		char *pWHead = NULL; 
		char *pWTail = NULL; 
		GetWord(pSearch, pMenStart, pMenEnd, pWHead,  pWTail);
		string strCompleteWord =  GetStr(pWHead, pWTail);
		string strObj = pObj; 
		if( GetStr(pWHead, pWTail) == strObj || strCompleteWord == strObj+';' )
		{
			//�ҵ��ඨ��
			strObjDefine = LastWord(pSearch, pMenStart); 
			*pTemp = cTemp;
			return true; 
		}
		*pTemp = cTemp;
		cTemp = *pSearch;
		*pSearch = 0; 
		pTemp = pSearch;  
	}
	*pTemp = cTemp;
	return  false; 
}
//���Զ�λ��ǰһ������
string AnalyseClassCallRelation::LastWord(char *pPos, char *pMenStart) 
{
	//���ҿհ� ���ҷǿհ�  ���ҿհ�  
	try
	{
		char *pTemp = pPos; 
		//���ҵ��հ�
		while(pTemp>=pMenStart && *pTemp != ' '&& *pTemp != '\t' && *pTemp != '\n')
		{
			--pTemp; 
		}
		if(pTemp < pMenStart)
		{
			return string(); 
		}
		//���ҵ��ǿհ�
		while(pTemp>=pMenStart && (*pTemp == ' '||*pTemp == '\t' || *pTemp == '\n'))
		{
			--pTemp; 
		}
		if(pTemp < pMenStart)
		{
			return string(); 
		}
		char *pWTail = pTemp; 
		//���ҿհ�
		while(pTemp>=pMenStart && *pTemp != ' '&& *pTemp != '\t' && *pTemp != '\n')
		{
			--pTemp; 
		}
		char *pWHead = (pTemp < pMenStart)?pMenStart:pTemp+1; 
		return GetStr(pWHead, pWTail); 
	}
	catch(...)
	{
		return string();
	}
}
//��ȡ�ļ�����
bool AnalyseClassCallRelation::GetFile(const string &strPath, char *&pOuter, unsigned char byMenID)
{
	FILE *fp; 
	fp = fopen(strPath.c_str(), "rb");
	if ( ! fp)
	{
		return false; 
	}
	fseek(fp, 0, SEEK_END); 
	unsigned dwFileLen = ftell(fp);
	rewind(fp);
	char *pBuf = GetMemory(dwFileLen+1, byMenID); 
	fread(pBuf, dwFileLen, 1, fp);
	fclose(fp); 
	pBuf[dwFileLen] = 0; 
	pOuter = pBuf; 
	return true; 
}

//��ȡ�ļ�����
bool AnalyseClassCallRelation::GetFile(const string &strPath, string &strContent,  unsigned char byMenID)
{
//	ifstream fin(strPath.c_str()); 
// 	if (! fin.is_open())
// 	{
// 		return false; 
// 	}
// 	string strLine; 
// 	while( getline(fin, strLine))
// 	{
// 		strContent +=  strLine + "\n";
// 	}
// 	fin.close();
	char *pBuf = NULL;
	if (! GetFile(strPath, pBuf, byMenID) )
	{
		return false; 
	}
	strContent = pBuf; 
	return true; 
}
//��ȡ�ļ��б�
bool AnalyseClassCallRelation::GetFileList(vector<ClassSource> &vFileList)
{
	HANDLE file;
	WIN32_FIND_DATA fileData;
//	wchar_t fn[1000];
	char szFilename[1024]; 
//	mbstowcs(fn, m_strProjectPath.c_str() ,999);
	string strFind = m_strProjectPath + string("\\*.*"); 
	file = FindFirstFile(strFind.c_str(), &fileData);

	ClassSource clsClassSource; 
	clsClassSource.Init(); 
	//�ҵ��ļ�����0  ���򷵻�-1
	while(FindNextFile(file, &fileData))
	{
		// ֻ����ͨ�ļ�������
		if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE))
		{
			continue; 
		}
		sprintf(szFilename, "%s", fileData.cFileName); 
		//�ж���ͷ�ļ� ����Դ�ļ�  �ж������Ƿ����
		//��ȡ��׺
		char *pTemp = szFilename + strlen(szFilename); 
		while( pTemp > szFilename && *pTemp-- != '.') ; 
		if (pTemp == szFilename)
		{
			continue; 
		}
		++ pTemp ; 
		string strSuffix(pTemp); 
		if (strSuffix == ".cpp")
		{
			clsClassSource.byHasImplementation = 1; 
		}
		else if(strSuffix == ".h" || strSuffix == ".hpp")
		{
			clsClassSource.byHasHeader = 1;
		}
		else
			continue;
		string strFile(szFilename, pTemp-szFilename);
		clsClassSource.strName = strFile; 
		vector<ClassSource>::iterator iter = vFileList.begin(); 
		while (iter != vFileList.end() )
		{
			if (iter->strName == strFile)
			{
				iter->byHasHeader = clsClassSource.byHasHeader?1:iter->byHasHeader; 
				iter->byHasImplementation = clsClassSource.byHasImplementation?1:iter->byHasImplementation; 
				break;; 
			}
			++ iter; 
		}
		if (iter == vFileList.end() )
		{
			vFileList.push_back(clsClassSource);
			clsClassSource.Init(); 
		}
	}
	return true; 
}
//��ͷ�ļ���ȡ����Ϣ
bool AnalyseClassCallRelation::GetClassFromHeader(const string& strHeaderFile, vector<string> &vClass)
{
	string strFilePath = m_strProjectPath + '\\' +  strHeaderFile; 
	string strContent; 
	if (! GetFile(strFilePath, strContent))
	{
		return false; 
	}
	vClass.clear(); 
	string::size_type pos = 0; 
	string::size_type WStartPos = 0; 
	string::size_type WEndPos = 0; 
	while ( (pos = strContent.find("class ", pos)) != string::npos )
	{
		pos += strlen("class "); 
		//�ҵ����� �Ƿ�̳� public CDialog�� ��Dialog ��Ѱenum IDD = 
		string::size_type newLinePos = strContent.find("\n", pos); 
		string::size_type PosTemp; 
		string strClassName = GetWord(strContent, pos, newLinePos, PosTemp);
		string strTemp ; 
		while ( (strTemp = GetWord(strContent, PosTemp, newLinePos, PosTemp)) != string())
		{
			if (string("CDialog") == strTemp )
			{
				//�̳� CDialog  
				PosTemp = strContent.find("enum", PosTemp); 
				if (PosTemp == string::npos)
				{
					break; 
				}
				newLinePos = strContent.find('\n', PosTemp);
				//��ȡ��ԴID 
				strTemp = GetIDFromLine(strContent, PosTemp, newLinePos); 
				if (strTemp == string())
				{
					break; 
				}
				ClassResourceRelation clsRelation; 
				clsRelation.struClass.strName = strClassName; 
				clsRelation.struClass.strHeaderFile = strFilePath; 
				clsRelation.struResource.strName = strTemp; 
				AddClassResRelation(clsRelation); 
				vClass.push_back(strClassName); 
			}
		}
	}
	if (vClass.empty())
	{
		return false; 
	}
	return true; 
}
 //�ж����Ƿ�Ϊclw�жԻ����� 
bool AnalyseClassCallRelation::IsClwClass(const string &strClassName)
{
	vector<ClassResourceRelation>::iterator iter = m_vRelationData.begin(); 
	while(iter != m_vRelationData.end() )
	{
		if (iter->struClass.strName == strClassName )
		{
			return true; 
		}
		++iter; 
	}
	return false; 
}

//�ӱ��������ݿ⣬��ȡ����������
bool AnalyseClassCallRelation::GetItemFromBeCallData(const string &strBeCallItem, int * pItem)
{
	vector<ClassItem>::iterator iter = m_vBeCallData.begin(); 
	int i = 0; 
	while (iter != m_vBeCallData.end() )
	{
		if (iter->strClassName == strBeCallItem )
		{
			*pItem = i; 
			return true; 
		}
		++ iter; 
		i++; 
	}
	return false; 
}


char* AnalyseClassCallRelation::GetMemory(unsigned int dwSize, unsigned char byMenID)
{
	unsigned int * pdwNowMaxSize = NULL; 
	char **pMen =NULL; 
	switch (byMenID)
	{
	case 0:
		pdwNowMaxSize = &m_dwNowMaxSize; 
		pMen = &m_pMen; 
		break; 
	case 1:
		pdwNowMaxSize = &m_dwNowMaxSize2; 
		pMen = &m_pMen2; 
		break; 
	default:
		break;
	}
	if (dwSize <= *pdwNowMaxSize)
	{
		return *pMen; 
	}
	if (*pMen)
	{
		delete []*pMen; 
		*pMen = NULL; 
	}
	*pMen = new char[dwSize]; 
	*pdwNowMaxSize = dwSize; 
	return *pMen; 
}

//ʹ��ID��ѯ
bool AnalyseClassCallRelation::QuerybyID(const string &str, deque<ClassResourceRelation> &qCallPath)
{
	return Query(str, false, qCallPath); 
}
//ʹ��Class��ѯ	
bool AnalyseClassCallRelation::QuerybyClass(const string &str, deque<ClassResourceRelation> &qCallPath)
{
	return Query(str, true, qCallPath); 
}
//��ѯ
bool AnalyseClassCallRelation::Query(const string &strQuery, bool bQueryByClass, deque<ClassResourceRelation> &qCallPath)
{
	
	string strTemp; 
	int iItem ; 
	//��ѯ���ݿ��Ƿ��и���Դ ʹ�����ͻ�ID
	if ( !GetInfoFromClw(strQuery,bQueryByClass, strTemp) )
	{
		return false; 
	}
	string strQueryTemp = bQueryByClass?strQuery:strTemp; 
	//ʹ��������ѯ ���ù�ϵ���ݿ��Ƿ��и���
	if ( ! GetItemFromBeCallData(strQueryTemp, &iItem) )
	{
		return false; 
	}

	//���ҵ���Ӧ����Դ  ��ѯ���ù�ϵ
	queue< deque<ClassResourceRelation> > vTotalPathTemp; 
	queue< deque<ClassResourceRelation> > vTotalPathFinish; 
	deque<ClassResourceRelation> qPath; 
	
	ClassResourceRelation clsCallRes;
	ClassItem &clsClassItem = m_vBeCallData[iItem];
	clsCallRes.struClass.strName  = clsClassItem.strClassName; 
	clsCallRes.struResource.strName = GetIDbyClass(clsCallRes.struClass.strName); 
	qPath.push_front(clsCallRes);   //���ڵ�ѹ��·��	
	//�жϽӿں��Ƿ���
	vector<CallItem>::size_type item = clsClassItem.vecItems.size(); 
	if ( item == 0 )
	{
		vTotalPathFinish.push(qPath); //��·��ѹ����ɶ���
	}
	else 
	{
		for (int i = 0; i < item; i++)
		{
			clsCallRes.struClass.strName = clsClassItem.vecItems[i].strClassName; 
			clsCallRes.struResource.strName = GetIDbyClass(clsCallRes.struClass.strName);	
			qPath.push_front(clsCallRes); 
			vTotalPathTemp.push(qPath); //��·��ѹ����ʱ����
		}	
	}	
	while (! vTotalPathTemp.empty())
	{
		//�ҳ�û��������ɵ�·��
		//������һ��Ԫ��
		qPath = vTotalPathTemp.front(); 
		vTotalPathTemp.pop();
		//��·���ĵ�һ�����Ƿ�����һ�û�н������·�����зֱ�ѹ��
		ClassResourceRelation &clsClassRes = qPath.front();
		//��ѯ���������� û�еĻ� ѹ����ɶ���
		if (! GetItemFromBeCallData(clsClassRes.struClass.strName, &iItem) )
		{
			//�����ļ��ǰ��յ����߼���  �������û�����ļ����뱻�������ݣ� ����һ�㶼�Ǵ���������� 
			vTotalPathFinish.push(qPath); //��·��ѹ����ɶ���
			continue; 
		}
		ClassResourceRelation clsCallRes;
		ClassItem &clsClassItem = m_vBeCallData[iItem];
		clsCallRes.struClass.strName  = clsClassItem.strClassName; 
		clsCallRes.struResource.strName = GetIDbyClass(clsCallRes.struClass.strName);	
		vector<CallItem>::size_type item = clsClassItem.vecItems.size();
		if ( item == 0 )
		{
			vTotalPathFinish.push(qPath); //��·��ѹ����ɶ���
		}
		else 
		{
			for (int i = 0; i < item; i++)
			{
				clsCallRes.struClass.strName = clsClassItem.vecItems[i].strClassName; 
				clsCallRes.struResource.strName = GetIDbyClass(clsCallRes.struClass.strName);		
				qPath.push_front(clsCallRes); 
				vTotalPathTemp.push(qPath); //��·��ѹ����ʱ����
			}	
		}	
	}
	qCallPath = vTotalPathFinish.front(); 
}

string AnalyseClassCallRelation::GetIDbyClass(const string &strClass)
{
	string strtemp; 
	return GetInfoFromClw(strClass, true, strtemp)?strtemp:string();
}
string AnalyseClassCallRelation::GetClassbyID(const string &strClass)
{
	string strtemp; 
	return GetInfoFromClw(strClass, false, strtemp)?strtemp:string();
}

bool AnalyseClassCallRelation::GetInfoFromClw(const string &strClass,  bool bGetIDByClass, string &strOut)
{
	vector<ClassResourceRelation>::iterator iter = m_vRelationData.begin();
	while (iter != m_vRelationData.end() )
	{
		string strCmp = (bGetIDByClass)?iter->struClass.strName:iter->struResource.strName; 
		if (strCmp == strClass)
		{
			strOut = (bGetIDByClass)?iter->struResource.strName:iter->struClass.strName; 
			return true; 
		}
		++ iter; 
	}
	return false; 
}


string AnalyseClassCallRelation::GetWord(string &strContent, string::size_type StartPos, string::size_type EndPos,string::size_type &RetWEndPos)
{
	string::size_type pos = StartPos; 
	string::size_type WStartPos = 0; 
	string::size_type WEndPos = 0; 
	//���ҵ�һ���ǿյ��ַ�λ��
	WStartPos = strContent.find_first_not_of (" \n\r", StartPos);
	if ( WEndPos == string::npos || WStartPos> EndPos)
	{
		return string(); 
	}
	//���ҵ��ʵĽ�β
	WEndPos = strContent.find_first_of (" \n\r", WStartPos);
	if ( WEndPos == string::npos || WEndPos > EndPos )
	{
		return string(); 
	}
	RetWEndPos = WEndPos; 
	return  strContent.substr(WStartPos, WEndPos-WStartPos); 
}

 //��enum { IDD = IDD_XXXX}������л�ȡID 
string AnalyseClassCallRelation::GetIDFromLine(string &strContent, string::size_type StartPos, string::size_type EndPos)
{
	string::size_type pos = 0; 
	string strTemp;
	pos = strContent.find('}', StartPos); 
	if (pos == string::npos || pos > EndPos)
	{
		return string(); 
	}
	pos = strContent.find('=', StartPos); 
	if (pos == string::npos || pos > EndPos)
	{
		return string(); 
	}
	string::size_type WStartPos = 0; 
	string::size_type WEndPos = 0; 
	WStartPos = strContent.find_first_not_of (" \t", pos+1);
	//���ҵ��ʵĽ�β
	WEndPos = strContent.find_first_of (" }\t", WStartPos);
	return  strContent.substr(WStartPos, WEndPos-WStartPos); 
}


bool AnalyseClassCallRelation::AddClassResRelation(ClassResourceRelation& clsRelation)
{
	//��ȡ��ԴID 
	vector<ClassResourceRelation>::iterator iter = m_vRelationData.begin(); 
	while (iter != m_vRelationData.end() )
	{
		if (iter->struClass.strName == clsRelation.struClass.strName && iter->struResource.strName == clsRelation.struResource.strName )
		{
			*iter = clsRelation; 
			return true; 
		}
		++ iter; 
	}
	m_vRelationData.push_back(clsRelation); 
	return true; 
}