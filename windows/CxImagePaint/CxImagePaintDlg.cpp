#include "stdafx.h"
#include "CxImagePaint.h"
#include "CxImagePaintDlg.h"
#include <afxdlgs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCxImagePaintDlg::CCxImagePaintDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CXIMAGEPAINT_DIALOG, pParent)
{
    m_hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    m_brush.size = 32.0;
    m_brush.opacity = 0.75;
    m_brush.hardness = 0.3;
    m_brush.spacing = 0.15;
    m_brush.color = RGB(0, 0, 0);
    m_brush.isEraser = false;
}

void CCxImagePaintDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCxImagePaintDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_OPEN, &CCxImagePaintDlg::OnBnClickedOpen)
    ON_BN_CLICKED(IDC_BTN_SAVE, &CCxImagePaintDlg::OnBnClickedSave)
    ON_BN_CLICKED(IDC_BTN_COLOR, &CCxImagePaintDlg::OnBnClickedColor)
    ON_BN_CLICKED(IDC_CHK_ERASER, &CCxImagePaintDlg::OnBnClickedEraser)
    ON_BN_CLICKED(IDC_BTN_ZOOMIN, &CCxImagePaintDlg::OnBnClickedZoomIn)
    ON_BN_CLICKED(IDC_BTN_ZOOMOUT, &CCxImagePaintDlg::OnBnClickedZoomOut)
    ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL CCxImagePaintDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_canvas.SubclassDlgItem(IDC_CANVAS, this);
    m_canvas.SetBrush(m_brush);

    m_sizeSlider.Attach(GetDlgItem(IDC_SLIDER_SIZE)->m_hWnd);
    m_sizeSlider.SetRange(1, 256);
    m_sizeSlider.SetTicFreq(10);
    m_sizeSlider.SetPos((int)m_brush.size);

    m_opacitySlider.Attach(GetDlgItem(IDC_SLIDER_OPACITY)->m_hWnd);
    m_opacitySlider.SetRange(0, 100);
    m_opacitySlider.SetPos((int)(m_brush.opacity * 100));

    m_hardnessSlider.Attach(GetDlgItem(IDC_SLIDER_HARDNESS)->m_hWnd);
    m_hardnessSlider.SetRange(0, 100);
    m_hardnessSlider.SetPos((int)(m_brush.hardness * 100));

    m_spacingSlider.Attach(GetDlgItem(IDC_SLIDER_SPACING)->m_hWnd);
    m_spacingSlider.SetRange(5, 200);
    m_spacingSlider.SetPos((int)(m_brush.spacing * 100));

    CheckDlgButton(IDC_CHK_ERASER, m_brush.isEraser ? BST_CHECKED : BST_UNCHECKED);

    UpdateBrushLabels();
    return TRUE;
}

void CCxImagePaintDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CCxImagePaintDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CCxImagePaintDlg::OnBnClickedOpen()
{
    CFileDialog dlg(TRUE, _T("png"), nullptr, OFN_FILEMUSTEXIST, _T("PNG Files (*.png)|*.png||"), this);
    if (dlg.DoModal() == IDOK)
    {
        if (!m_canvas.LoadFromFile(dlg.GetPathName()))
        {
            AfxMessageBox(_T("无法打开 PNG 图像"));
        }
    }
}

void CCxImagePaintDlg::OnBnClickedSave()
{
    if (!m_canvas.GetCurrentPath().IsEmpty())
    {
        if (!m_canvas.SaveToFile(m_canvas.GetCurrentPath()))
            AfxMessageBox(_T("保存失败"));
        return;
    }

    CFileDialog dlg(FALSE, _T("png"), _T("painting.png"), OFN_OVERWRITEPROMPT, _T("PNG Files (*.png)|*.png||"), this);
    if (dlg.DoModal() == IDOK)
    {
        if (!m_canvas.SaveToFile(dlg.GetPathName()))
            AfxMessageBox(_T("保存失败"));
    }
}

void CCxImagePaintDlg::OnBnClickedColor()
{
    SyncBrushFromUI();
    CColorDialog dlg(m_brush.color, CC_FULLOPEN, this);
    if (dlg.DoModal() == IDOK)
    {
        m_brush.color = dlg.GetColor();
        m_brush.isEraser = false;
        CheckDlgButton(IDC_CHK_ERASER, BST_UNCHECKED);
        m_canvas.SetBrush(m_brush);
    }
}

void CCxImagePaintDlg::OnBnClickedEraser()
{
    SyncBrushFromUI();
    m_brush.isEraser = (IsDlgButtonChecked(IDC_CHK_ERASER) == BST_CHECKED);
    m_canvas.SetBrush(m_brush);
}

void CCxImagePaintDlg::OnBnClickedZoomIn()
{
    m_canvas.SetZoom(m_canvas.GetZoom() * 1.25);
}

void CCxImagePaintDlg::OnBnClickedZoomOut()
{
    m_canvas.SetZoom(m_canvas.GetZoom() / 1.25);
}

void CCxImagePaintDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    UNREFERENCED_PARAMETER(nSBCode);
    UNREFERENCED_PARAMETER(nPos);
    UNREFERENCED_PARAMETER(pScrollBar);

    SyncBrushFromUI();
    UpdateBrushLabels();
    m_canvas.SetBrush(m_brush);

    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CCxImagePaintDlg::UpdateBrushLabels()
{
    CString text;
    text.Format(_T("%d px"), m_sizeSlider.GetPos());
    SetDlgItemText(IDC_LBL_SIZE, text);

    text.Format(_T("%d%%"), m_opacitySlider.GetPos());
    SetDlgItemText(IDC_LBL_OPACITY, text);

    text.Format(_T("%d%%"), m_hardnessSlider.GetPos());
    SetDlgItemText(IDC_LBL_HARDNESS, text);

    text.Format(_T("%d%%"), m_spacingSlider.GetPos());
    SetDlgItemText(IDC_LBL_SPACING, text);
}

void CCxImagePaintDlg::SyncBrushFromUI()
{
    m_brush.size = m_sizeSlider.GetPos();
    m_brush.opacity = m_opacitySlider.GetPos() / 100.0;
    m_brush.hardness = m_hardnessSlider.GetPos() / 100.0;
    m_brush.spacing = m_spacingSlider.GetPos() / 100.0;
    m_brush.isEraser = (IsDlgButtonChecked(IDC_CHK_ERASER) == BST_CHECKED);
}
