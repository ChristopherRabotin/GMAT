//$Header$
//------------------------------------------------------------------------------
//                              ReportFileSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
/**
 * Declares ReportFileSetupPanel class. This class allows user to setup OpenGL Plot.
 */
//------------------------------------------------------------------------------
#ifndef ReportFileSetupPanel_hpp
#define ReportFileSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class ReportFileSetupPanel: public GmatPanel
{
public:
    ReportFileSetupPanel(wxWindow *parent, const wxString &subscriberName);
   
protected:
    GuiItemManager *theGuiManager; //loj: move this to GmatPanel later
    Subscriber *theSubscriber;
    wxString *theParamList;
    
    wxBoxSizer *pageBoxSizer;
    wxBoxSizer *optionBoxSizer;

    wxCheckBox *writeCheckBox;
    
    wxStaticText *fileStaticText;
    wxTextCtrl *fileTextCtrl;
    wxButton *browseButton; 


    void OnWriteCheckBoxChange(wxCommandEvent& event);
    void OnBrowseButton(wxCommandEvent& event);

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        RF_TEXT = 93000,
        RF_WRITE_CHECKBOX,
        
        ID_TEXT,
        ID_TEXT_CTRL,
        ID_BROWSE_BUTTON,
    };
};
#endif
