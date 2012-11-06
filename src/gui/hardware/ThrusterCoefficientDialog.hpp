//$Id$
//------------------------------------------------------------------------------
//                              ThrusterCoefficientDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

class ThrusterCoefficientDialog : public GmatDialog
{
public:
   ThrusterCoefficientDialog(wxWindow *parent, wxWindowID id, 
                             const wxString &title, GmatBase *obj,
                             const wxString &type, const RealArray &coefsCOrK);

   RealArray&      GetCoefValues()  { return coefValues;    }
   bool            AreCoefsSaved()  { return coefsModified; }
   
private:
   
   GmatBase    *theObject;
   Integer     coefCount;
   StringArray coefNames;
   RealArray   coefValues;
   
   bool        coefsModified;

   wxString coefType;
   
   wxGrid *coefGrid;
   
   // methods inherited from GmatDialog
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   virtual void ResetData();
   
   // event handling   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_GRID = 30300,
   };
};

#endif
