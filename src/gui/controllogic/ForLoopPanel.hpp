//$Header$
//------------------------------------------------------------------------------
//                              ForLoopPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/05/17
/**
 * This class contains the Conditional Statement Setup window.
 */
//------------------------------------------------------------------------------
#ifndef ForLoopPanel_hpp
#define ForLoopPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatPanel.hpp"

class ForLoopPanel : public GmatPanel
{
public:
    // constructors
    ForLoopPanel( wxWindow *parent);
    ~ForLoopPanel();  

private:
    wxGrid *conditionGrid;
   
    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    //loj: 2/27/04 commented out
    //virtual void OnHelp();
    //virtual void OnScript();

    // Layout & data handling methods
    void Setup(wxWindow *parent);
                
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 46000,
        ID_TEXTCTRL,
        ID_GRID,
    };
};

#endif // ForLoopPanel_hpp



