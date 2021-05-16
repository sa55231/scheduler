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
    CRibbonDateTimeControl(UINT id, const CString& label, bool time = false);

   
    ~CRibbonDateTimeControl();
    virtual CSize GetRegularSize(CDC*) override;
    //virtual CSize GetIntermediateSize(CDC* pDC) override;
    //virtual CSize GetCompactSize(CDC* pDC) override;
    virtual void OnDraw(CDC* pDC) override;
    void CopyFrom(const CMFCRibbonBaseElement& s);    
    void OnAfterChangeRect(CDC* pDC);
    CTime GetTime();
    void SetTime(const CTime& time);
protected:

private:
    void Init();
private:
    DWORD m_dwStyle = 0;
    DWORD m_dwTimeStatus = GDT_VALID;
    int   m_iWidth = 100;
    BOOL  m_bHorz = TRUE;
    CTime m_time;
    CDateTimeCtrl* dateTimeCtrl = nullptr;
    CSize m_szMargin = {5,5};
    LONG m_lLabelLength = 0;
    LONG m_lControlLength = 0;
    CString m_strLabel;
    bool isTime = false;
};

