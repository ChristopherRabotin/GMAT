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
// Author: Phillip Silvia, based on the OrbitPanel by Allison Greene
// Created: 2009/08/12
/**
 * Panel for displaying the currently loaded model for the spacecraft and storing
 * initial parameters.
 */
//------------------------------------------------------------------------------
#include "VisualModelPanel.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include "LoadPOV.hpp"
#include <sstream>

//#define DEBUG_ORBIT_PANEL
//#define DEBUG_ORBIT_PANEL_LOAD
//#define DEBUG_ORBIT_PANEL_CONVERT
//#define DEBUG_ORBIT_PANEL_COMBOBOX
//#define DEBUG_ORBIT_PANEL_STATE_TYPE
//#define DEBUG_ORBIT_PANEL_SAVE
//#define DEBUG_ORBIT_PANEL_CHECK_RANGE

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(VisualModelPanel, wxPanel)
   EVT_TEXT_ENTER(ID_TEXTCTRL, VisualModelPanel::OnTextCtrlEnter)
   EVT_TEXT_ENTER(ID_ROT_TEXT, VisualModelPanel::OnTextCtrlEnter)
   EVT_TEXT_ENTER(ID_TRAN_TEXT, VisualModelPanel::OnTextCtrlEnter)
   EVT_TEXT_ENTER(ID_SCALE_TEXT, VisualModelPanel::OnTextCtrlEnter)
   EVT_TEXT(ID_TEXTCTRL, VisualModelPanel::OnTextCtrlChange)
   EVT_TEXT(ID_ROT_TEXT, VisualModelPanel::OnTextCtrlChange)
   EVT_TEXT(ID_TRAN_TEXT, VisualModelPanel::OnTextCtrlChange)
   EVT_TEXT(ID_SCALE_TEXT, VisualModelPanel::OnTextCtrlChange)
   EVT_BUTTON(ID_BROWSE_BUTTON, VisualModelPanel::OnBrowseButton)
   EVT_BUTTON(ID_RECENTER_BUTTON, VisualModelPanel::OnRecenterButton)
   EVT_BUTTON(ID_AUTOSCALE_BUTTON, VisualModelPanel::OnAutoscaleButton)
   EVT_BUTTON(ID_EARTH_BUTTON, VisualModelPanel::OnEarthButton)
   EVT_SLIDER(ID_ROT_SLIDER, VisualModelPanel::OnSlide)
   EVT_SLIDER(ID_TRAN_SLIDER, VisualModelPanel::OnSlide)
   EVT_SLIDER(ID_SCALE_SLIDER, VisualModelPanel::OnSlide)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// VisualModelPanel(GmatPanel *scPanel, wxWindow *parent, Spacecraft *spacecraft,
//            SolarSystem *solarsystem
//------------------------------------------------------------------------------
/**
 * Constructs VisualModelPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the Universe GUI
 */
//------------------------------------------------------------------------------
VisualModelPanel::VisualModelPanel(GmatPanel *scPanel, wxWindow *parent,
                       Spacecraft *spacecraft, SolarSystem *solarsystem)
   : wxPanel(parent)
{
   // initalize data members
   theScPanel = scPanel;
   theGuiManager = GuiItemManager::GetInstance();
   currentSpacecraft = spacecraft;
   theSolarSystem = solarsystem;

   //LoadPOV(new ModelObject(), wxT("C:/Programs/DevelGMAT/files/models/DMSPWithTextures/DMSP-WithTextures-POVRayFormat.pov"));

   FileManager *fm = FileManager::Instance();
   modelPath = wxT(fm->GetPathname("MODEL_PATH").c_str());

   Create();
}


//------------------------------------------------------------------------------
// ~VisualModelPanel()
//------------------------------------------------------------------------------
VisualModelPanel::~VisualModelPanel()
{

}

//------------------------------------------------------------------------------
// bool CanvasOn(bool onOrOff)
//
// Needed for the Mac, to avoid having the other tabs messed up by the Canvas
//------------------------------------------------------------------------------
bool VisualModelPanel::CanvasOn(bool onOrOff)
{
   if (modelCanvas) modelCanvas->Show(onOrOff);
   return true;
}
//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 *
 * @note Creates the page for orbit information
 */
//------------------------------------------------------------------------------
void VisualModelPanel::Create()
{
   #ifdef DEBUG_ORBIT_PANEL
      MessageInterface::ShowMessage("In VisualModelPanel::Create() \n");
   #endif

   Integer bsize = 2.0; // border size

   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;

   // Creates the sizer for the overall panel
   wxBoxSizer *visSizer = new wxBoxSizer(wxHORIZONTAL);

   // Sizer for the left side, which has all the sliders
   wxBoxSizer *leftSizer =
      new wxBoxSizer(wxVERTICAL);

   // A box containing the file loader
   wxStaticBox *filenameBox =
      new wxStaticBox(this, ID_STATIC_ELEMENT, wxT("File Name"));
   wxStaticBoxSizer *filenameSizer =
      new wxStaticBoxSizer(filenameBox, wxVERTICAL);

   // Sizer for the file loader
   wxFlexGridSizer *flexFileSizer =
      new wxFlexGridSizer(1, 2, bsize, bsize);
   // Sizer for the rotation sliders
   wxFlexGridSizer *rotationSizer =
      new wxFlexGridSizer(3, 6, bsize, bsize);
   // Sizer for the translation sliders
   wxFlexGridSizer *translationSizer =
      new wxFlexGridSizer(3, 5, bsize, bsize);
   // Sizer for the scale slider
   wxFlexGridSizer *scaleSizer =
      new wxFlexGridSizer(1, 4, bsize, bsize);

   // Box and sizer containing the rotation sizer
   wxStaticBox *rotationBox =
      new wxStaticBox(this, ID_STATIC_ELEMENT, wxT("Rotation"));
   wxStaticBoxSizer *rotationOuterSizer =
      new wxStaticBoxSizer(rotationBox, wxVERTICAL);
   // Box and sizer containing the translation sizer
   wxStaticBox *translationBox =
      new wxStaticBox(this, ID_STATIC_ELEMENT, wxT("Translation"));
   wxStaticBoxSizer *translationOuterSizer =
      new wxStaticBoxSizer(translationBox, wxVERTICAL);
   // Box and sizer containing the scale sizer
   wxStaticBox *scaleBox =
      new wxStaticBox(this, ID_STATIC_ELEMENT, wxT("Scale"));
   wxStaticBoxSizer *scaleOuterSizer =
      new wxStaticBoxSizer(scaleBox, wxVERTICAL);

   // Box, panel, and sizers for the canvas on the right
   wxStaticBox *canvasBox =
      new wxStaticBox(this, ID_STATIC_ELEMENT, wxT("Display"));
   wxStaticBoxSizer *rightSizer =
      new wxStaticBoxSizer(canvasBox, wxVERTICAL);
   wxPanel *canvasPanel =
      new wxPanel(this, ID_FRAME);
   wxSizer *canvasSizer =
      new wxBoxSizer(wxVERTICAL);
   // Set up the canvas with the correct sizer
   canvasPanel->SetSizer(canvasSizer);
   modelCanvas =
      new VisualModelCanvas(canvasPanel, currentSpacecraft, ID_CANVAS, wxDefaultPosition, wxDefaultSize,
      wxT("Visual Model Canvas"), WX_GL_DOUBLEBUFFER);
   // Add the canvas to the sizer and set its size
   modelCanvas->SetSizeHints(300, 350);

   // The "Show Earth" button
   showEarthButton =
      new wxButton(this, ID_EARTH_BUTTON, wxT("Show Earth"));
	showEarthButton->SetToolTip(wxT("Show a wireframe Earth for size reference"));

   // Text and button for the file loader
   modelTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(180, 20), wxTE_PROCESS_ENTER);
   wxButton *browseButton =
      new wxButton(this, ID_BROWSE_BUTTON, wxT("Browse..."));
	browseButton->SetToolTip(wxT("Find a model file to attach to the spacecraft"));

   // Labels and slider for x rotation
   wxStaticText *xRotText =
      new wxStaticText(this, ID_TEXT, wxT("X"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *xRotMin =
      new wxStaticText(this, ID_TEXT, wxT("-180"), wxDefaultPosition, wxDefaultSize, 0);
   xRotSlider =
      new wxSlider(this, ID_ROT_SLIDER, 0, -180, 180, wxDefaultPosition, wxSize(120, 25),
      wxSL_HORIZONTAL);
   wxStaticText *xRotMax =
      new wxStaticText(this, ID_TEXT, wxT("180"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *xRotDegrees =
      new wxStaticText(this, ID_TEXT, wxT("Degrees"), wxDefaultPosition, wxDefaultSize, 0);
   xRotValueText =
	  new wxTextCtrl(this, ID_ROT_TEXT, wxT("0.000000"), wxDefaultPosition, wxSize(70, 20), wxTE_PROCESS_ENTER);


   // Labels and slider for y rotation
   wxStaticText *yRotText =
      new wxStaticText(this, ID_TEXT, wxT("Y"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *yRotMin =
      new wxStaticText(this, ID_TEXT, wxT("-180"), wxDefaultPosition, wxDefaultSize, 0);
   yRotSlider =
      new wxSlider(this, ID_ROT_SLIDER, 0, -180, 180, wxDefaultPosition, wxSize(120, 25),
      wxSL_HORIZONTAL);
   wxStaticText *yRotMax =
      new wxStaticText(this, ID_TEXT, wxT("180"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *yRotDegrees =
      new wxStaticText(this, ID_TEXT, wxT("Degrees"), wxDefaultPosition, wxDefaultSize, 0);
   yRotValueText =
	  new wxTextCtrl(this, ID_ROT_TEXT, wxT("0.000000"), wxDefaultPosition, wxSize(70, 20), wxTE_PROCESS_ENTER);

   // Labels and slider for z rotation
   wxStaticText *zRotText =
      new wxStaticText(this, ID_TEXT, wxT("Z"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *zRotMin =
      new wxStaticText(this, ID_TEXT, wxT("-180"), wxDefaultPosition, wxDefaultSize, 0);
   zRotSlider =
      new wxSlider(this, ID_ROT_SLIDER, 0, -180, 180, wxDefaultPosition, wxSize(120, 25),
      wxSL_HORIZONTAL);
   wxStaticText *zRotMax =
      new wxStaticText(this, ID_TEXT, wxT("180"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *zRotDegrees =
      new wxStaticText(this, ID_TEXT, wxT("Degrees"), wxDefaultPosition, wxDefaultSize, 0);
   zRotValueText =
	  new wxTextCtrl(this, ID_ROT_TEXT, wxT("0.000000"), wxDefaultPosition, wxSize(70, 20), wxTE_PROCESS_ENTER);

   // Label and slider for x translation
   wxStaticText *xTranText =
      new wxStaticText(this, ID_TEXT, wxT("X"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *xTranMin =
      new wxStaticText(this, ID_TEXT, wxT("-3.5"), wxDefaultPosition, wxDefaultSize, 0);
   xTranSlider =
      new wxSlider(this, ID_TRAN_SLIDER, 0, -350, 350, wxDefaultPosition, wxSize(120, 25),
      wxSL_HORIZONTAL);
   wxStaticText *xTranMax =
      new wxStaticText(this, ID_TEXT, wxT("3.5"), wxDefaultPosition, wxDefaultSize, 0);
   xTranValueText =
	  new wxTextCtrl(this, ID_TRAN_TEXT, wxT("0.000000"), wxDefaultPosition, wxSize(70, 20), wxTE_PROCESS_ENTER);

   // Label and slider for y translation
   wxStaticText *yTranMin =
      new wxStaticText(this, ID_TEXT, wxT("-3.5"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *yTranText =
      new wxStaticText(this, ID_TEXT, wxT("Y"), wxDefaultPosition, wxDefaultSize, 0);
   yTranSlider =
      new wxSlider(this, ID_TRAN_SLIDER, 0, -350, 350, wxDefaultPosition, wxSize(120, 25),
      wxSL_HORIZONTAL);
   wxStaticText *yTranMax =
      new wxStaticText(this, ID_TEXT, wxT("3.5"), wxDefaultPosition, wxDefaultSize, 0);
   yTranValueText =
	  new wxTextCtrl(this, ID_TRAN_TEXT, wxT("0.000000"), wxDefaultPosition, wxSize(70, 20), wxTE_PROCESS_ENTER);

   // Label and slider for z translation
   wxStaticText *zTranMin =
      new wxStaticText(this, ID_TEXT, wxT("-3.5"), wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *zTranText =
      new wxStaticText(this, ID_TEXT, wxT("Z"), wxDefaultPosition, wxDefaultSize, 0);
   zTranSlider =
      new wxSlider(this, ID_TRAN_SLIDER, 0, -350, 350, wxDefaultPosition, wxSize(120, 25),
      wxSL_HORIZONTAL);
   wxStaticText *zTranMax =
      new wxStaticText(this, ID_TEXT, wxT("3.5"), wxDefaultPosition, wxDefaultSize, 0);
   zTranValueText =
		new wxTextCtrl(this, ID_TRAN_TEXT, wxT("0.000000"), wxDefaultPosition, wxSize(70, 20), wxTE_PROCESS_ENTER);

   // Slider for scale
   wxStaticText *scaleMinLabel =
      new wxStaticText(this, ID_TEXT, wxT("0.001"), wxDefaultPosition, wxDefaultSize, 0);
   scaleSlider =
      new wxSlider(this, ID_SCALE_SLIDER, 0, -1000, 1000, wxDefaultPosition, wxSize(120, 25),
      wxSL_HORIZONTAL);
   wxStaticText *scaleMaxLabel =
      new wxStaticText(this, ID_TEXT, wxT("1000.0"), wxDefaultPosition, wxDefaultSize, 0);
   scaleValueText =
		new wxTextCtrl(this, ID_SCALE_TEXT, wxT("1.000000"), wxDefaultPosition, wxSize(70, 20), wxTE_PROCESS_ENTER);

   // The recentering button
   recenterButton =
      new wxButton(this, ID_RECENTER_BUTTON, wxT("Recenter Model"));
	recenterButton->SetToolTip(wxT("Automatically center the model on its center mass"));
   // The autoscaling button
   autoscaleButton =
	   new wxButton(this, ID_AUTOSCALE_BUTTON, wxT("Autoscale Model"));
	autoscaleButton->SetToolTip(wxT("Automatically scale the model to a visible size"));

   // Add all components to their appropriate sizers
   flexFileSizer->Add(modelTextCtrl, 0, wxALIGN_LEFT | wxALL, bsize);
   flexFileSizer->Add(browseButton, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(xRotText, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(xRotMin, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(xRotSlider, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(xRotMax, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(xRotDegrees, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(xRotValueText, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(yRotText, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(yRotMin, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(yRotSlider, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(yRotMax, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(yRotDegrees, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(yRotValueText, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(zRotText, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(zRotMin, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(zRotSlider, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(zRotMax, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(zRotDegrees, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationSizer->Add(zRotValueText, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(xTranText, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(xTranMin, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(xTranSlider, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(xTranMax, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(xTranValueText, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(yTranText, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(yTranMin, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(yTranSlider, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(yTranMax, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(yTranValueText, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(zTranText, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(zTranMin, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(zTranSlider, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(zTranMax, 0, wxALIGN_LEFT | wxALL, bsize);
   translationSizer->Add(zTranValueText, 0, wxALIGN_LEFT | wxALL, bsize);
   scaleSizer->Add(scaleMinLabel, 0, wxALIGN_LEFT | wxALL, bsize);
   scaleSizer->Add(scaleSlider, 0, wxALIGN_LEFT | wxALL, bsize);
   scaleSizer->Add(scaleMaxLabel, 0, wxALIGN_LEFT | wxALL, bsize);
   scaleSizer->Add(scaleValueText, 0, wxALIGN_LEFT | wxALL, bsize);
   canvasSizer->Add(modelCanvas);

   // Add the inner sizers to their out sizers
   filenameSizer->Add(flexFileSizer, 0, wxALIGN_LEFT | wxALL, bsize);
   rotationOuterSizer->Add(rotationSizer, 0, wxALIGN_LEFT | wxALL, bsize);
   translationOuterSizer->Add(translationSizer, 0, wxALIGN_LEFT | wxALL, bsize);
   scaleOuterSizer->Add(scaleSizer, 0, wxALIGN_LEFT | wxALL, bsize);

   // Put all of the sizers into their appropriate sides of the panel
   leftSizer->Add(filenameSizer, 0, wxGROW | wxALIGN_CENTER, bsize);
   leftSizer->Add(rotationOuterSizer, 0, wxGROW | wxALIGN_CENTER, bsize);
   leftSizer->Add(translationOuterSizer, 0, wxGROW | wxALIGN_CENTER, bsize);
   leftSizer->Add(scaleOuterSizer, 0, wxGROW | wxALIGN_CENTER, bsize);
	leftSizer->Add(recenterButton, 0, wxALL | wxALIGN_CENTER, bsize);
	leftSizer->Add(autoscaleButton, 0, wxALL | wxALIGN_CENTER, bsize);
   rightSizer->Add(canvasPanel, 0, wxEXPAND | wxALIGN_CENTER, bsize);
   rightSizer->Add(showEarthButton, 0, wxALL | wxALIGN_CENTER, bsize);
   canvasPanel->SetSizer(canvasSizer);

   // Add the left and right halves together
   visSizer->Add(leftSizer, 1, wxGROW | wxALIGN_CENTER, bsize);
   visSizer->Add(rightSizer, 1, wxGROW | wxALIGN_CENTER, bsize);

   InitializeCanvas();
	if (currentSpacecraft->modelFile == "")
	{
		ToggleInterface(false);
		interfaceEnabled = false;
	}
	else
		interfaceEnabled = true;
   // Set the sizer to the overall sizer
   this->SetSizer( visSizer );
}

void VisualModelPanel::ToggleInterface(bool enable)
{
	if (enable)
	{
		xRotSlider->Enable();
		yRotSlider->Enable();
		zRotSlider->Enable();
		xRotValueText->Enable();
		yRotValueText->Enable();
		zRotValueText->Enable();

		xTranSlider->Enable();
		yTranSlider->Enable();
		zTranSlider->Enable();
		xTranValueText->Enable();
		yTranValueText->Enable();
		zTranValueText->Enable();

		scaleSlider->Enable();
		scaleValueText->Enable();

		autoscaleButton->Enable();
		recenterButton->Enable();
	}
	else
	{
		xRotSlider->Disable();
		yRotSlider->Disable();
		zRotSlider->Disable();
		xRotValueText->Disable();
		yRotValueText->Disable();
		zRotValueText->Disable();

		xTranSlider->Disable();
		yTranSlider->Disable();
		zTranSlider->Disable();
		xTranValueText->Disable();
		yTranValueText->Disable();
		zTranValueText->Disable();

		scaleSlider->Disable();
		scaleValueText->Disable();

		autoscaleButton->Disable();
		recenterButton->Disable();	
	}
}

void VisualModelPanel::InitializeCanvas()
{
   Real x,y,z;
   if (currentSpacecraft->modelFile != "")
   {
      modelCanvas->LoadModel(currentSpacecraft->modelFile.c_str());
      x = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetX"));
      y = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetY"));
      z = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetZ"));
      xTranValueText->SetLabel(wxString::Format(wxT("%f"), x));
      yTranValueText->SetLabel(wxString::Format(wxT("%f"), y));
      zTranValueText->SetLabel(wxString::Format(wxT("%f"), z));
      xTranSlider->SetValue(x*100);
      yTranSlider->SetValue(y*100);
      zTranSlider->SetValue(z*100);
      //modelCanvas->Translate(x, y, z);

      x = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelRotationX"));
      y = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelRotationY"));
      z = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelRotationZ"));
      xRotValueText->SetLabel(wxString::Format(wxT("%f"), x));
      yRotValueText->SetLabel(wxString::Format(wxT("%f"), y));
      zRotValueText->SetLabel(wxString::Format(wxT("%f"), z));
      xRotSlider->SetValue(x);
      yRotSlider->SetValue(y);
      zRotSlider->SetValue(z);
      //modelCanvas->Rotate(true, x, y, z);

      x = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelScale"));
      scaleSlider->SetValue(x);
      scaleValueText->SetLabel(wxString::Format(wxT("%f"), x));
      //modelCanvas->Scale(x, x, x);

      modelTextCtrl->ChangeValue(wxT(currentSpacecraft->modelFile.c_str()));
      modelCanvas->Refresh(false);
   }
}

//------------------------------------------------------------------------------
// void ResetSliders()
//------------------------------------------------------------------------------
/**
 * Resets the rotation, translation, and scale sizers back to their starting
 * values.
 */
//------------------------------------------------------------------------------
void VisualModelPanel::ResetSliders()
{
   xRotSlider->SetValue(0);
   yRotSlider->SetValue(0);
   zRotSlider->SetValue(0);
   xRotValueText->SetLabel(wxT("0"));
   yRotValueText->SetLabel(wxT("0"));
   zRotValueText->SetLabel(wxT("0"));
   currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelRotationX"), 0);
   currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelRotationY"), 0);
   currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelRotationZ"), 0);
   xTranSlider->SetValue(0);
   yTranSlider->SetValue(0);
   zTranSlider->SetValue(0);
   xTranValueText->SetLabel(wxT("0"));
   yTranValueText->SetLabel(wxT("0"));
   zTranValueText->SetLabel(wxT("0"));
   mTextChanged = false;
   currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetX"), 0);
   currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetY"), 0);
   currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetZ"), 0);
   scaleSlider->SetValue(1);
   scaleValueText->SetLabel(wxT("1"));
   currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetX"), 1);
}

//------------------------------------------------------------------------------
// void OnPaint(wxPaintEvent& event)
//------------------------------------------------------------------------------
/**
 * Paints the panel each cycle.
 */
//------------------------------------------------------------------------------
void VisualModelPanel::OnPaint(wxPaintEvent& event)
{
	if (interfaceEnabled && currentSpacecraft->modelFile == "")
	{
		interfaceEnabled = false;
		ToggleInterface(false);
	}
	if (!interfaceEnabled && currentSpacecraft->modelFile != "")
	{
		interfaceEnabled = true;
		ToggleInterface(true);
	}
   wxPaintDC(this);
}

//------------------------------------------------------------------------------
// void OnSlide(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Processes the events where one of the sliders move. It rotates, translates,
 * or scales the loaded model appropriately.
 */
//------------------------------------------------------------------------------
void VisualModelPanel::OnSlide(wxCommandEvent &event)
{
   int id = event.GetId();
   float x,y,z;
   switch (id)
   {
      case ID_ROT_SLIDER:
         x = (float)xRotSlider->GetValue();
         y = (float)yRotSlider->GetValue();
         z = (float)zRotSlider->GetValue();
         //modelCanvas->Rotate(true, x, y, z);
         currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelRotationX"), x);
         currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelRotationY"), y);
         currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelRotationZ"), z);
			xRotValueText->SetLabel(wxString::Format(wxT("%f"), x));
			yRotValueText->SetLabel(wxString::Format(wxT("%f"), y));
			zRotValueText->SetLabel(wxString::Format(wxT("%f"), z));
			modelCanvas->Refresh(false);
         break;
      case ID_TRAN_SLIDER:
         x = (float)xTranSlider->GetValue()/100.0f;
         y = (float)yTranSlider->GetValue()/100.0f;
         z = (float)zTranSlider->GetValue()/100.0f;
         //modelCanvas->Translate(x, y, z);
         currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetX"), x);
         currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetY"), y);
         currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetZ"), z);
			xTranValueText->SetLabel(wxString::Format(wxT("%f"), x));
			yTranValueText->SetLabel(wxString::Format(wxT("%f"), y));
			zTranValueText->SetLabel(wxString::Format(wxT("%f"), z));
			modelCanvas->Refresh(false);
         break;
      case ID_SCALE_SLIDER:
         x = (float)scaleSlider->GetValue();
         // We want to scale down if the value is negative, not invert
         if (x < 0){
            int m = scaleSlider->GetMax();
            x = (float)(m+x)/m;
         }
         else if (x == 0)
            break;
         //modelCanvas->Scale(x, x, x);
			scaleValueText->SetLabel(wxString::Format(wxT("%f"), x));
			currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelScale"), x);
			modelCanvas->Refresh(false);
			break;
   }
   dataChanged = true;
	theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Processes the the events where the filename text ctrl changes.
 */
//------------------------------------------------------------------------------
void VisualModelPanel::OnTextCtrlChange(wxCommandEvent& event)
{
    mTextChanged = true;
	dataChanged = true;
	theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnTextCtrlEnter(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Processes the the events where the filename text ctrl changes.
 */
//------------------------------------------------------------------------------
void VisualModelPanel::OnTextCtrlEnter(wxCommandEvent& event)
{
	int id = event.GetId();
	UpdateTextCtrl(id);
	dataChanged = true;
	theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void UpdateTextCtrl(int id)
//------------------------------------------------------------------------------
/**
 * Updates control based on text in text ctrl
 */
//------------------------------------------------------------------------------
void VisualModelPanel::UpdateTextCtrl(int id)
{
	double x,y,z;
	switch (id)
	{
    case (ID_TEXTCTRL):
            // Load the model indicated by the path
            modelCanvas->LoadModel(modelTextCtrl->GetLabelText());
            currentSpacecraft->modelFile = modelTextCtrl->GetLabelText();
            break;
	case (ID_ROT_TEXT):
			xRotValueText->GetLabelText().ToDouble(&x);
			yRotValueText->GetLabelText().ToDouble(&y);
			zRotValueText->GetLabelText().ToDouble(&z);
			x = x < -180? -180 : x > 180? 180 : x;
			y = y < -180? -180 : y > 180? 180 : y;
			z = z < -180? -180 : z > 180? 180 : z;
			//modelCanvas->Rotate(true, x, y, z);
			currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelRotationX"), x);
			currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelRotationY"), y);
			currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelRotationZ"), z);
			xRotValueText->SetLabel(wxString::Format(wxT("%f"), x));
			yRotValueText->SetLabel(wxString::Format(wxT("%f"), y));
			zRotValueText->SetLabel(wxString::Format(wxT("%f"), z));
			xRotSlider->SetValue((int)x);
			yRotSlider->SetValue((int)y);
			zRotSlider->SetValue((int)z);
			modelCanvas->Refresh(false);
			break;
		case (ID_TRAN_TEXT):
			xTranValueText->GetLabelText().ToDouble(&x);
			yTranValueText->GetLabelText().ToDouble(&y);
			zTranValueText->GetLabelText().ToDouble(&z);
			x = x < -3.5? -3.5 : x > 3.5? 3.5 : x;
			y = y < -3.5? -3.5 : y > 3.5? 3.5 : y;
			z = z < -3.5? -3.5 : z > 3.5? 3.5 : z;
			//modelCanvas->Translate(x, y, z);
			currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetX"), x);
			currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetY"), y);
			currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetZ"), z);
			xTranValueText->SetLabel(wxString::Format(wxT("%f"), x));
			yTranValueText->SetLabel(wxString::Format(wxT("%f"), y));
			zTranValueText->SetLabel(wxString::Format(wxT("%f"), z));
			xTranSlider->SetValue((int)x*100);
			yTranSlider->SetValue((int)y*100);
			zTranSlider->SetValue((int)z*100);
			modelCanvas->Refresh(false);
			break;
		case (ID_SCALE_TEXT):
			scaleValueText->GetLabelText().ToDouble(&x);
			x = x <= 0? 0.001 : x > 1000? 1000 : x;
			//modelCanvas->Scale(x, x, x);
			currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelScale"), x);
			scaleValueText->SetLabel(wxString::Format(wxT("%f"), x));
			if (x < 1)
			{
				int m = scaleSlider->GetMax();
				x = x*m-m;
			}
			scaleSlider->SetValue(x);
			modelCanvas->Refresh(false);
			break;
	}
}

//------------------------------------------------------------------------------
// void OnBrowseButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * This function occurs when the Browse button is pushed by the user.
 * It opens up a file dialog that allows the user to select a model.
 */
//------------------------------------------------------------------------------
void VisualModelPanel::OnBrowseButton(wxCommandEvent& event)
{
   wxString filename, path;
   // Open up a file dialog
   wxFileDialog fileDialog(this, wxT("Please select a model."), modelPath, wxEmptyString,
      wxT("3DS and POV files (*.3ds;*.pov)|*.3ds;*.pov"), wxOPEN);
   //wxFileDialog fileDialog(this, wxT("Please select a 3ds model."), modelPath, wxEmptyString,
      //wxT("3DS files (*.3ds)|*.3ds"), wxOPEN);
   // If it succeeded...
   if (fileDialog.ShowModal() == wxID_OK)
   {
      // Load the model indicated by the path
      filename = fileDialog.GetFilename();
      path = fileDialog.GetPath();
      modelCanvas->LoadModel(path);
      currentSpacecraft->modelFile = path;
      // Reset all of the sliders
      ResetSliders();
      dataChanged = true;
      theScPanel->EnableUpdate(true);
      // Set the textctrl to display the selected filename
      modelTextCtrl->ChangeValue(wxT(filename));
	  dataChanged = true;
	  theScPanel->EnableUpdate(true);
	  ToggleInterface(true);
   }
}

//------------------------------------------------------------------------------
// void OnRecenterButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * This function triggers when the recenter button is pushed.
 * It resets the translation sliders and recenters the model based on its
 * axis-aligned bounding box.
 */
//------------------------------------------------------------------------------
void VisualModelPanel::OnRecenterButton(wxCommandEvent& event)
{
   float offset[3];
   modelCanvas->RecenterModel(offset);
   xTranSlider->SetValue(offset[0]*100);
   yTranSlider->SetValue(offset[1]*100);
   zTranSlider->SetValue(offset[2]*100);
   xTranValueText->SetLabel(wxString::Format(wxT("%f"), offset[0]));
   yTranValueText->SetLabel(wxString::Format(wxT("%f"), offset[1]));
   zTranValueText->SetLabel(wxString::Format(wxT("%f"), offset[2]));
   currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetX"), offset[0]);
   currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetY"), offset[1]);
   currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetZ"), offset[2]);
   modelCanvas->Refresh(false);
   dataChanged = true;

	theScPanel->EnableUpdate(true);
}

void VisualModelPanel::OnAutoscaleButton(wxCommandEvent& event)
{
	float scale = modelCanvas->AutoscaleModel();
	scaleSlider->SetValue(scale);
	scaleValueText->SetLabel(wxString::Format(wxT("%f"), scale));
	currentSpacecraft->SetRealParameter(currentSpacecraft->GetParameterID("ModelScale"), scale);
	modelCanvas->Refresh(false);
	dataChanged = true;
 	theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnEarthButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * When the Show Earth button is pushed, it toggles whether or not the wireframe
 * Earth is painted or not.
 */
//------------------------------------------------------------------------------
void VisualModelPanel::OnEarthButton(wxCommandEvent& event)
{
   if (showEarthButton->GetLabel() == wxT("Show Earth"))
      showEarthButton->SetLabel(wxT("Hide Earth"));
   else
      showEarthButton->SetLabel(wxT("Show Earth"));
   modelCanvas->ToggleEarth();
}

//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
wxString VisualModelPanel::ToString(Real rval)
{
   return theGuiManager->ToWxString(rval);
}

void VisualModelPanel::SaveData()
{
   // don't do anything if no data has been changed.
   // note that dataChanged will be true if the user modified any combo box or
   // text ctrl, whether or not he/she actually changed the value; we want to only
   // send the values to the object if something was really changed, to avoid
   // the hasBeenModified flag being set to true erroneously
   canClose    = true;

   try
   {
      if (mTextChanged)
      {
        UpdateTextCtrl(ID_ROT_TEXT);
        UpdateTextCtrl(ID_TRAN_TEXT);
        UpdateTextCtrl(ID_SCALE_TEXT);
        UpdateTextCtrl(ID_TEXTCTRL);
        mTextChanged = false;
      }
   }
   catch (BaseException &ex)
   {
      canClose = false;
      dataChanged = true;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }

   if (canClose)
   {
      dataChanged = false;
   }
}

