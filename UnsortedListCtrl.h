#pragma once
#include <afxlistctrl.h>
class CUnsortedListCtrl :
    public CMFCListCtrl
{
    DECLARE_DYNAMIC(CUnsortedListCtrl)
protected:
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

