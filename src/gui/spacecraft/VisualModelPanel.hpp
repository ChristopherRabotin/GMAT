//$Id$
//------------------------------------------------------------------------------
//                            VisualModelPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Phillip Silvia, Jr.
// Created: 2009/08/12
/**
 * This class contains information needed to setup users spacecraft visual 
 * model through GUI.
 */
//------------------------------------------------------------------------------
#ifndef VisualModelPanel_hpp
#define VisualModelPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "wx/spinctrl.h"
#include "VisualModelCanvas.hpp"
#include "Spacecraft.hpp"

class VisualModelPanel: public wxPanel
{
public:
   VisualModelPanel(GmatPanel *scPanel, wxWindow *parent,
              Spacecraft *spacecraft, SolarSystem *solarsystem);
   ~VisualModelPanel();
   
   bool IsDataChanged() { return dataChanged; }
   bool CanClosePanel() { return canClose; }
   void SaveData();

   bool CanvasOn(bool onOrOff);

protected:
      
   // Event Handling
   DECLARE_EVENT_TABLE();
   
   void OnTextCtrlChange(wxCommandEvent& event);
   void OnTextCtrlEnter(wxCommandEvent& event);
   void OnBrowseButton(wxCommandEvent& event);
   void OnRecenterButton(wxCommandEvent& event);
   void OnAutoscaleButton(wxCommandEvent& event);
   void OnEarthButton(wxCommandEvent& event);
   void OnSpin(wxSpinEvent& event);
   void OnSlide(wxCommandEvent& event);
   void UpdateTextCtrl(int id);
   
private:

   bool dataChanged;
   bool canClose;
   bool mTextChanged;
	bool interfaceEnabled;
   
   wxString modelPath;
   
   // The current spacecraft
   Spacecraft     *currentSpacecraft;
   // The solar system
   SolarSystem    *theSolarSystem;
   // The gui manager
   GuiItemManager *theGuiManager;
   // The canvas for drawing the model and its transformations
   VisualModelCanvas *modelCanvas;

   // Sliders for rotation
   wxSlider *xRotSlider;
   wxSlider *yRotSlider;
   wxSlider *zRotSlider;

   // Text Displays for the current rotation values
   wxTextCtrl *xRotValueText;
   wxTextCtrl *yRotValueText;
   wxTextCtrl *zRotValueText;

   // Sliders for translation
   wxSlider *xTranSlider;
   wxSlider *yTranSlider;
   wxSlider *zTranSlider;

   // Text Displays for the current translation values
   wxTextCtrl *xTranValueText;
   wxTextCtrl *yTranValueText;
   wxTextCtrl *zTranValueText;

   // Slider for scale
   wxSlider *scaleSlider;

   // Text Display for the current scale value
   wxTextCtrl *scaleValueText;

   // Text for loading the model
   wxTextCtrl *modelTextCtrl;

   // Button for toggling the Earth
   wxButton *showEarthButton;

	// Button that automatically scales the model
	wxButton *autoscaleButton;

	// Button that automatically centers the model
	wxButton *recenterButton;
   
   // The spacecraft panel that this panel belongs to
   GmatPanel *theScPanel;
   
   void Create();
   void InitializeCanvas();
	void ToggleInterface(bool enable);
   void AutoScaleModel();
   void ResetSliders();
   wxString ToString(Real rval);
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 30200,
      ID_MODELFILE_TEXT,
      ID_CANVAS,
      ID_FRAME,
      
      ID_BROWSE_BUTTON,
      ID_RECENTER_BUTTON,
		ID_AUTOSCALE_BUTTON,
      ID_EARTH_BUTTON,
      
      ID_ROT_SLIDER,
      ID_TRAN_SLIDER,
      ID_SCALE_SLIDER,
      
      ID_ROT_TEXT,
      ID_TRAN_TEXT,
      ID_SCALE_TEXT,
      
      ID_STATIC_ELEMENT     
   };
};
#endif

