//$Header$
//------------------------------------------------------------------------------
//                              UniversePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
/**
 * This class allows user to specify where Universe information is 
 * coming from
 */
//------------------------------------------------------------------------------
#ifndef UniversePanel_hpp
#define UniversePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"

//  #include <wx/sizer.h>
//  #include <wx/image.h>
//  #include <wx/statline.h>
//  #include <wx/listctrl.h>

// Declare window functions
//loj: commented out
//  #define ID_TEXT          10003
//  #define ID_LISTBOX       10004
//  #define ID_BUTTON        10005
//  #define ID_BUTTON_ADD    10008
//  #define ID_BUTTON_SORT   10009
//  #define ID_BUTTON_REMOVE 10010

class UniversePanel: public GmatPanel
{
public:
    UniversePanel(wxWindow *parent);
   
private:

  //    void CreateUniverse(wxWindow *parent);
    void OnAddButton(wxCommandEvent& event);
    void OnSortButton(wxCommandEvent& event);
    void OnRemoveButton(wxCommandEvent& event);
    void OnBrowseButton(wxCommandEvent& event);

    void OnAvailableSelectionChange(wxCommandEvent& event);
    void OnSelectedSelectionChange(wxCommandEvent& event);

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    //loj: 2/27/04 commented out
    //virtual void OnHelp();
    //virtual void OnScript();
    
//      wxBoxSizer *item0;
//      wxGridSizer *item1;
//      wxBoxSizer *item2;
//      wxBoxSizer *item5;
//      wxBoxSizer *item9;

    wxTextCtrl *fileTextCtrl;

    wxStaticText *item3;
    wxStaticText *item10;
    wxStaticText *filetypeStaticText;    

    wxListBox *availableListBox;
    wxListBox *selectedListBox;
    
    wxButton *addButton;
    wxButton *prioritizeButton;
    wxButton *removeButton;
    wxButton *browseButton;
    
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 6100,
        ID_LISTBOX,
        ID_BUTTON,
        ID_BUTTON_ADD,
        ID_BUTTON_SORT,
        ID_BUTTON_REMOVE,

        ID_AVAILABLE_LIST,
        ID_TEXT_CTRL,
        ID_BUTTON_BROWSE,
        ID_SELECTED_LIST,
    };
};
#endif



