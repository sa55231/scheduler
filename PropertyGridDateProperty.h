#pragma once
#include <afxpropertygridctrl.h>

#include <chrono>

class CPropertyGridDateProperty :
    public CMFCPropertyGridProperty
{
    DECLARE_DYNAMIC(CPropertyGridDateProperty)
public:
    CPropertyGridDateProperty(const CString& strName, const COleDateTime& time,
        LPCTSTR lpszDescr, DWORD_PTR dwData, bool useTime = false);
    virtual ~CPropertyGridDateProperty();

    virtual CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);
    virtual BOOL OnUpdateValue();
    virtual CString FormatProperty();

protected:
    void FormatTime();
public:
    COleDateTime GetTime();
private:
    bool useTime = false;
    CString formattedDateTimeString;
};

