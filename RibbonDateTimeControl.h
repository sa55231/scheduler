#pragma once
#include <afxbaseribbonelement.h>
class CRibbonDateTimeControl :
    public CMFCRibbonBaseElement
{
    DECLARE_DYNCREATE(CRibbonDateTimeControl)
protected:
    CRibbonDateTimeControl() 
    {
        Init();
    }
 public:
    CRibbonDateTimeControl(UINT id);

   
    ~CRibbonDateTimeControl();
    virtual CSize GetRegularSize(CDC*) override;
    //virtual CSize GetIntermediateSize(CDC* pDC) override;
    //virtual CSize GetCompactSize(CDC* pDC) override;
    virtual void OnDraw(CDC* pDC) override;
    void CopyFrom(const CMFCRibbonBaseElement& s);
    
    void OnAfterChangeRect(CDC* pDC);
    
protected:

private:
    void Init();
private:
    UINT id = 0;
    DWORD m_dwStyle = 0;
    DWORD m_dwTimeStatus = GDT_VALID;
    int   m_iWidth = 100;
    BOOL  m_bHorz = TRUE;
    CTime m_time;
    CDateTimeCtrl* dateTimeCtrl = nullptr;
    CSize m_szMargin = {5,5};
};

