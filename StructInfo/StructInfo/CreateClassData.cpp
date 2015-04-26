
#include "stdafx.h"
#include "CreateClassData.h"
#include <assert.h>
#include <cstring>
//从s字符串中找出str字符串
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

//开发测试用
void AnalyseClassCallRelation::Test()
{
	m_strProjectPath = "B:\\TestData\\ClientDemo";    //分析目录
	m_strProjectName = "ClientDemo";  //项目名称
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
//项目信息
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

//目录是否存在
bool AnalyseClassCallRelation::IsFolderExist(const string  &strDirPath)
{
	//判断分析目录是否存在
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

//分析clw文件
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
	//寻找[DLG: 在寻找对应的[CLS:
	const string  DLGFLAG("[DLG:");  //dlg 资源标记
	const string  DLGCLASSFLAG("Class="); //dlg下的class
	const string  CLASSFLAG("[CLS:"); //class标记
	const string  HEADERFILEFLAG("HeaderFile="); //HeaderFile标记
	const string  IMPFILE("ImplementationFile="); //ImplementationFile标记
	while( (DlgPos = strContent.find(DLGFLAG, DlgPos)) != string::npos )
	{
		//提取资源ID 
		ClwResouceItem clsResItem; 
		string::size_type TempPos = 0; 
		TempPos = strContent.find(']', DlgPos); 
		assert(TempPos != string::npos);
		clsResItem.strName = strContent.substr(DlgPos+DLGFLAG.length(), TempPos-DlgPos-DLGFLAG.length());
		string::size_type TempPosS = 0;
		TempPos = strContent.find(DLGCLASSFLAG, DlgPos);
		assert(TempPos != string::npos);
		DlgPos += DLGFLAG.length();   //一个分析完成  准备下一个

		TempPosS = strContent.find_first_of(" \r\n", TempPos);  
		assert(TempPosS != string::npos);
		ClwClassItem clsClassItem; 
		clsClassItem.strName =  strContent.substr(TempPos+DLGCLASSFLAG.length(), TempPosS-TempPos-DLGCLASSFLAG.length());

		//寻找对应的class 格式固定先头文件 再是实现文件
		if (clsClassItem.strName == "?")
		{
			DlgPos += DLGFLAG.length();   //clw文件有问题  继续查找  后面可能推出函数报错
			continue; 
		}
		string strClass; 
		strClass = string("[CLS:")+clsClassItem.strName+string("]");
		TempPos = strContent.find(strClass);
		//clw不一定准确  有时候有资源  没有文件
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

		DlgPos += DLGFLAG.length();   //一个分析完成  准备下一个
	}
	return true; 
}
//开始分析   
bool AnalyseClassCallRelation::Analyse()
{
	//是否获取读取clw文件 
	//不从clw文件获取类与资源的关系
	/*
	if ( m_vRelationData.empty() )
	{
		GetClwFile(); 
	}
	*/
	//获取文件列表 
	vector<ClassSource> vFileList; 
	if (! GetFileList(vFileList))
	{
		return false; 
	}
	//先分配7K的内存用来存放文件
	GetMemory(40*1024);
	//对文件列表里文件做处理
	vector<string> vClass;
	vector<ClassSource>::iterator iter = vFileList.begin(); 
	FILE *fp; 
	fp = fopen("Test.data", "w"); 
	while (iter != vFileList.end() )
	{
		//获取头文件信息， 得出该文件需要分析的类 
		vClass.clear();
		string strFileName = iter->strName + ".h"; 

		DWORD dwFileStartTime = GetTickCount();  
		if (! GetClassFromHeader(strFileName, vClass) )
		{
			++ iter ;
			continue; 
		}
		//判断是否为clw中的类
		vector<string>::iterator strIter = vClass.begin(); 
		while (strIter != vClass.end() )
		{

//			if (IsClwClass(*strIter))
			{
				//是clw中的类，分析， 传入类实现文件 头文件和实现文件只有后缀区别
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
	//根据类 找到对应头文件和资源
	string strSourceFile;
	strSourceFile = m_strProjectPath + string("\\") + strFileName + string(".cpp");
	string strContent; 
	if (! GetFile(strSourceFile, strContent))
	{
		return ; 
	}
	//模式 查找.DoModal（）调用  ，找到对应的类  写入数据
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

			//被调用关系
			CallItem struCallItem ;  
			struCallItem.strClassName = strClassName;
			int iItem; 
			if ( GetItemFromBeCallData(strClass, &iItem) )
			{
				//数据库里有
				ClassItem &clsItem = m_vBeCallData[iItem];			
				clsItem.vecItems.push_back(struCallItem); 
			}
			else
			{
				//新的被调用关系
				ClassItem clsItem; 
				clsItem.strClassName = strClass; 
				clsItem.vecItems.push_back(struCallItem); 
				m_vBeCallData.push_back(clsItem); 
			}
		}
		pTemp = pWTail + 1; 
	}
	//分析非模式
	pTemp = pBuf; 
	char *pPos = NULL; 
	char *pnewLine = NULL;
	char *pCreateSign = "Create"; 
	while( (pTemp = strstr(pTemp, pCreateSign)) != NULL )
	{
		//判断后面一个是不是空格还（
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
			//在同一行 没有IDD字样  不符合 跳过这个
			pTemp += strlen(pCreateSign); 
			continue; 
		}
		//有一种情况是  m_subDVRNVRCfg.Create(CSubDlgDVRNVRCfg::IDD,this);
		//获取资源ID
		string strID; 
		bRet = GetWord(pPos, pBuf, pBuf+dwFileLen, pWHead, pWTail); 
		assert( bRet ); 
		strID = GetStr(pWHead, pWTail);
		//获取类对象
		//获取行首 
		GetLineHead(pPos, pBuf, pBuf+dwFileLen, pPos);
		SkipSpace(pPos, pBuf+dwFileLen, pPos);   //跳过空白
		bRet = GetPureWord(pPos, pBuf, pBuf+dwFileLen, pWHead, pWTail);
		assert( bRet );
		strClassObj = GetStr(pWHead, pWTail);
		if( FindClassObjDefine(strClassObj.c_str(), strFileName,  strClass) )
		{
			CallItem struBeCallItem ; 
			struBeCallItem.strClassName = strClass; 
			struCallClass.vecItems.push_back(struBeCallItem); 

			//被调用关系
			CallItem struCallItem ;  
			struCallItem.strClassName = strClassName;
			int iItem; 
			//从被调用数据库查看是否有该项记录
			if ( GetItemFromBeCallData(strClass, &iItem) )
			{
				//数据库里有
				ClassItem &clsItem = m_vBeCallData[iItem];
				//查看是否记录该调用关系
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
					//没有改调用记录 添加
					clsItem.vecItems.push_back(struCallItem); 
				}
				
			}
			else
			{
				//新的被调用关系
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
//获取pos位置所在的纯正的单词 
bool AnalyseClassCallRelation::GetPureWord(char *pPos, char *pMenStart, char *pMenEnd, char *&pWHead, char *&pWTail )
{
	char *pTemp = pPos; 
	try
	{
		////获取所在位置的行首
		//GetLineHead(pTemp, pMenStart, pMenEnd, pTemp);
		//SkipSpace(pTemp, pMenEnd, pTemp);   //跳过空白
		//开始位置
		while(pTemp>=pMenStart && *pTemp != ' '&& *pTemp != '\t' && *pTemp != '\r' 
			&& *pTemp != '\n' && *pTemp != '(')
		{
			-- pTemp;
		}

		//获取单词开始位置
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

//跳过空白
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


//获取所在位置的行首
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

 //获取pos位置所在的单词 
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
		pWHead = (pTemp>=pMenStart)? pTemp+1:pMenStart;   //单词的开始
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
//获取一段内存的字符串形式	
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
//从文件找到对象类定义
bool AnalyseClassCallRelation::FindClassObjDefine(const char *pObj, const string &strFileName, string &strObjDefine,  bool bStartHeaderFile )
{
	string strSuffix = (bStartHeaderFile)?string(".h"):string(".cpp"); 
	string strFilePath =  m_strProjectPath + string("\\") + strFileName + strSuffix;
	//实现头文件查找字符串 
	char *pBuf; 
	if (! GetFile(strFilePath, pBuf, 1))
	{
		return false; 
	}
	unsigned int dwFileLen = strlen(pBuf); 
	//在函数找到对应的类型 
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
			//找到类定义
			strObjDefine = LastWord(pTemp, pBuf); 
			return true; 
		}
		pTemp += strlen(pObj);
	}
	//从实现文件中查找

	return false; 
}


//从指定位置之上  寻找类定义的
bool AnalyseClassCallRelation::FindClassObjDefine(const char *pObj, char *pPos, char *pMenStart, char *pMenEnd, string &strObjDefine)
{
	//在函数找到对应的类型 
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
			//找到类定义
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
//在自定位置前一个单词
string AnalyseClassCallRelation::LastWord(char *pPos, char *pMenStart) 
{
	//查找空白 查找非空白  查找空白  
	try
	{
		char *pTemp = pPos; 
		//查找到空白
		while(pTemp>=pMenStart && *pTemp != ' '&& *pTemp != '\t' && *pTemp != '\n')
		{
			--pTemp; 
		}
		if(pTemp < pMenStart)
		{
			return string(); 
		}
		//查找到非空白
		while(pTemp>=pMenStart && (*pTemp == ' '||*pTemp == '\t' || *pTemp == '\n'))
		{
			--pTemp; 
		}
		if(pTemp < pMenStart)
		{
			return string(); 
		}
		char *pWTail = pTemp; 
		//查找空白
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
//获取文件内容
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

//获取文件内容
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
//获取文件列表
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
	//找到文件返回0  否则返回-1
	while(FindNextFile(file, &fileData))
	{
		// 只对普通文件做处理
		if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE))
		{
			continue; 
		}
		sprintf(szFilename, "%s", fileData.cFileName); 
		//判读是头文件 还是源文件  判断名称是否存在
		//获取后缀
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
//从头文件获取类信息
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
		//找到类型 是否继承 public CDialog， 是Dialog 找寻enum IDD = 
		string::size_type newLinePos = strContent.find("\n", pos); 
		string::size_type PosTemp; 
		string strClassName = GetWord(strContent, pos, newLinePos, PosTemp);
		string strTemp ; 
		while ( (strTemp = GetWord(strContent, PosTemp, newLinePos, PosTemp)) != string())
		{
			if (string("CDialog") == strTemp )
			{
				//继承 CDialog  
				PosTemp = strContent.find("enum", PosTemp); 
				if (PosTemp == string::npos)
				{
					break; 
				}
				newLinePos = strContent.find('\n', PosTemp);
				//获取资源ID 
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
 //判断类是否为clw中对话框类 
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

//从被调用数据库，获取被调用类项
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

//使用ID查询
bool AnalyseClassCallRelation::QuerybyID(const string &str, deque<ClassResourceRelation> &qCallPath)
{
	return Query(str, false, qCallPath); 
}
//使用Class查询	
bool AnalyseClassCallRelation::QuerybyClass(const string &str, deque<ClassResourceRelation> &qCallPath)
{
	return Query(str, true, qCallPath); 
}
//查询
bool AnalyseClassCallRelation::Query(const string &strQuery, bool bQueryByClass, deque<ClassResourceRelation> &qCallPath)
{
	
	string strTemp; 
	int iItem ; 
	//查询数据库是否有该资源 使用类型或ID
	if ( !GetInfoFromClw(strQuery,bQueryByClass, strTemp) )
	{
		return false; 
	}
	string strQueryTemp = bQueryByClass?strQuery:strTemp; 
	//使用类名查询 调用关系数据库是否有该项
	if ( ! GetItemFromBeCallData(strQueryTemp, &iItem) )
	{
		return false; 
	}

	//能找到对应的资源  查询调用关系
	queue< deque<ClassResourceRelation> > vTotalPathTemp; 
	queue< deque<ClassResourceRelation> > vTotalPathFinish; 
	deque<ClassResourceRelation> qPath; 
	
	ClassResourceRelation clsCallRes;
	ClassItem &clsClassItem = m_vBeCallData[iItem];
	clsCallRes.struClass.strName  = clsClassItem.strClassName; 
	clsCallRes.struResource.strName = GetIDbyClass(clsCallRes.struClass.strName); 
	qPath.push_front(clsCallRes);   //将节点压入路径	
	//判断接口后是否还有
	vector<CallItem>::size_type item = clsClassItem.vecItems.size(); 
	if ( item == 0 )
	{
		vTotalPathFinish.push(qPath); //将路径压入完成队列
	}
	else 
	{
		for (int i = 0; i < item; i++)
		{
			clsCallRes.struClass.strName = clsClassItem.vecItems[i].strClassName; 
			clsCallRes.struResource.strName = GetIDbyClass(clsCallRes.struClass.strName);	
			qPath.push_front(clsCallRes); 
			vTotalPathTemp.push(qPath); //将路径压入临时队列
		}	
	}	
	while (! vTotalPathTemp.empty())
	{
		//找出没有搜索完成的路径
		//弹出第一个元素
		qPath = vTotalPathTemp.front(); 
		vTotalPathTemp.pop();
		//该路径的第一个项是否还有下一项，没有进入完成路径，有分别压入
		ClassResourceRelation &clsClassRes = qPath.front();
		//查询被调用数据 没有的话 压入完成队列
		if (! GetItemFromBeCallData(clsClassRes.struClass.strName, &iItem) )
		{
			//分析文件是按照调用逻辑的  而且最后没给该文件加入被调用数据， 所以一般都是从这里结束的 
			vTotalPathFinish.push(qPath); //将路径压入完成队列
			continue; 
		}
		ClassResourceRelation clsCallRes;
		ClassItem &clsClassItem = m_vBeCallData[iItem];
		clsCallRes.struClass.strName  = clsClassItem.strClassName; 
		clsCallRes.struResource.strName = GetIDbyClass(clsCallRes.struClass.strName);	
		vector<CallItem>::size_type item = clsClassItem.vecItems.size();
		if ( item == 0 )
		{
			vTotalPathFinish.push(qPath); //将路径压入完成队列
		}
		else 
		{
			for (int i = 0; i < item; i++)
			{
				clsCallRes.struClass.strName = clsClassItem.vecItems[i].strClassName; 
				clsCallRes.struResource.strName = GetIDbyClass(clsCallRes.struClass.strName);		
				qPath.push_front(clsCallRes); 
				vTotalPathTemp.push(qPath); //将路径压入临时队列
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
	//查找第一个非空的字符位置
	WStartPos = strContent.find_first_not_of (" \n\r", StartPos);
	if ( WEndPos == string::npos || WStartPos> EndPos)
	{
		return string(); 
	}
	//查找单词的结尾
	WEndPos = strContent.find_first_of (" \n\r", WStartPos);
	if ( WEndPos == string::npos || WEndPos > EndPos )
	{
		return string(); 
	}
	RetWEndPos = WEndPos; 
	return  strContent.substr(WStartPos, WEndPos-WStartPos); 
}

 //从enum { IDD = IDD_XXXX}，语句中获取ID 
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
	//查找单词的结尾
	WEndPos = strContent.find_first_of (" }\t", WStartPos);
	return  strContent.substr(WStartPos, WEndPos-WStartPos); 
}


bool AnalyseClassCallRelation::AddClassResRelation(ClassResourceRelation& clsRelation)
{
	//提取资源ID 
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