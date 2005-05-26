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
#include <map>

class UniversePanel: public GmatPanel
{
public:
   UniversePanel(wxWindow *parent);
   ~UniversePanel();
    
   
private:
   StringArray mAllFileTypes;
   StringArray mFileTypesInUse;
   std::map<wxString, wxString> mFileTypeNameMap;
   bool mHasFileTypesInUseChanged;
   bool mHasFileNameChanged;
   
   wxComboBox *mFileTypeComboBox;
   wxTextCtrl *mFileNameTextCtrl;
   wxButton   *mBrowseButton;
   
   wxListBox *availableListBox;
   wxListBox *selectedListBox;
   
   wxButton *addButton;
   wxButton *removeButton;
   wxButton *clearButton;
   wxButton *prioritizeButton;
   
   void OnAddButton(wxCommandEvent& event);
   void OnSortButton(wxCommandEvent& event);
   void OnRemoveButton(wxCommandEvent& event);
   void OnClearButton(wxCommandEvent& event);
   void OnBrowseButton(wxCommandEvent& event);
   void OnListBoxSelect(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();

   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 6100,
      ID_LISTBOX,
      ID_COMBOBOX,
      ID_BUTTON_ADD,
      ID_BUTTON_SORT,
      ID_BUTTON_REMOVE,
      ID_BUTTON_CLEAR,
      ID_AVAILABLE_LIST,
      ID_TEXT_CTRL,
      ID_BUTTON_BROWSE,
      ID_SELECTED_LIST,
   };
};
#endif
