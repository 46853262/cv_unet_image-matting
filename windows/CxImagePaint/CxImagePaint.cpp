#include "stdafx.h"
#include "CxImagePaint.h"
#include "MainFrm.h"
#include "CxImagePaintDoc.h"
#include "CxImagePaintView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCxImagePaintApp, CWinApp)
    ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
    ON_COMMAND(ID_FILE_SAVE, &CWinApp::OnFileSave)
    ON_COMMAND(ID_FILE_EXIT, &CCxImagePaintApp::OnAppExit)
END_MESSAGE_MAP()

CCxImagePaintApp::CCxImagePaintApp() noexcept : m_hRichEdit(nullptr)
{
    SetAppID(_T("CxImagePaint.App"));
}

CCxImagePaintApp theApp;

BOOL CCxImagePaintApp::InitInstance()
{
    INITCOMMONCONTROLSEX InitCtrls{ sizeof(InitCtrls), ICC_WIN95_CLASSES };
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    m_hRichEdit = AfxInitRichEdit2();

    CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CCxImagePaintDoc),
        RUNTIME_CLASS(CMainFrame),
        RUNTIME_CLASS(CCxImagePaintView));

    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);

    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    return TRUE;
}

int CCxImagePaintApp::ExitInstance()
{
    if (m_hRichEdit)
        FreeLibrary(m_hRichEdit);
    return CWinApp::ExitInstance();
}

void CCxImagePaintApp::OnAppExit()
{
    CWinApp::OnAppExit();
}
