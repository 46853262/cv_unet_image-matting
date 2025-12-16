#include "stdafx.h"
#include "CxImagePaint.h"
#include "CxImagePaintDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCxImagePaintApp, CWinApp)
END_MESSAGE_MAP()

CCxImagePaintApp::CCxImagePaintApp() noexcept
    : m_hRichEdit(nullptr)
{
    SetAppID(_T("CxImagePaint.DialogApp"));
}

CCxImagePaintApp theApp;

BOOL CCxImagePaintApp::InitInstance()
{
    INITCOMMONCONTROLSEX InitCtrls{ sizeof(InitCtrls), ICC_WIN95_CLASSES | ICC_BAR_CLASSES };
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    m_hRichEdit = AfxInitRichEdit2();

    CCxImagePaintDlg dlg;
    m_pMainWnd = &dlg;
    dlg.DoModal();

    return FALSE;
}

int CCxImagePaintApp::ExitInstance()
{
    if (m_hRichEdit)
        FreeLibrary(m_hRichEdit);
    return CWinApp::ExitInstance();
}
