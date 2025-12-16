#pragma once

class CCxImagePaintApp : public CWinApp
{
public:
    CCxImagePaintApp() noexcept;

    BOOL InitInstance() override;
    int ExitInstance() override;

    DECLARE_MESSAGE_MAP()
private:
    HINSTANCE m_hRichEdit;
};
