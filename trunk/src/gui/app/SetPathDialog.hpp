//$Id$
//------------------------------------------------------------------------------
//                              SetPathDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2008/04/16
//
/**
 * Shows dialog for setting path for various files used in the system.
 * 
 */
//------------------------------------------------------------------------------
#ifndef SetPathDialog_hpp
#define SetPathDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include "MultiPathSetupPanel.hpp"
#include "SinglePathSetupPanel.hpp"

class SetPathDialog : public GmatDialog
{
public:
   
   SetPathDialog(wxWindow *parent);
   ~SetPathDialog();
   
protected:
   
   wxNotebook *mPathNotebook;
   MultiPathSetupPanel  *mGmatFunPathPanel;
   MultiPathSetupPanel  *mMatlabPathPanel;
   SinglePathSetupPanel *mOutputPathPanel;
   
   // override methods from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   void OnPageChange(wxCommandEvent &event);
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9300,
      ID_NOTEBOOK,
   };
   
};

#endif
