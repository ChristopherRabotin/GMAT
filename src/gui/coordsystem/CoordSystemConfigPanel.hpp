//$Id$
//------------------------------------------------------------------------------
//                              CoordSystemConfigPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Waka Waktola
// Created: 2004/10/26
/**
 * This class contains the Coordinate System Setup Panel.
 */
//------------------------------------------------------------------------------
#ifndef CoordSystemConfigPanel_hpp
#define CoordSystemConfigPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "CoordinateSystem.hpp"
#include "CoordPanel.hpp"

class CoordSystemConfigPanel : public GmatPanel
{
public:
   // constructors
   CoordSystemConfigPanel(wxWindow *parent, const wxString &coordName);
   ~CoordSystemConfigPanel();  
   
private:
   CoordinateSystem *theCoordSys;
   CoordPanel       *mCoordPanel;
   wxString         mEpochFormat;
   bool             mOriginChanged;
   bool             mObjRefChanged;
   bool             mEpochChanged;
   
   wxTextCtrl *epochTextCtrl;
   wxTextCtrl *intervalTextCtrl;

   wxComboBox *originComboBox;
   wxComboBox *typeComboBox;
   wxComboBox *primaryComboBox;
   wxComboBox *formatComboBox;
   wxComboBox *secondaryComboBox;
   wxComboBox *refObjectComboBox;
   wxComboBox *constraintCSComboBox;
   
   wxComboBox *xComboBox;
   wxComboBox *yComboBox;
   wxComboBox *zComboBox;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   virtual void ResetFlags();

   // data handling methods
   void OnTextUpdate(wxCommandEvent& event);
   void OnComboUpdate(wxCommandEvent& event);

   // event handling
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {
       ID_TEXTCTRL = 46000,
       ID_COMBO,
       ID_TEXT,
   };
};

#endif // CoordSystemConfigPanel_hpp
