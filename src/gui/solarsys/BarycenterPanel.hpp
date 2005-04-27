//$Header$
//------------------------------------------------------------------------------
//                              BarycenterPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/04/21
/**
 * This class allows user to specify Barycenter bodies
 */
//------------------------------------------------------------------------------
#ifndef BarycenterPanel_hpp
#define BarycenterPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"


class BarycenterPanel: public GmatPanel
{
public:
    BarycenterPanel(wxWindow *parent);
    ~BarycenterPanel();
    
   
private:
    StringArray theFileTypesInUse;
    
  //    void CreateUniverse(wxWindow *parent);
    void OnAddButton(wxCommandEvent& event);
    void OnSortButton(wxCommandEvent& event);
    void OnRemoveButton(wxCommandEvent& event);
    void OnBrowseButton(wxCommandEvent& event);

    void OnAvailableSelectionChange(wxCommandEvent& event);

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    //loj: 2/27/04 commented out
    //virtual void OnHelp();
    //virtual void OnScript();
    
    wxStaticText *item3;
    wxStaticText *item10;

    wxListBox *availableListBox;
    wxListBox *selectedListBox;
    
    wxButton *addButton;
    wxButton *removeButton;
    
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 6100,
        ID_LISTBOX,
        ID_BUTTON,
        ID_BUTTON_ADD,
        ID_BUTTON_REMOVE,

        ID_AVAILABLE_LIST,
        ID_TEXT_CTRL,
        ID_SELECTED_LIST,
    };
};
#endif



