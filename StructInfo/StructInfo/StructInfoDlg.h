
// StructInfoDlg.h : header file
//

#pragma once
#include "afxwin.h"

#define RC_BUF_SIZE (3*1024*1024)
#define SAVEFILE    ("Config.data")
#include "CreateClassData.h"
// CStructInfoDlg dialog
class CStructInfoDlg : public CDialog
{
// Construction
public:
	CStructInfoDlg(CWnd* pParent = NULL);	// standard constructor
	~CStructInfoDlg(); 
// Dialog Data
	enum { IDD = IDD_STRUCTINFO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg); 
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CString m_csStructName;
	DWORD m_dwStructSize;
	BOOL m_byQueryType;
	vector<string> m_vTitleID; 
	vector<string> m_vPorjectPath; 
	vector<string> m_vPorjectName; 


protected:
	BOOL QueryStructSize();
	BOOL QueryDemo();
	BOOL Title2ResourceID(); 
	BOOL ID2Path(); 
	BOOL Class2Path(); 
	BOOL Title2Path(); 



protected:
	void ReadConfigFile(char *pFile);  //��ȡ�����ļ�  ��ʱ������  ��ʼ�����������ļ�
	void ReadPathDataFile(char *pFile); //��ȡ�����ļ�
	void WritePathDataFile(char *pFile); //���������ļ�
	void AddProjectPath(); 
	void ToCreateAnylseData();  //�������������ݿ�  
	BOOL QueryPath(unsigned char byType=1, CString csInput = CString());  //��ѯ·��  1-ͨ��ID��ѯ 2-ͨ������
	BOOL ReadRCFile();
	char* ToReadRC(); 
	BOOL GetIDfromTitle(vector<string> &vID);    //�ӱ����ȡ��ԴID



private:
//�ַ����߼�
	BOOL WordInLine(char *pWord, char *&pLHead, char *&pLTail);   //�������ڵ��� 
	BOOL IsWordInLine(char *pWord, char *&pLHead, char *&pLTail);     //�����Ƿ��ڸ���
	BOOL LastLine(char *pPos, char *&pLHead, char *&pLTail);     //��һ��
	string GetWord(char *pPos);    //�ӿ�ʼλ�� ��ȡ���� 
	
private:
	char *m_pRcBuf; 
	AnalyseClassCallRelation m_clsAnaylse; 
	char m_szProjectName[256]; 
	char m_szProjectPath[512]; 
	BOOL m_BUpdateData; 

protected:
	CString m_csInput;
	CString m_csOutput;
	CComboBox m_cmbPorjectName;
	CComboBox m_cmbPorjectPath;

public:
	afx_msg void onEditChangeProjectPath();
	afx_msg void OnClose();
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButAdd();
	afx_msg void onEditChangeProjectName();
	afx_msg void OnCbnSelchangeComboProjectPath();
	afx_msg void OnCbnSelchangeComboProjectName();
};
