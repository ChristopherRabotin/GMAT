//$Header$
//------------------------------------------------------------------------------
//                              ThrusterCoefficientDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2005/01/13
//
/**
 * Declares ThrusterCoefficientDialog class. This class shows dialog window where
 * thruster coefficients can be modified.
 * 
 */
//------------------------------------------------------------------------------
#ifndef ThrusterCoefficientDialog_hpp
#define ThrusterCoefficientDialog_hpp

#include "gmatwxdefs.hpp"
#include "GmatDialog.hpp"
#include "Thruster.hpp"

class ThrusterCoefficientDialog : public GmatDialog
{
public:
   ThrusterCoefficientDialog(wxWindow *parent, Thruster *thruster, std::string type);
   
private:  
    
   Thruster *theThruster;
   
   StringArray coefName;
   RealArray coefValue;
   
   std::string coefType;
     
   wxGrid *coefficientGrid;

   // methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling   
   DECLARE_EVENT_TABLE();
   void OnCellValueChange(wxGridEvent &event);
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_GRID = 30300,
   };
};

#endif



