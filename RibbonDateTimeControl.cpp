#include "pch.h"
#include "RibbonDateTimeControl.h"


class CRibbonDateTimeMenuButton : public CMFCRibbonButton
{
	DECLARE_DYNCREATE(CRibbonDateTimeMenuButton)
public:
	CRibbonDateTimeMenuButton() {}
	CRibbonDateTimeMenuButton(CWnd* pWndParent, int x, int y)
	{	
	}
	
	void OnAfterChangeRect(CDC* pDC)
	{
		CWnd* pWndParent = GetParentWnd();

		DWORD dwEditStyle = WS_CHILD | WS_TABSTOP;
		dateTimeCtrl = new CDateTimeCtrl();
		CRect rectBtn = GetRect();
		CRect originalRect = rectBtn;
		pWndParent->ClientToScreen(&rectBtn);

		int x = rectBtn.left;
		int y = rectBtn.bottom;

		dateTimeCtrl->Create(dwEditStyle, originalRect, pWndParent, -1);

		//dateTimeCtrl->SetWindowPos(NULL, rectEdit.left, rectEdit.top, rectEdit.Width(), rectEdit.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

		dateTimeCtrl->ShowWindow(SW_SHOWNOACTIVATE);
	}
	void GetClientRect(LPRECT rect)
	{
		if (dateTimeCtrl)
		{
			dateTimeCtrl->GetClientRect(rect);
		}		
		
	}
	~CRibbonDateTimeMenuButton()
	{
		if (dateTimeCtrl)
		{
			dateTimeCtrl->DestroyWindow();
			delete dateTimeCtrl;
			dateTimeCtrl = nullptr;
		}
	}
private:
	//CMFCToolBarDateTimeCtrl ctrl;
	CDateTimeCtrl* dateTimeCtrl = nullptr;
};
IMPLEMENT_DYNCREATE(CRibbonDateTimeMenuButton, CMFCRibbonButton)

IMPLEMENT_DYNCREATE(CRibbonDateTimeControl, CMFCRibbonBaseElement)

static const int nDefaultComboHeight = 150;
static const int nDefaultWidth = 108;

class RibbonComboboxButton : public CMFCRibbonButton
{
public:
	RibbonComboboxButton()
	{
		
		CMFCToolBarMenuButton a;

	}
};

CRibbonDateTimeControl::CRibbonDateTimeControl(UINT id):id(id)
{
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
	int cy = nTextHeight + 15;
	int cx = pDC->GetTextExtent(_T("Start date:")).cx;
	cx += pDC->GetTextExtent(_T("99/99/9999 99:99")).cx;

	
	cx += 10;
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

	pDC->DrawText(_T("Start date:"), m_rect, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

	if (dateTimeCtrl)
	{
		CMFCToolBarDateTimeCtrl dummy;
		dummy.m_nStyle |= DTS_RIGHTALIGN;		
		dummy.m_bImage = true;
		dummy.OnDraw(pDC, m_rect, nullptr);
		return;
	}


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

		DWORD dwEditStyle = WS_CHILD | WS_TABSTOP;
		dateTimeCtrl = new CDateTimeCtrl();
		CRect rectBtn = GetRect();
		CRect originalRect = rectBtn;
		pWndParent->ClientToScreen(&rectBtn);

		int x = rectBtn.left;
		int y = rectBtn.bottom;

		dateTimeCtrl->Create(dwEditStyle, CRect(0,0,0,0), pWndParent, id);
		dateTimeCtrl->SetFont(GetTopLevelRibbonBar()->GetFont());
		//dateTimeCtrl->SetWindowPos(NULL, rectEdit.left, rectEdit.top, rectEdit.Width(), rectEdit.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
	}


	if (!dateTimeCtrl->IsWindowVisible())
	{
		CRect rectEdit = m_rect;

		int cx = 100;
		if (GetGlobalData()->GetRibbonImageScale() > 1.)
		{
			cx = (int)(.5 + GetGlobalData()->GetRibbonImageScale() * cx);
		}

		rectEdit.left = rectEdit.right - cx;

		rectEdit.DeflateRect(m_szMargin.cx, m_szMargin.cy);

		dateTimeCtrl->SetWindowPos(NULL, rectEdit.left, rectEdit.top, rectEdit.Width(), rectEdit.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
		dateTimeCtrl->ShowWindow(SW_SHOWNOACTIVATE);
	}
}
