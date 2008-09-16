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
#include "Barycenter.hpp"

class BarycenterPanel: public GmatPanel
{
public:
    BarycenterPanel(wxWindow *parent, const wxString &name);
    ~BarycenterPanel();
    
   wxArrayString& GetBodyNames()
      { return mBodyNames; }
   bool IsBodySelected()
      { return mIsBodySelected; }

private:
   Barycenter *theBarycenter;

   static const int MAX_LIST_SIZE = 30;
   wxArrayString mBodyNames;

   bool mIsBodySelected;

   wxString mSelBodyName;

   wxButton *addBodyButton;
   wxButton *removeBodyButton;
   wxButton *clearBodyButton;

   wxListBox *bodyListBox;
   wxListBox *bodySelectedListBox;

   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   //loj: 2/27/04 commented out
   //virtual void OnHelp();
   //virtual void OnScript();
    
   void OnButton(wxCommandEvent& event);

   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 6100,
      ID_LISTBOX,
      ID_BUTTON,
      ID_BUTTON_ADD,
      ID_BUTTON_REMOVE,
      ID_BODY_SEL_LISTBOX,
   };
};
#endif



