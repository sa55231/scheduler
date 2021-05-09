#pragma once


#include "schedulerDoc.h"
class CMainFrame;

class CViewDockingPane : public CDockablePane
{
	DECLARE_DYNAMIC(CViewDockingPane)
public:
    /// Initializes the pane with the specified document
    void Initialize(CSchedulerDoc* pDoc);

    void DeInitialize();

    /// Checks if window is valid and then forwards call to pure virtual OnUpdate() method.
    void Update(const LPARAM lHint);

protected:
    CViewDockingPane();
    virtual ~CViewDockingPane();


    CSchedulerDoc* GetDocument() const { ASSERT(NULL != document); return document; }

    CMainFrame* GetMainFrame() const;

    /**
     * This method is called after a document pointer has been set with #Initialize().
     * Override this in derived classes to mimic a view's "OnInitialUpdate()-behavior".
     */
    virtual void OnInitialUpdate() = 0;

    /**
     * Called by #Update(). Overrider to mimic a view's "OnUpdate()-behavior".
     * This method has a simplified parameter list. Enhance this if necessary.
     */
    virtual void OnUpdate(const LPARAM lHint) = 0;

    DECLARE_MESSAGE_MAP()
private:
    CSchedulerDoc* document = nullptr;
};

