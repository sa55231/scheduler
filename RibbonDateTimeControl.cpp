#include "pch.h"
#include "RibbonDateTimeControl.h"

IMPLEMENT_DYNCREATE(CRibbonDateTimeControl, CMFCRibbonBaseElement)

static const int nDefaultComboHeight = 150;
static const int nDefaultWidth = 108;


CRibbonDateTimeControl::CRibbonDateTimeControl(UINT id, const CString& label, bool time):
	m_strLabel(label),isTime(time)
{
	m_nID = id;
	Init();
}

CRibbonDateTimeControl::~CRibbonDateTimeControl()
{
	if (dateTimeCtrl)
	{
		dateTimeCtrl->DestroyWindow();
		delete dateTimeCtrl;
		dateTimeCtrl = nullptr;
	}
}

void CRibbonDateTimeControl::Init()
{
	m_dwStyle = WS_CHILD | WS_VISIBLE;
	m_iWidth = 100;
	m_bHorz = TRUE;
	m_dwTimeStatus = GDT_VALID;
	m_time = CTime::GetCurrentTime();	
	//SetButtonMode();
}

CTime CRibbonDateTimeControl::GetTime()
{
	if (dateTimeCtrl && dateTimeCtrl->GetSafeHwnd() != NULL)
	{
		dateTimeCtrl->GetTime(m_time);
	}
	return m_time;
}
void CRibbonDateTimeControl::SetTime(const CTime& time)
{
	m_time = time;
	if (dateTimeCtrl && dateTimeCtrl->GetSafeHwnd() != NULL)
	{
		dateTimeCtrl->SetTime(&m_time);
	}
}

void CRibbonDateTimeControl::CopyFrom(const CMFCRibbonBaseElement& s)
{
	CMFCRibbonBaseElement::CopyFrom(s);
	CRibbonDateTimeControl& src = (CRibbonDateTimeControl&)s;

	if (dateTimeCtrl)
	{
		dateTimeCtrl->DestroyWindow();
		delete dateTimeCtrl;
		dateTimeCtrl = nullptr;
	}

	m_lLabelLength = src.m_lLabelLength;
	m_lControlLength = src.m_lControlLength;
	m_strLabel = src.m_strLabel;
	isTime = src.isTime;
	m_time = src.m_time;

}

CSize CRibbonDateTimeControl::GetRegularSize(CDC* pDC)
{
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	int nTextHeight = tm.tmHeight;
	if ((nTextHeight % 2) != 0)
	{
		nTextHeight++;
	}	
	int cy = nTextHeight + m_szMargin.cy + tm.tmExternalLeading;
	int cx =  pDC->GetTextExtent(m_strLabel).cx;
	m_lLabelLength = cx;

	m_lControlLength= pDC->GetTextExtent(_T("9999/MMMM/99  ")).cx;
	cx += m_lControlLength;
	if (GetGlobalData()->GetRibbonImageScale() > 1.)
	{
		cx = (int)(.5 + GetGlobalData()->GetRibbonImageScale() * cx);
	}

	return CSize(cx,cy);
}

void CRibbonDateTimeControl::OnDraw(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	if (m_rect.IsRectEmpty())
	{
		return;
	}
	
	CRect rectEdit = m_rect;

	int cx = m_lControlLength;
	if (GetGlobalData()->GetRibbonImageScale() > 1.)
	{
		cx = (int)(.5 + GetGlobalData()->GetRibbonImageScale() * cx);
	}

	rectEdit.right -= cx;
	pDC->DrawText(m_strLabel, rectEdit, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);	
	//CMFCVisualManager::GetInstance()->GetRibbonEditBackgroundColor(nullptr, false, true, false);
}

void CRibbonDateTimeControl::OnAfterChangeRect(CDC* pDC)
{
	CMFCRibbonBaseElement::OnAfterChangeRect(pDC);

	if (m_rect.IsRectEmpty())
	{
		if (dateTimeCtrl->GetSafeHwnd() != NULL)
		{
			dateTimeCtrl->ShowWindow(SW_HIDE);
		}

		if (dateTimeCtrl->GetSafeHwnd() != NULL)
		{
			dateTimeCtrl->ShowWindow(SW_HIDE);
		}

		return;
	}

	CRect rectCommandOld ;
	m_Location = RibbonElementSingleInGroup;
	//m_rectCommand = m_rect;

	if (dateTimeCtrl == nullptr)
	{
		CWnd* pWndParent = GetParentWnd();

		
		dateTimeCtrl = new CDateTimeCtrl();
		CRect rectBtn = GetRect();
		CRect originalRect = rectBtn;
		pWndParent->ClientToScreen(&rectBtn);

		int x = rectBtn.left;
		int y = rectBtn.bottom;
		DWORD dwEditStyle = WS_CHILD | WS_VISIBLE;
		if (isTime)
		{
			dwEditStyle |= DTS_TIMEFORMAT;			
		}		
		dateTimeCtrl->Create(dwEditStyle, CRect(0,0,0,0), pWndParent, m_nID);
		dateTimeCtrl->SetFont(GetTopLevelRibbonBar()->GetFont());
		dateTimeCtrl->SetTime(&m_time);

		if (isTime)
		{
			dateTimeCtrl->SetFormat(_T("HH:mm"));
		}
		else
		{
			dateTimeCtrl->SetFormat(_T("yyyy/MMM/dd"));
		}
	}


	if (!dateTimeCtrl->IsWindowVisible())
	{
		CRect rectEdit = m_rect;

		int cx = m_lControlLength;
		if (GetGlobalData()->GetRibbonImageScale() > 1.)
		{
			cx = (int)(.5 + GetGlobalData()->GetRibbonImageScale() * cx);
		}

		rectEdit.left = rectEdit.right - cx;

		//rectEdit.DeflateRect(m_szMargin.cx, m_szMargin.cy);

		dateTimeCtrl->SetWindowPos(NULL, rectEdit.left, rectEdit.top, rectEdit.Width(), rectEdit.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
		dateTimeCtrl->ShowWindow(SW_SHOWNOACTIVATE);
	}
}
