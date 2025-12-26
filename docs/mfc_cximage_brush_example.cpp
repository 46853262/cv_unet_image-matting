// Minimal MFC + CxImage sample for painting on a 32-bit PNG with brush settings
// This is illustrative sample code intended to be compiled inside an MFC MDI/SDI project.
// Requirements: CxImage added to the project include path and linked, MFC in a shared DLL.
// The code focuses on brush logic: opacity, hardness, size, spacing, eraser support, and zoomed preview.

#include "stdafx.h"
#include "PngPaintView.h"
#include <cmath>

IMPLEMENT_DYNCREATE(CPngPaintView, CView)

BEGIN_MESSAGE_MAP(CPngPaintView, CView)
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_PAINT()
    ON_COMMAND(ID_FILE_OPEN, &CPngPaintView::OnFileOpen)
    ON_COMMAND(ID_FILE_SAVE, &CPngPaintView::OnFileSave)
    ON_COMMAND(ID_VIEW_ZOOMIN, &CPngPaintView::OnZoomIn)
    ON_COMMAND(ID_VIEW_ZOOMOUT, &CPngPaintView::OnZoomOut)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, &CPngPaintView::OnUpdateZoomIn)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, &CPngPaintView::OnUpdateZoomOut)
END_MESSAGE_MAP()

CPngPaintView::CPngPaintView()
    : m_zoom(1.0), m_isPainting(false)
{
    // Defaults similar to Photoshop soft round brush
    m_brush.size = 32.0;
    m_brush.opacity = 0.75;     // 0..1
    m_brush.hardness = 0.3;     // 0..1
    m_brush.spacing = 0.15;     // percentage of size
    m_brush.color = RGB(0, 0, 0);
    m_brush.isEraser = false;
}

void CPngPaintView::OnFileOpen()
{
    CFileDialog dlg(TRUE, _T("png"), nullptr, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("PNG Files (*.png)|*.png||"));
    if (dlg.DoModal() != IDOK)
        return;

    if (!m_image.Load(dlg.GetPathName()))
    {
        AfxMessageBox(_T("Failed to load PNG."));
        return;
    }

    // Ensure 32-bit format for predictable alpha
    if (m_image.GetBpp() != 32)
        m_image.IncreaseBpp(32);

    Invalidate();
}

void CPngPaintView::OnFileSave()
{
    if (m_image.IsValid())
        m_image.Save(m_image.GetSourceName(), CXIMAGE_FORMAT_PNG);
}

void CPngPaintView::OnPaint()
{
    CPaintDC dc(this);
    if (!m_image.IsValid())
        return;

    const int w = m_image.GetWidth();
    const int h = m_image.GetHeight();

    CDC memDC;
    memDC.CreateCompatibleDC(&dc);

    // StretchDIBits respects SetStretchBltMode for quality. HALFTONE is slower but smoother.
    dc.SetStretchBltMode(HALFTONE);

    CxImage *src = &m_image;
    BITMAPINFO *pBI = (BITMAPINFO *)src->GetInfo();
    BYTE *pBits = src->GetBits();

    CSize dstSize((int)(w * m_zoom), (int)(h * m_zoom));
    StretchDIBits(dc.GetSafeHdc(),
                  0, 0, dstSize.cx, dstSize.cy,
                  0, 0, w, h,
                  pBits,
                  pBI,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

CPoint CPngPaintView::ScreenToImage(const CPoint &pt) const
{
    return CPoint((int)(pt.x / m_zoom), (int)(pt.y / m_zoom));
}

void CPngPaintView::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_image.IsValid())
        return;

    SetCapture();
    m_isPainting = true;
    m_lastPaintPos = ScreenToImage(point);
    ApplyBrush(m_lastPaintPos);
}

void CPngPaintView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_isPainting || !(nFlags & MK_LBUTTON) || !m_image.IsValid())
        return;

    CPoint imgPt = ScreenToImage(point);

    // Spacing control: apply brush stamps along the dragged line
    const double spacingPx = std::max(1.0, m_brush.size * m_brush.spacing);
    CPoint delta = imgPt - m_lastPaintPos;
    const double len = std::hypot((double)delta.x, (double)delta.y);

    if (len < spacingPx)
        return;

    const double stepX = delta.x / len * spacingPx;
    const double stepY = delta.y / len * spacingPx;

    double curX = m_lastPaintPos.x;
    double curY = m_lastPaintPos.y;

    for (double traveled = 0; traveled < len; traveled += spacingPx)
    {
        curX += stepX;
        curY += stepY;
        ApplyBrush(CPoint((int)curX, (int)curY));
    }

    m_lastPaintPos = imgPt;
}

void CPngPaintView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_isPainting)
    {
        ReleaseCapture();
        m_isPainting = false;
    }
}

// Utility to fetch/assign pixel with alpha
static inline void GetPixel32(const CxImage &img, int x, int y, BYTE *rgba)
{
    RGBQUAD q = img.GetPixelColor(x, y, true);
    rgba[0] = q.rgbBlue;
    rgba[1] = q.rgbGreen;
    rgba[2] = q.rgbRed;
    rgba[3] = q.rgbReserved; // alpha
}

static inline void SetPixel32(CxImage &img, int x, int y, const BYTE *rgba)
{
    RGBQUAD q{rgba[2], rgba[1], rgba[0], rgba[3]};
    img.SetPixelColor(x, y, q, true);
}

void CPngPaintView::ApplyBrush(const CPoint &center)
{
    if (!m_image.IsValid())
        return;

    const int w = m_image.GetWidth();
    const int h = m_image.GetHeight();

    const double radius = m_brush.size * 0.5;
    const double hardness = std::clamp(m_brush.hardness, 0.0, 1.0);
    const double opacity = std::clamp(m_brush.opacity, 0.0, 1.0);

    const int minX = max(0, (int)std::floor(center.x - radius));
    const int maxX = min(w - 1, (int)std::ceil(center.x + radius));
    const int minY = max(0, (int)std::floor(center.y - radius));
    const int maxY = min(h - 1, (int)std::ceil(center.y + radius));

    BYTE rgba[4];
    const BYTE r = GetRValue(m_brush.color);
    const BYTE g = GetGValue(m_brush.color);
    const BYTE b = GetBValue(m_brush.color);

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            const double dx = x - center.x + 0.5;
            const double dy = y - center.y + 0.5;
            const double dist = std::sqrt(dx * dx + dy * dy);
            if (dist > radius)
                continue;

            // Hardness remaps falloff; hardness 1.0 => flat top, 0 => smooth fade
            double t = dist / radius;
            double falloff;
            if (t <= hardness)
                falloff = 1.0;
            else
            {
                double softT = (t - hardness) / (1.0 - hardness);
                falloff = 1.0 - softT; // simple linear falloff; replace with smoother step if desired
            }

            double alpha = opacity * std::clamp(falloff, 0.0, 1.0);
            if (alpha <= 0.0)
                continue;

            GetPixel32(m_image, x, y, rgba);

            // Premultiplied alpha blend
            double dstA = rgba[3] / 255.0;
            double dstR = rgba[2] / 255.0;
            double dstG = rgba[1] / 255.0;
            double dstB = rgba[0] / 255.0;

            double srcA = alpha;
            double srcR = r / 255.0;
            double srcG = g / 255.0;
            double srcB = b / 255.0;

            if (m_brush.isEraser)
            {
                // Eraser reduces alpha only (Photoshop-like)
                double outA = dstA * (1.0 - srcA);
                rgba[3] = (BYTE)(outA * 255.0 + 0.5);
            }
            else
            {
                double outA = srcA + dstA * (1.0 - srcA);
                double outR = (srcR * srcA + dstR * dstA * (1.0 - srcA)) / (outA > 0 ? outA : 1);
                double outG = (srcG * srcA + dstG * dstA * (1.0 - srcA)) / (outA > 0 ? outA : 1);
                double outB = (srcB * srcA + dstB * dstA * (1.0 - srcA)) / (outA > 0 ? outA : 1);

                rgba[2] = (BYTE)(outR * 255.0 + 0.5);
                rgba[1] = (BYTE)(outG * 255.0 + 0.5);
                rgba[0] = (BYTE)(outB * 255.0 + 0.5);
                rgba[3] = (BYTE)(outA * 255.0 + 0.5);
            }

            SetPixel32(m_image, x, y, rgba);
        }
    }

    Invalidate(FALSE);
}

void CPngPaintView::OnZoomIn()
{
    m_zoom = min(8.0, m_zoom * 1.25);
    Invalidate(FALSE);
}

void CPngPaintView::OnZoomOut()
{
    m_zoom = max(0.05, m_zoom / 1.25);
    Invalidate(FALSE);
}

void CPngPaintView::OnUpdateZoomIn(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_image.IsValid() && m_zoom < 8.0);
}

void CPngPaintView::OnUpdateZoomOut(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_image.IsValid() && m_zoom > 0.05);
}

// Header (PngPaintView.h)
// class CPngPaintView : public CView
// {
// protected:
//     struct BrushSettings
//     {
//         double size;      // pixels
//         double opacity;   // 0..1
//         double hardness;  // 0..1
//         double spacing;   // 0..1 (percentage of size)
//         COLORREF color;
//         bool isEraser;
//     } m_brush;
//
//     CxImage m_image;
//     double m_zoom;
//     bool m_isPainting;
//     CPoint m_lastPaintPos;
//
//     DECLARE_DYNCREATE(CPngPaintView)
//     DECLARE_MESSAGE_MAP()
//
//     CPoint ScreenToImage(const CPoint &pt) const;
//     void ApplyBrush(const CPoint &center);
// public:
//     CPngPaintView();
//     virtual void OnPaint();
//     afx_msg void OnFileOpen();
//     afx_msg void OnFileSave();
//     afx_msg void OnZoomIn();
//     afx_msg void OnZoomOut();
//     afx_msg void OnUpdateZoomIn(CCmdUI *pCmdUI);
//     afx_msg void OnUpdateZoomOut(CCmdUI *pCmdUI);
//     afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//     afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//     afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
// };
//
// To integrate:
// 1. Add CxImage sources to the project; ensure CXIMAGE_SUPPORT_PNG and zlib are enabled.
// 2. Create an SDI app with a view class named CPngPaintView and paste the code above.
// 3. Map ID_FILE_OPEN/ID_FILE_SAVE and zoom commands via the resource editor or define your own IDs.
// 4. Provide UI (toolbar/ribbon) to switch brush.color, size, hardness, opacity, spacing, and m_brush.isEraser.
// 5. Because PNG is saved through m_image.Save(m_image.GetSourceName(), CXIMAGE_FORMAT_PNG), the edits persist and can be reopened later.
// 6. For better performance, consider caching a premultiplied buffer and only invalidating the dirty rects.
