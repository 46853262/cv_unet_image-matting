#pragma once
#include "CxImage/ximage.h"

struct BrushSettings
{
    double size;     // pixels
    double opacity;  // 0..1
    double hardness; // 0..1
    double spacing;  // 0..1 (percentage of diameter)
    COLORREF color;
    bool isEraser;
};

class CPaintCanvas : public CStatic
{
    DECLARE_DYNAMIC(CPaintCanvas)
public:
    CPaintCanvas();

    bool LoadFromFile(LPCTSTR path);
    bool SaveToFile(LPCTSTR path);
    void SetBrush(const BrushSettings& brush);
    BrushSettings GetBrush() const { return m_brush; }
    void SetZoom(double zoom);
    double GetZoom() const { return m_zoom; }
    CString GetCurrentPath() const { return m_currentPath; }

protected:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()

private:
    void ApplyBrush(const CPoint& pt);
    CPoint ScreenToImage(const CPoint& pt) const;
    void EnsureAlpha();

private:
    CxImage m_image;
    CString m_currentPath;
    double m_zoom;
    bool m_isPainting;
    CPoint m_lastPaintPos;
    BrushSettings m_brush;
};
