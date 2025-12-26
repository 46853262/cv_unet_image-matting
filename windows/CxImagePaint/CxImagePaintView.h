#pragma once
#include "CxImage/ximage.h"

struct BrushSettings
{
    double size;
    double opacity;   // 0..1
    double hardness;  // 0..1
    double spacing;   // 0..1 (percentage of diameter)
    COLORREF color;
    bool isEraser;
};

class CCxImagePaintView : public CView
{
protected:
    CCxImagePaintView() noexcept;
    DECLARE_DYNCREATE(CCxImagePaintView)

public:
    virtual void OnDraw(CDC* pDC);
    BOOL PreCreateWindow(CREATESTRUCT& cs) override;

protected:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnViewZoomIn();
    afx_msg void OnViewZoomOut();
    afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
    afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
    afx_msg void OnToolsBrush();
    afx_msg void OnToolsEraser();
    afx_msg void OnToolsToggleEraser();
    afx_msg void OnToolsIncreaseSize();
    afx_msg void OnToolsDecreaseSize();
    afx_msg void OnToolsIncreaseOpacity();
    afx_msg void OnToolsDecreaseOpacity();
    afx_msg void OnToolsIncreaseHardness();
    afx_msg void OnToolsDecreaseHardness();
    DECLARE_MESSAGE_MAP()

private:
    CxImage m_image;
    double m_zoom;
    bool m_isPainting;
    CPoint m_lastPaintPos;
    BrushSettings m_brush;

    CPoint ScreenToImage(const CPoint& pt) const;
    void ApplyBrush(const CPoint& pt);
};
