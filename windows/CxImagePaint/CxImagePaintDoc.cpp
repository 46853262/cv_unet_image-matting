#include "stdafx.h"
#include "CxImagePaintDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCxImagePaintDoc, CDocument)

BEGIN_MESSAGE_MAP(CCxImagePaintDoc, CDocument)
END_MESSAGE_MAP()

BOOL CCxImagePaintDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    return TRUE;
}

void CCxImagePaintDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // No document state because view owns the bitmap
    }
    else
    {
    }
}

#ifdef _DEBUG
void CCxImagePaintDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CCxImagePaintDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif
