#pragma once

class CCxImagePaintDoc : public CDocument
{
protected:
    DECLARE_DYNCREATE(CCxImagePaintDoc)

public:
    BOOL OnNewDocument() override;
    void Serialize(CArchive& ar) override;

#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext& dc) const override;
#endif
};
