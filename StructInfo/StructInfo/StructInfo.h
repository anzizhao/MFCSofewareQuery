
// StructInfo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
//#include "HPR_Types.h"

// CStructInfoApp:
// See StructInfo.cpp for the implementation of this class
//

class CStructInfoApp : public CWinAppEx
{
public:
	CStructInfoApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CStructInfoApp theApp;