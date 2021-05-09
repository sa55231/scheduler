#include "pch.h"
#include "ViewDockingPane.h"
#include "MainFrm.h"

IMPLEMENT_DYNAMIC(CViewDockingPane, CDockablePane)

BEGIN_MESSAGE_MAP(CViewDockingPane, CDockablePane)
END_MESSAGE_MAP()

CViewDockingPane::CViewDockingPane()
{
}

CViewDockingPane::~CViewDockingPane()
{
}

void CViewDockingPane::Initialize(CSchedulerDoc* pDoc)
{
	document = pDoc;
	OnInitialUpdate();
}

void CViewDockingPane::DeInitialize()
{

}

CMainFrame* CViewDockingPane::GetMainFrame() const
{
	return ((CMainFrame*)AfxGetMainWnd());
}

void CViewDockingPane::Update(const LPARAM lHint)
{
	if(document != nullptr)
	{
		OnUpdate(lHint);
	}
}
