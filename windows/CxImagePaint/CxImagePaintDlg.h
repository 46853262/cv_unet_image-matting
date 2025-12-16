#pragma once
#include "PaintCanvas.h"

class CCxImagePaintDlg : public CDialogEx
{
public:
    CCxImagePaintDlg(CWnd* pParent = nullptr);

    enum { IDD = IDD_CXIMAGEPAINT_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedOpen();
    afx_msg void OnBnClickedSave();
    afx_msg void OnBnClickedColor();
    afx_msg void OnBnClickedEraser();
    afx_msg void OnBnClickedZoomIn();
    afx_msg void OnBnClickedZoomOut();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    DECLARE_MESSAGE_MAP()

private:
    void UpdateBrushLabels();
    void SyncBrushFromUI();

private:
    HICON m_hIcon;
    CPaintCanvas m_canvas;
    BrushSettings m_brush;
    CSliderCtrl m_sizeSlider;
    CSliderCtrl m_opacitySlider;
    CSliderCtrl m_hardnessSlider;
    CSliderCtrl m_spacingSlider;
};
