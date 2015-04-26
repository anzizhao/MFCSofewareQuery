#ifndef _CREATE_CLASS_DATA_H
#define _CREATE_CLASS_DATA_H

#include <vector>
#include <string>
#include <fstream>
#include <deque>
#include <queue>
using namespace std; 

//ͨ�� ������Ϣ
typedef struct _CallItem
{
	string strClassName;   //������
	string strFunction;    //���ú�������
}CallItem; 

//����Ϣ 
typedef struct _ClassItem
{
	string strClassName;   //����������
	vector<CallItem> vecItems;   //������Ϣ
}ClassItem; 

//����Ϣ 
typedef struct _ClwClassItem
{
	string strName; 
	string strHeaderFile; 
	string strImplementationFile; 
}ClwClassItem; 
//��Դ��Ϣ
typedef struct _ClwResouceItem
{
	string strName; 
}ClwResouceItem; 

//�����Դ�Ĺ�ϵ
typedef struct _ClassResourceRelation
{
	ClwClassItem struClass; 
	ClwResouceItem struResource; 
}ClassResourceRelation;

//��Դ�ļ���Ϣ
typedef struct _ClassSource
{
	string strName;    //����
	unsigned char  byHasHeader;   //�����Ƿ���ͷ�ļ�
	unsigned char  byHasImplementation; //�����Ƿ���ʵ���ļ�
	void Init()
	{
		strName = ""; 
		byHasHeader  = 0; 
		byHasImplementation = 0; 
	}
}ClassSource; 

//��������ù�ϵ��
class AnalyseClassCallRelation
{
private:
	vector<ClassItem> m_vCallData;    //���ù�ϵ����
	vector<ClassItem> m_vBeCallData;  //�����ù�ϵ���� 
	vector<ClassResourceRelation> m_vRelationData;  //�����Դ������

private:
	string m_strProjectPath;    //����Ŀ¼
	string m_strProjectName;  //��Ŀ����
public:

	AnalyseClassCallRelation(); 
	~AnalyseClassCallRelation(); 
	
	bool ProjectInfo(const string  &strProjectName, const string  &strProjectPath);// ��Ŀ��Ϣ
	bool Analyse();   //��ʼ����
	bool QuerybyID(const string &str, deque<ClassResourceRelation> &qCallPath);   //ʹ��ID��ѯ
	bool QuerybyClass(const string &str, deque<ClassResourceRelation> &qCallPath);   //ʹ��Class��ѯ
	void Test(); //����������

protected:
	void Analyse(const string  &strFileName, const string &strClassName);    //����һ���Ի���,
	bool Query(const string &str, bool bQueryByClass, deque<ClassResourceRelation> &qCallPath);  //��ѯ
	bool FindClassObjDefine(const char *pObj, char *pPos, char *pMenStart, char *pMenEnd, string &strObjDefine); //���ڴ��ҵ������ඨ��
	bool FindClassObjDefine(const char *pObj, const string &strFilename, string &strObjDefine, bool bStartHeaderFile = true); //���ļ��ҵ������ඨ��

	bool IsClwClass(const string &strClassName);  //�ж����Ƿ�Ϊclw�жԻ����� 
	bool IsFolderExist(const string  &strPath);  //Ŀ¼�Ƿ����
	bool GetFileList(vector<ClassSource> &vFileList); //��ȡ�ļ��б�
	bool GetClassFromHeader(const string& strHeaderFile, vector<string> &vClass);//��ͷ�ļ���ȡ����Ϣ �Ի��� ��ԴID 
	bool GetItemFromBeCallData(const string &strBeCallItem, int *pItem);  //�ӱ��������ݿ⣬��ȡ����������
	string GetIDbyClass(const string &strClass); 
	string GetClassbyID(const string &strClass); 
	bool GetInfoFromClw(const string &strClass,  bool bGetIDByClass, string &strOut); 
	bool GetBeCallItem(const string &strBeCallClass, ClassItem *&pItem);  //��ȡĳ���������Ϣ

	bool GetClwFile();  //����clw�ļ�
	bool AddClassResRelation(ClassResourceRelation& clsRelation); 
private:
	//�ַ���С���߼�
	bool GetWord(char *pPos, char *pMenStart, char *pMenEnd, char *&pWHead, char *&pWTail);  //��ȡposλ�����ڵĵ��� 
	string GetStr(char *pStart, char *pEnd);               //��ȡһ���ڴ���ַ�����ʽ
	string LastWord(char *pPos, char *pMenStart);     //�ӿ�ʼλ�� ��ȡ�ϸ�����
	bool SkipSpace(char *pPos, char *pMenEnd, char *&pWHead);     //�����հ�
	bool GetPureWord(char *pPos, char *pMenStart, char *pMenEnd, char *&pWHead, char *&pWTail );//��ȡposλ�����ڵĴ����ĵ��� 
	bool GetLineHead(char *pPos, char *pMenStart, char *pMenEnd, char *&pLineHead); //��ȡ����λ�õ�����
	//��string��һ��λ�û�ȡ����
	string GetWord(string &strContent, string::size_type StartPos, string::size_type EndPos, string::size_type &WEndPos);
	string GetIDFromLine(string &strContent, string::size_type StartPos,  string::size_type EndPos); //��enum { IDD = xxxx}������л�ȡID 
	//
	bool GetFile(const string &strPath, string &strContent,  unsigned char byMenID = 0);   //��ȡ�ļ�����
	bool GetFile(const string &strPath, char *&pOuter,  unsigned char byMenID = 0);  //��ȡ�ļ�����


	char* GetMemory(unsigned int dwSize, unsigned char byMenID = 0); 
	char* m_pMen;
	char* m_pMen2; 
	unsigned int m_dwNowMaxSize; 
	unsigned int m_dwNowMaxSize2; 
};

#endif 