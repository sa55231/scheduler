#include "pch.h"
#include "PropertyGridDateProperty.h"

IMPLEMENT_DYNAMIC(CPropertyGridDateProperty, CMFCPropertyGridProperty)


CPropertyGridDateProperty::CPropertyGridDateProperty(const CString& strName, const COleDateTime& time,
    LPCTSTR lpszDescr, DWORD_PTR dwData, bool useTime):
	CMFCPropertyGridProperty(strName, time, lpszDescr, dwData),useTime(useTime)
{
}

CPropertyGridDateProperty::~CPropertyGridDateProperty()
{

}

void CPropertyGridDateProperty::FormatTime()
{
	COleDateTime time(m_varValue);
	ASSERT(time.GetStatus() == COleDateTime::valid);
	if (useTime)
	{
		formattedDateTimeString = time.Format(_T("%R"));
	}
	else
	{
		formattedDateTimeString = time.Format(_T("%Y %b %d"));//  date::format("%Y %b %d", seconds).c_str();
	}
}

COleDateTime CPropertyGridDateProperty::GetTime()
{
	COleDateTime tmpTime(m_varValue);
	ASSERT(tmpTime.GetStatus() == COleDateTime::valid);
	return tmpTime;
}

CWnd* CPropertyGridDateProperty::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	
	CDateTimeCtrl* dateTimeCtrl = new CDateTimeCtrl();

	DWORD dwEditStyle = WS_CHILD | WS_VISIBLE;
	if (useTime)
	{
		dwEditStyle |= DTS_TIMEFORMAT;
	}
	dateTimeCtrl->Create(dwEditStyle, rectEdit, m_pWndList, AFX_PROPLIST_ID_INPLACE);
	dateTimeCtrl->SetFont(m_pWndList->GetFont());	
	COleDateTime tt(m_varValue);
	ASSERT(tt.GetStatus() == COleDateTime::valid);
	dateTimeCtrl->SetTime(tt);

	if (useTime)
	{
		dateTimeCtrl->SetFormat(_T("HH:mm"));
	}
	else
	{
		dateTimeCtrl->SetFormat(_T("yyyy MMM dd"));
	}
	dateTimeCtrl->ShowWindow(SW_SHOWNOACTIVATE);
	bDefaultFormat = TRUE;

	return dateTimeCtrl;
}
BOOL CPropertyGridDateProperty::OnUpdateValue()
{
	TRACE("OnUpdateValue var: %ld\n", m_varValue.llVal);
	ASSERT_VALID(this);

	if (m_pWndInPlace == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	auto oldTimePoint = m_varValue;
	CDateTimeCtrl* dateTimeCtrl = (CDateTimeCtrl*)m_pWndInPlace;

	COleDateTime tmpTime;
	dateTimeCtrl->GetTime(tmpTime);
	ASSERT(tmpTime.GetStatus() == COleDateTime::valid);
	m_varValue = tmpTime;

	if (oldTimePoint != m_varValue)
	{
		m_pWndList->OnPropertyChanged(this);
	}
	TRACE("AFTER ON PROPERTY CHANGED var: %ld\n", m_varValue.llVal);

	return TRUE;
}
CString CPropertyGridDateProperty::FormatProperty()
{
	FormatTime();
	return formattedDateTimeString;
}
