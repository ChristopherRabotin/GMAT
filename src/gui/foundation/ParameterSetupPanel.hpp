//$Header$
//------------------------------------------------------------------------------
//                              ParameterSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/25
//
/**
 * Declares ParameterSetupPanel class.
 */
//------------------------------------------------------------------------------

#ifndef ParameterSetupPanel_hpp
#define ParameterSetupPanel_hpp

#include "GmatPanel.hpp"
#include "Parameter.hpp"

class ParameterSetupPanel: public GmatPanel
{
public:
   // constructors
   ParameterSetupPanel(wxWindow *parent, const wxString &name); 
    
private:

   Parameter *mParam;
   bool mIsColorChanged;
   
   wxColour mColor;
   wxString theVarName;
    
   wxTextCtrl *nameTextCtrl;
   wxTextCtrl *expTextCtrl;
   wxTextCtrl *epochTextCtrl;
   wxTextCtrl *indexTextCtrl;
    
   wxButton *mColorButton;
   
   wxComboBox *cbodyComboBox;
   wxComboBox *coordComboBox;
   wxComboBox *rbodyComboBox;
    
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnComboSelection(wxCommandEvent& event);
   void OnColorButtonClick(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON,
      ID_COLOR_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL
   };
};

#endif
