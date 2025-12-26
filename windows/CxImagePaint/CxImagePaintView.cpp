#include "stdafx.h"
#include "CxImagePaintView.h"
#include <cmath>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCxImagePaintView, CView)

BEGIN_MESSAGE_MAP(CCxImagePaintView, CView)
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_COMMAND(ID_FILE_OPEN, &CCxImagePaintView::OnFileOpen)
    ON_COMMAND(ID_FILE_SAVE, &CCxImagePaintView::OnFileSave)
    ON_COMMAND(ID_VIEW_ZOOMIN, &CCxImagePaintView::OnViewZoomIn)
    ON_COMMAND(ID_VIEW_ZOOMOUT, &CCxImagePaintView::OnViewZoomOut)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, &CCxImagePaintView::OnUpdateZoomIn)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, &CCxImagePaintView::OnUpdateZoomOut)
    ON_COMMAND(ID_TOOLS_BRUSH, &CCxImagePaintView::OnToolsBrush)
    ON_COMMAND(ID_TOOLS_ERASER, &CCxImagePaintView::OnToolsEraser)
    ON_COMMAND(ID_TOOLS_TOGGLE_ERASER, &CCxImagePaintView::OnToolsToggleEraser)
    ON_COMMAND(ID_TOOLS_INCREASE_SIZE, &CCxImagePaintView::OnToolsIncreaseSize)
    ON_COMMAND(ID_TOOLS_DECREASE_SIZE, &CCxImagePaintView::OnToolsDecreaseSize)
    ON_COMMAND(ID_TOOLS_INCREASE_OPACITY, &CCxImagePaintView::OnToolsIncreaseOpacity)
    ON_COMMAND(ID_TOOLS_DECREASE_OPACITY, &CCxImagePaintView::OnToolsDecreaseOpacity)
    ON_COMMAND(ID_TOOLS_INCREASE_HARDNESS, &CCxImagePaintView::OnToolsIncreaseHardness)
    ON_COMMAND(ID_TOOLS_DECREASE_HARDNESS, &CCxImagePaintView::OnToolsDecreaseHardness)
END_MESSAGE_MAP()

CCxImagePaintView::CCxImagePaintView() noexcept
    : m_zoom(1.0), m_isPainting(false)
{
    m_brush.size = 32.0;
    m_brush.opacity = 0.75;
    m_brush.hardness = 0.3;
    m_brush.spacing = 0.15;
    m_brush.color = RGB(0, 0, 0);
    m_brush.isEraser = false;
}

BOOL CCxImagePaintView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

void CCxImagePaintView::OnDraw(CDC* pDC)
{
    if (!m_image.IsValid())
        return;

    const int w = m_image.GetWidth();
    const int h = m_image.GetHeight();

    pDC->SetStretchBltMode(HALFTONE);

    BITMAPINFO* pBI = (BITMAPINFO*)m_image.GetInfo();
    BYTE* pBits = m_image.GetBits();
    CSize dstSize((int)(w * m_zoom), (int)(h * m_zoom));

    StretchDIBits(pDC->GetSafeHdc(),
        0, 0, dstSize.cx, dstSize.cy,
        0, 0, w, h,
        pBits,
        pBI,
        DIB_RGB_COLORS,
        SRCCOPY);
}

CPoint CCxImagePaintView::ScreenToImage(const CPoint& pt) const
{
    return CPoint((int)(pt.x / m_zoom), (int)(pt.y / m_zoom));
}

void CCxImagePaintView::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_image.IsValid())
        return;

    SetCapture();
    m_isPainting = true;
    m_lastPaintPos = ScreenToImage(point);
    ApplyBrush(m_lastPaintPos);
}

void CCxImagePaintView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_isPainting || !(nFlags & MK_LBUTTON) || !m_image.IsValid())
        return;

    CPoint imgPt = ScreenToImage(point);
    const double spacingPx = std::max(1.0, m_brush.size * m_brush.spacing);
    CPoint delta = imgPt - m_lastPaintPos;
    const double len = std::hypot((double)delta.x, (double)delta.y);

    if (len < spacingPx)
        return;

    const int steps = (int)(len / spacingPx);
    for (int i = 1; i <= steps; ++i)
    {
        double t = (spacingPx * i) / len;
        CPoint interp((int)(m_lastPaintPos.x + delta.x * t),
            (int)(m_lastPaintPos.y + delta.y * t));
        ApplyBrush(interp);
    }
    m_lastPaintPos = imgPt;
}

void CCxImagePaintView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_isPainting)
    {
        ReleaseCapture();
        m_isPainting = false;
    }
    CView::OnLButtonUp(nFlags, point);
}

void CCxImagePaintView::OnFileOpen()
{
    CFileDialog dlg(TRUE, _T("png"), nullptr, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("PNG Files (*.png)|*.png||"));
    if (dlg.DoModal() != IDOK)
        return;

    if (!m_image.Load(dlg.GetPathName()))
    {
        AfxMessageBox(_T("Failed to load PNG."));
        return;
    }

    if (m_image.GetBpp() != 32)
        m_image.IncreaseBpp(32);

    Invalidate();
}

void CCxImagePaintView::OnFileSave()
{
    if (m_image.IsValid())
        m_image.Save(m_image.GetSourceName(), CXIMAGE_FORMAT_PNG);
}

void CCxImagePaintView::OnViewZoomIn()
{
    m_zoom = std::min(8.0, m_zoom * 1.25);
    Invalidate();
}

void CCxImagePaintView::OnViewZoomOut()
{
    m_zoom = std::max(0.05, m_zoom / 1.25);
    Invalidate();
}

void CCxImagePaintView::OnUpdateZoomIn(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_zoom < 8.0);
}

void CCxImagePaintView::OnUpdateZoomOut(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_zoom > 0.05);
}

void CCxImagePaintView::OnToolsBrush()
{
    m_brush.isEraser = false;
}

void CCxImagePaintView::OnToolsEraser()
{
    m_brush.isEraser = true;
}

void CCxImagePaintView::OnToolsToggleEraser()
{
    m_brush.isEraser = !m_brush.isEraser;
}

void CCxImagePaintView::OnToolsIncreaseSize()
{
    m_brush.size = std::min(512.0, m_brush.size + 4.0);
}

void CCxImagePaintView::OnToolsDecreaseSize()
{
    m_brush.size = std::max(1.0, m_brush.size - 4.0);
}

void CCxImagePaintView::OnToolsIncreaseOpacity()
{
    m_brush.opacity = std::min(1.0, m_brush.opacity + 0.05);
}

void CCxImagePaintView::OnToolsDecreaseOpacity()
{
    m_brush.opacity = std::max(0.0, m_brush.opacity - 0.05);
}

void CCxImagePaintView::OnToolsIncreaseHardness()
{
    m_brush.hardness = std::min(1.0, m_brush.hardness + 0.05);
}

void CCxImagePaintView::OnToolsDecreaseHardness()
{
    m_brush.hardness = std::max(0.0, m_brush.hardness - 0.05);
}

void CCxImagePaintView::ApplyBrush(const CPoint& pt)
{
    if (!m_image.IsValid())
        return;

    const int radius = (int)std::ceil(m_brush.size * 0.5);
    const int diameter = radius * 2;

    for (int dy = -radius; dy <= radius; ++dy)
    {
        for (int dx = -radius; dx <= radius; ++dx)
        {
            int x = pt.x + dx;
            int y = pt.y + dy;

            if (x < 0 || y < 0 || x >= m_image.GetWidth() || y >= m_image.GetHeight())
                continue;

            double dist = std::sqrt((double)(dx * dx + dy * dy));
            if (dist > radius)
                continue;

            double norm = dist / radius;
            double falloff = 1.0;
            if (norm > m_brush.hardness)
            {
                double t = (norm - m_brush.hardness) / (1.0 - m_brush.hardness);
                falloff = 1.0 - t;
            }

            double alpha = m_brush.opacity * falloff;

            RGBQUAD* pixel = m_image.GetPixelAddress(x, y);
            if (!pixel)
                continue;

            BYTE srcA = pixel->rgbReserved;
            BYTE dstA;
            BYTE dstR;
            BYTE dstG;
            BYTE dstB;

            if (m_brush.isEraser)
            {
                double newAlpha = srcA * (1.0 - alpha);
                dstA = (BYTE)newAlpha;
                dstR = pixel->rgbRed;
                dstG = pixel->rgbGreen;
                dstB = pixel->rgbBlue;
            }
            else
            {
                BYTE brushR = GetRValue(m_brush.color);
                BYTE brushG = GetGValue(m_brush.color);
                BYTE brushB = GetBValue(m_brush.color);

                double invA = (1.0 - alpha);
                dstR = (BYTE)(brushR * alpha + pixel->rgbRed * invA);
                dstG = (BYTE)(brushG * alpha + pixel->rgbGreen * invA);
                dstB = (BYTE)(brushB * alpha + pixel->rgbBlue * invA);
                dstA = (BYTE)(255 * alpha + srcA * invA);
            }

            pixel->rgbRed = dstR;
            pixel->rgbGreen = dstG;
            pixel->rgbBlue = dstB;
            pixel->rgbReserved = dstA;
        }
    }

    Invalidate(FALSE);
}
