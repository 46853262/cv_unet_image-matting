#include "stdafx.h"
#include "PaintCanvas.h"
#include <cmath>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CPaintCanvas, CStatic)

BEGIN_MESSAGE_MAP(CPaintCanvas, CStatic)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CPaintCanvas::CPaintCanvas()
    : m_zoom(1.0)
    , m_isPainting(false)
{
    m_brush.size = 32.0;
    m_brush.opacity = 0.75;
    m_brush.hardness = 0.3;
    m_brush.spacing = 0.15;
    m_brush.color = RGB(0, 0, 0);
    m_brush.isEraser = false;
}

bool CPaintCanvas::LoadFromFile(LPCTSTR path)
{
    if (!m_image.Load(path, CXIMAGE_FORMAT_PNG))
        return false;

    if (m_image.GetBpp() != 32)
        m_image.IncreaseBpp(32);
    EnsureAlpha();

    m_currentPath = path;
    Invalidate(FALSE);
    return true;
}

bool CPaintCanvas::SaveToFile(LPCTSTR path)
{
    if (!m_image.IsValid())
        return false;
    m_currentPath = path;
    return m_image.Save(path, CXIMAGE_FORMAT_PNG);
}

void CPaintCanvas::SetBrush(const BrushSettings& brush)
{
    m_brush = brush;
}

void CPaintCanvas::SetZoom(double zoom)
{
    m_zoom = std::max(0.05, std::min(8.0, zoom));
    Invalidate(FALSE);
}

void CPaintCanvas::EnsureAlpha()
{
    if (!m_image.IsValid())
        return;

    if (!m_image.AlphaIsValid())
        m_image.AlphaCreate();
}

CPoint CPaintCanvas::ScreenToImage(const CPoint& pt) const
{
    return CPoint((int)(pt.x / m_zoom), (int)(pt.y / m_zoom));
}

void CPaintCanvas::OnPaint()
{
    CPaintDC dc(this);

    CRect rcClient;
    GetClientRect(&rcClient);

    if (!m_image.IsValid())
    {
        dc.FillSolidRect(rcClient, RGB(30, 30, 30));
        dc.SetTextColor(RGB(220, 220, 220));
        dc.SetBkMode(TRANSPARENT);
        dc.DrawText(_T("Open a 32-bit PNG to start painting"), -1, rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        return;
    }

    const int w = m_image.GetWidth();
    const int h = m_image.GetHeight();
    const CSize dstSize((int)(w * m_zoom), (int)(h * m_zoom));

    dc.SetStretchBltMode(HALFTONE);

    BITMAPINFO* pBI = (BITMAPINFO*)m_image.GetInfo();
    BYTE* pBits = m_image.GetBits();

    StretchDIBits(dc.GetSafeHdc(),
        0, 0, dstSize.cx, dstSize.cy,
        0, 0, w, h,
        pBits,
        pBI,
        DIB_RGB_COLORS,
        SRCCOPY);
}

BOOL CPaintCanvas::OnEraseBkgnd(CDC* pDC)
{
    UNREFERENCED_PARAMETER(pDC);
    return TRUE;
}

void CPaintCanvas::OnLButtonDown(UINT nFlags, CPoint point)
{
    UNREFERENCED_PARAMETER(nFlags);
    if (!m_image.IsValid())
        return;

    SetCapture();
    m_isPainting = true;
    m_lastPaintPos = ScreenToImage(point);
    ApplyBrush(m_lastPaintPos);
}

void CPaintCanvas::OnMouseMove(UINT nFlags, CPoint point)
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

void CPaintCanvas::OnLButtonUp(UINT nFlags, CPoint point)
{
    UNREFERENCED_PARAMETER(nFlags);
    UNREFERENCED_PARAMETER(point);
    if (m_isPainting)
    {
        ReleaseCapture();
        m_isPainting = false;
    }
}

void CPaintCanvas::ApplyBrush(const CPoint& pt)
{
    if (!m_image.IsValid())
        return;

    const int radius = (int)std::ceil(m_brush.size * 0.5);

    for (int dy = -radius; dy <= radius; ++dy)
    {
        for (int dx = -radius; dx <= radius; ++dx)
        {
            const int x = pt.x + dx;
            const int y = pt.y + dy;

            if (x < 0 || y < 0 || x >= m_image.GetWidth() || y >= m_image.GetHeight())
                continue;

            const double dist = std::sqrt((double)(dx * dx + dy * dy));
            if (dist > radius)
                continue;

            double norm = dist / radius;
            double falloff = 1.0;
            if (norm > m_brush.hardness)
            {
                const double t = (norm - m_brush.hardness) / (1.0 - m_brush.hardness);
                falloff = 1.0 - t;
            }

            const double alpha = m_brush.opacity * falloff;

            RGBQUAD* pixel = m_image.GetPixelAddress(x, y);
            if (!pixel)
                continue;

            const BYTE srcA = pixel->rgbReserved;
            BYTE dstA;
            BYTE dstR;
            BYTE dstG;
            BYTE dstB;

            if (m_brush.isEraser)
            {
                const double newAlpha = srcA * (1.0 - alpha);
                dstA = (BYTE)newAlpha;
                dstR = pixel->rgbRed;
                dstG = pixel->rgbGreen;
                dstB = pixel->rgbBlue;
            }
            else
            {
                const BYTE brushR = GetRValue(m_brush.color);
                const BYTE brushG = GetGValue(m_brush.color);
                const BYTE brushB = GetBValue(m_brush.color);

                const double invA = (1.0 - alpha);
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
