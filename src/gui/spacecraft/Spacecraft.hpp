//$Header$
//------------------------------------------------------------------------------
//                            Spacecraft
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
/**
 * This class contains information needed to setup users spacecraft through GUI
 * 
 */
//------------------------------------------------------------------------------
#ifndef Spacecraft_hpp
#define Spacecraft_hpp

#include "gmatwxdefs.h"
#include "wx/sizer.h"
#include "wx/splitter.h"
#include "wx/listctrl.h"
#include "wx/treectrl.h"
#include "wx/notebook.h"
#include "wx/button.h"
#include "wx/grid.h"

// Declare window functions
#define ID_TEXT 10000
#define ID_COMBO 10001
#define ID_TEXTCTRL 10002

#define ID_NOTEBOOK 20000
#define ID_BUTTON   20001
#define ID_ELEMENT_TAG1     20006
#define ID_ELEMENT_TAG2     20007
#define ID_ELEMENT_TAG3     20008
#define ID_ELEMENT_TAG4     20009
#define ID_ELEMENT_TAG5     20010
#define ID_ELEMENT_TAG6     20011

#define ID_ELEMENT_VALUE1     20012
#define ID_ELEMENT_VALUE2     20013
#define ID_ELEMENT_VALUE3     20014
#define ID_ELEMENT_VALUE4     20015
#define ID_ELEMENT_VALUE5     20016
#define ID_ELEMENT_VALUE6     20017

#define ID_CHOICE_BODY  20018
#define ID_CHOICE_FRAME 20019
#define ID_CHOICE_EPOCH 20020
#define ID_CHOICE_STATE 20021

#define ID_EPOCH_VALUE 10011

#define ID_BUTTON_OK     20022
#define ID_BUTTON_APPLY  20023
#define ID_SC_BUTTON_CANCEL 20024
#define ID_BUTTON_HELP   20025


class Spacecraft: public wxPanel
{
public:
 // Spacecraft(wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE);
   Spacecraft(wxWindow *parent);
   
private:
  void CreateNotebook(wxWindow *parent);
   wxPanel *CreateOrbit(wxWindow *parent);
   void CartElements(wxWindow *parent);
   void KapElements(wxWindow *parent);

//   wxBoxSizer *item;
   wxPanel *elements;
   wxNotebook *mainNotebook;
   wxNotebookSizer * sizer;
   wxPanel *orbit;
   wxPanel *attitude;
   wxPanel *properties;
   wxPanel *actuators;
   wxPanel *sensors;
   wxPanel *tanks;
   wxPanel *visual;
   wxStaticText *coordinateLabel;
   wxButton *okButton;
   wxButton *applyButton;
   wxButton *cancelButton;
   //wxButton *helpButton;
    
   void OnBodyChoice(wxCommandEvent& event);
   void OnFrameChoice(wxCommandEvent& event);
   void OnEpochChoice(wxCommandEvent& event);
   void OnStateChoice(wxCommandEvent& event);
   void OnOkButton(wxCommandEvent& event);
   void OnApplyButton(wxCommandEvent& event);
   void OnCancelButton(wxCommandEvent& event);
   void OnHelpButton(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
};
#endif
