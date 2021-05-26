#include "pch.h"
#include "UnsortedListCtrl.h"

IMPLEMENT_DYNAMIC(CUnsortedListCtrl, CMFCListCtrl)

BEGIN_MESSAGE_MAP(CUnsortedListCtrl, CMFCListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CUnsortedListCtrl::OnColumnClick)
END_MESSAGE_MAP()

void CUnsortedListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
}