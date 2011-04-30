//$Id: SolverSetupPanel.hpp 9457 2011-04-21 18:40:34Z lindajun $
//------------------------------------------------------------------------------
//                           SolverSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2008/04/17
/**
 * This class is a generic setup panel used by plug-in solvers.
 */
//------------------------------------------------------------------------------

#ifndef SolverSetupPanel_hpp
#define SolverSetupPanel_hpp

#include "GmatPanel.hpp"
#include "Solver.hpp"

/**
 * Generic configuration panel for Solvers
 * 
 * This class defines a generic Solver configuration panel that is used when a
 * custom panel has not been coded.  It provides access to all of the Solver's
 * writable parameters using text controls and comboboxes.
 * 
 * The code has been tested using two Optimizers: the VF13ad optimizer and the
 * (incomplete) SteepestDescent optimizer.  The panel functions correctly for
 * both of these optimizers running either as part of a base GMAT build or 
 * through the plug-in interfaces.
 */
class SolverSetupPanel : public GmatPanel
{
public:
	SolverSetupPanel(wxWindow *parent, const wxString &name);
	virtual ~SolverSetupPanel();
	
protected:
   /// The solver that the panel configures
   Solver               *theSolver;
   /// Flag indicating that a change was made to the panel
   bool                 isTextModified;
   
   virtual void         Create();
   virtual void         LoadData();
   virtual void         SaveData();
   void                 Setup(wxWindow *parent);
   wxControl *          BuildControl(wxWindow *parent, Integer index);
   void                 LoadControl(const std::string &label);
   void                 SaveControl(const std::string &label);
   
   // Text control event method
   void OnTextUpdate(wxCommandEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);

   /// Labels used for the configurable properties
   std::vector<wxStaticText*>       propertyDescriptors;
   /// GUI controls that are used to configure the properties
   std::vector<wxControl*>          propertyControls;
   /// Mapping between text strings and the index for the associated control
   std::map<std::string, Integer>   controlMap;

   /// IDs used for event management
   enum
   {
      ID_TEXT = 55000,
      ID_TEXTCTRL,
      ID_COMBOBOX
   };
   
   /// True-false strings (just a convenience here)
   static const wxString TF_SCHEMES[2];

};

#endif /* SolverSetupPanel_hpp */
