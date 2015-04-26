#ifndef _CREATE_CLASS_DATA_H
#define _CREATE_CLASS_DATA_H

#include <vector>
#include <string>
#include <fstream>
#include <deque>
#include <queue>
using namespace std; 

//通用 调用信息
typedef struct _CallItem
{
	string strClassName;   //类名称
	string strFunction;    //调用函数名称
}CallItem; 

//类信息 
typedef struct _ClassItem
{
	string strClassName;   //调用类名称
	vector<CallItem> vecItems;   //调用信息
}ClassItem; 

//类信息 
typedef struct _ClwClassItem
{
	string strName; 
	string strHeaderFile; 
	string strImplementationFile; 
}ClwClassItem; 
//资源信息
typedef struct _ClwResouceItem
{
	string strName; 
}ClwResouceItem; 

//类和资源的关系
typedef struct _ClassResourceRelation
{
	ClwClassItem struClass; 
	ClwResouceItem struResource; 
}ClassResourceRelation;

//类源文件信息
typedef struct _ClassSource
{
	string strName;    //名称
	unsigned char  byHasHeader;   //该项是否有头文件
	unsigned char  byHasImplementation; //该项是否有实现文件
	void Init()
	{
		strName = ""; 
		byHasHeader  = 0; 
		byHasImplementation = 0; 
	}
}ClassSource; 

//分析类调用关系类
class AnalyseClassCallRelation
{
private:
	vector<ClassItem> m_vCallData;    //调用关系数据
	vector<ClassItem> m_vBeCallData;  //被调用关系数据 
	vector<ClassResourceRelation> m_vRelationData;  //类和资源的数据

private:
	string m_strProjectPath;    //分析目录
	string m_strProjectName;  //项目名称
public:

	AnalyseClassCallRelation(); 
	~AnalyseClassCallRelation(); 
	
	bool ProjectInfo(const string  &strProjectName, const string  &strProjectPath);// 项目信息
	bool Analyse();   //开始分析
	bool QuerybyID(const string &str, deque<ClassResourceRelation> &qCallPath);   //使用ID查询
	bool QuerybyClass(const string &str, deque<ClassResourceRelation> &qCallPath);   //使用Class查询
	void Test(); //开发测试用

protected:
	void Analyse(const string  &strFileName, const string &strClassName);    //分析一个对话框,
	bool Query(const string &str, bool bQueryByClass, deque<ClassResourceRelation> &qCallPath);  //查询
	bool FindClassObjDefine(const char *pObj, char *pPos, char *pMenStart, char *pMenEnd, string &strObjDefine); //从内存找到对象类定义
	bool FindClassObjDefine(const char *pObj, const string &strFilename, string &strObjDefine, bool bStartHeaderFile = true); //从文件找到对象类定义

	bool IsClwClass(const string &strClassName);  //判断类是否为clw中对话框类 
	bool IsFolderExist(const string  &strPath);  //目录是否存在
	bool GetFileList(vector<ClassSource> &vFileList); //获取文件列表
	bool GetClassFromHeader(const string& strHeaderFile, vector<string> &vClass);//从头文件获取类信息 对话框 资源ID 
	bool GetItemFromBeCallData(const string &strBeCallItem, int *pItem);  //从被调用数据库，获取被调用类项
	string GetIDbyClass(const string &strClass); 
	string GetClassbyID(const string &strClass); 
	bool GetInfoFromClw(const string &strClass,  bool bGetIDByClass, string &strOut); 
	bool GetBeCallItem(const string &strBeCallClass, ClassItem *&pItem);  //获取某项被调用类信息

	bool GetClwFile();  //分析clw文件
	bool AddClassResRelation(ClassResourceRelation& clsRelation); 
private:
	//字符串小工具集
	bool GetWord(char *pPos, char *pMenStart, char *pMenEnd, char *&pWHead, char *&pWTail);  //获取pos位置所在的单词 
	string GetStr(char *pStart, char *pEnd);               //获取一段内存的字符串形式
	string LastWord(char *pPos, char *pMenStart);     //从开始位置 获取上个单词
	bool SkipSpace(char *pPos, char *pMenEnd, char *&pWHead);     //跳过空白
	bool GetPureWord(char *pPos, char *pMenStart, char *pMenEnd, char *&pWHead, char *&pWTail );//获取pos位置所在的纯正的单词 
	bool GetLineHead(char *pPos, char *pMenStart, char *pMenEnd, char *&pLineHead); //获取所在位置的行首
	//从string的一段位置获取单词
	string GetWord(string &strContent, string::size_type StartPos, string::size_type EndPos, string::size_type &WEndPos);
	string GetIDFromLine(string &strContent, string::size_type StartPos,  string::size_type EndPos); //从enum { IDD = xxxx}，语句中获取ID 
	//
	bool GetFile(const string &strPath, string &strContent,  unsigned char byMenID = 0);   //获取文件内容
	bool GetFile(const string &strPath, char *&pOuter,  unsigned char byMenID = 0);  //获取文件内容


	char* GetMemory(unsigned int dwSize, unsigned char byMenID = 0); 
	char* m_pMen;
	char* m_pMen2; 
	unsigned int m_dwNowMaxSize; 
	unsigned int m_dwNowMaxSize2; 
};

#endif 