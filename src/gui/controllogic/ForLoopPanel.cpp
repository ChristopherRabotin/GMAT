//$Header$
//------------------------------------------------------------------------------
//                              ForLoopPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/05/17
/**
 * This class contains the Conditional Statement Setup window.
 */
//------------------------------------------------------------------------------
#include "ForLoopPanel.hpp"

// gui includes
#include "gmatwxdefs.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatAppData.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ForLoopPanel, GmatPanel)
    EVT_TEXT(ID_TEXTCTRL, ForLoopPanel::OnTextUpdate)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ForLoopPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ForLoopPanel::ForLoopPanel(wxWindow *parent, GmatCommand *cmd)
    : GmatPanel(parent)
{
   theForCommand = (For *)cmd;
   
   mStartValue = 0;
   mStepSize = 0;
   mEndValue = 0;   
   
   Create();
   Show();
}

ForLoopPanel::~ForLoopPanel()
{
}

//-------------------------------
// private methods
//-------------------------------
void ForLoopPanel::Create()
{
    Setup(this);    
}

void ForLoopPanel::Setup( wxWindow *parent)
{
   // wxStaticText
   indexStaticText =
      new wxStaticText(this, ID_TEXT , wxT("Index"),
                        wxDefaultPosition, wxDefaultSize, 0);
   startStaticText =
      new wxStaticText(this, ID_TEXT , wxT("Start"),
                        wxDefaultPosition, wxDefaultSize, 0);
   stepStaticText =
      new wxStaticText(this, ID_TEXT , wxT("Increment"), 
                        wxDefaultPosition, wxDefaultSize, 0);
   endStaticText =
      new wxStaticText(this, ID_TEXT , wxT("End"), 
                        wxDefaultPosition, wxDefaultSize, 0);
                        
   // wxTextCtrl
   indexTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(250,-1), 0);
   startTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(250,-1), 0);
   stepTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0);
   endTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(250,-1), 0);
                      
   Integer bsize = 10; // border size
   
   // wx*Sizers
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 3, 0, 0 );
   
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( indexStaticText, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( indexTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( startStaticText, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( startTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);  
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize); 
   flexGridSizer1->Add( stepStaticText, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( stepTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( endStaticText, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( endTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(flexGridSizer1, 0, wxGROW, bsize);
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ForLoopPanel::LoadData()
{
    if (theForCommand != NULL)
    {
       Integer paramId;
       //paramId = theForCommand->GetParameterID("StartValue");
       //mForName = theForCommand->GetRealParameter(paramId);
       paramId = theForCommand->GetParameterID("StartValue");
       mStartValue = theForCommand->GetRealParameter(paramId);
       paramId = theForCommand->GetParameterID("Step");
       mStepSize = theForCommand->GetRealParameter(paramId);
       paramId = theForCommand->GetParameterID("EndValue");
       mEndValue = theForCommand->GetRealParameter(paramId);
    
       wxString s1, s2, s3;
       s1.Printf("%.10f", mStartValue);
       s2.Printf("%.10f", mStepSize);
       s3.Printf("%.10f", mEndValue);
    
       startTextCtrl->SetValue(s1);
       stepTextCtrl->SetValue(s2);
       endTextCtrl->SetValue(s3);
    }
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ForLoopPanel::SaveData()
{
    Integer paramId;
    paramId = theForCommand->GetParameterID("StartValue");
    theForCommand->SetRealParameter(paramId, mStartValue);
    paramId = theForCommand->GetParameterID("Step");
    theForCommand->SetRealParameter(paramId, mStepSize);
    paramId = theForCommand->GetParameterID("EndValue");
    theForCommand->SetRealParameter(paramId, mEndValue);
}

//------------------------------------------------------------------------------
// void OnGravityTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ForLoopPanel::OnTextUpdate(wxCommandEvent& event)
{
   if (event.GetEventObject() == startTextCtrl)
      mStartValue = atof(startTextCtrl->GetValue());
   else if (event.GetEventObject() == stepTextCtrl)
      mStepSize = atof(stepTextCtrl->GetValue());
   else if (event.GetEventObject() == endTextCtrl)
      mEndValue = atof(endTextCtrl->GetValue());
   
   theApplyButton->Enable(true);
}
