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
#include "ParameterSelectDialog.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ForLoopPanel, GmatPanel)
    EVT_TEXT(ID_TEXTCTRL, ForLoopPanel::OnTextUpdate)
    EVT_BUTTON(ID_BUTTON, ForLoopPanel::OnButtonClick)
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
   mIncrValue = 0;
   mEndValue = 0;   
   
   mIndexIsParam = false;
   mStartIsParam = false;
   mEndIsParam = false;
   mIncrIsParam = false;
   
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
   incrStaticText =
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
   incrTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0);
   endTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(250,-1), 0);
   indexTextCtrl->SetEditable(false);
                      
   // wxButton
   indexButton =
      new wxButton(this, ID_BUTTON, wxT("View"),
                    wxDefaultPosition, wxSize(75, -1), 0);
   startButton =
      new wxButton(this, ID_BUTTON, wxT("View"),
                    wxDefaultPosition, wxSize(75, -1), 0);
   stepButton =
      new wxButton(this, ID_BUTTON, wxT("View"),
                    wxDefaultPosition, wxSize(75, -1), 0);
   endButton =
      new wxButton(this, ID_BUTTON, wxT("View"),
                    wxDefaultPosition, wxSize(75, -1), 0);
                      
   Integer bsize = 10; // border size
   
   // wx*Sizers
   wxFlexGridSizer *flexGridSizer1 = new wxFlexGridSizer( 4, 0, 0 );
   
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( indexStaticText, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( indexTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( indexButton, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( startStaticText, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( startTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize); 
   flexGridSizer1->Add( startButton, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
    
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize); 
   flexGridSizer1->Add( incrStaticText, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( incrTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( stepButton, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   flexGridSizer1->Add( 20, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( endStaticText, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( endTextCtrl, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   flexGridSizer1->Add( endButton, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
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
       wxString s1, s2, s3;

       paramId = theForCommand->GetParameterID("IndexName");
       mIndexString = theForCommand->GetStringParameter(paramId).c_str();

       if (mIndexString.IsEmpty())
       {
          mIndexString = "Default set to internal index";
          mIndexIsSet = false;
       } 
       else
       {
          mIndexIsSet = true;  
          
          paramId = theForCommand->GetParameterID("IndexIsParam");
          mIndexIsParam = theForCommand->GetBooleanParameter(paramId);
          
          if (mIndexIsParam)
          {
             Parameter *param = (Parameter*)theForCommand->GetRefObject
                             (Gmat::PARAMETER, mIndexString.c_str());
                
             mIndexString = param->GetName().c_str();
          }  
       } 
       indexTextCtrl->SetValue(mIndexString);     
    
       paramId = theForCommand->GetParameterID("StartIsParam");
       mStartIsParam = theForCommand->GetBooleanParameter(paramId);
          
       if (mStartIsParam)
       {
           paramId = theForCommand->GetParameterID("StartName");
           mStartString = theForCommand->GetStringParameter(paramId).c_str();
 
           Parameter *param = (Parameter*)theForCommand->GetRefObject
                             (Gmat::PARAMETER, mStartString.c_str());
           
           mStartString = param->GetName().c_str(); 
           startTextCtrl->SetValue(mStartString);
       }
       else
       {    
          paramId = theForCommand->GetParameterID("StartValue");
          mStartValue = theForCommand->GetRealParameter(paramId);
          s1.Printf("%.10f", mStartValue);
          startTextCtrl->SetValue(s1);
       }    
       
       paramId = theForCommand->GetParameterID("IncrIsparam");
       mIncrIsParam = theForCommand->GetBooleanParameter(paramId);

       if (mIncrIsParam)
       {
           paramId = theForCommand->GetParameterID("IncrementName");
           mIncrString = theForCommand->GetStringParameter(paramId).c_str();
           
           Parameter *param = (Parameter*)theForCommand->GetRefObject
                             (Gmat::PARAMETER, mIncrString.c_str());
           mIncrString = param->GetName().c_str();
           incrTextCtrl->SetValue(mIncrString);
       }
       else
       {    
          paramId = theForCommand->GetParameterID("Step");
          mIncrValue = theForCommand->GetRealParameter(paramId);
          s2.Printf("%.10f", mIncrValue);
          incrTextCtrl->SetValue(s2);
       }    
       
       paramId = theForCommand->GetParameterID("EndIsParam");
       mEndIsParam = theForCommand->GetBooleanParameter(paramId);
       
       if (mEndIsParam)
       {
           paramId = theForCommand->GetParameterID("EndName");
           mEndString = theForCommand->GetStringParameter(paramId).c_str();
           
           Parameter *param = (Parameter*)theForCommand->GetRefObject
                             (Gmat::PARAMETER, mEndString.c_str());
           mEndString = param->GetName().c_str();
           endTextCtrl->SetValue(mEndString); 
       }
       else
       {    
          paramId = theForCommand->GetParameterID("EndValue");
          mEndValue = theForCommand->GetRealParameter(paramId);
          s3.Printf("%.10f", mEndValue);
          endTextCtrl->SetValue(s3); 
       }   
    }
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ForLoopPanel::SaveData()
{
    Integer paramId;
    
    if (mIndexIsSet)
    {
       if (mIndexIsParam)
       {    
          paramId = theForCommand->GetParameterID("IndexName");
          theForCommand->SetStringParameter(paramId, mIndexString.c_str());
          Parameter* theParam = theGuiInterpreter->GetParameter(mIndexString.c_str());
          theForCommand->SetRefObject(theParam, Gmat::PARAMETER, mIndexString.c_str());
       }    
    }    
    
    if (mStartIsParam)
    {
       paramId = theForCommand->GetParameterID("StartName");
       theForCommand->SetStringParameter(paramId, mStartString.c_str());
       Parameter* theParam = theGuiInterpreter->GetParameter(mStartString.c_str());
       theForCommand->SetRefObject(theParam, Gmat::PARAMETER, mStartString.c_str());
    }
    else
    {    
       paramId = theForCommand->GetParameterID("StartValue");
       theForCommand->SetRealParameter(paramId, mStartValue);
    }   
    
    if (mIncrIsParam)
    {
       paramId = theForCommand->GetParameterID("IncrementName");
       theForCommand->SetStringParameter(paramId, mIncrString.c_str());
       Parameter* theParam = theGuiInterpreter->GetParameter(mIncrString.c_str());
       theForCommand->SetRefObject(theParam, Gmat::PARAMETER, mIncrString.c_str());
    }
    else
    {        
       paramId = theForCommand->GetParameterID("Step");
       theForCommand->SetRealParameter(paramId, mIncrValue);
    }    
    
    if (mEndIsParam)
    {
       paramId = theForCommand->GetParameterID("EndName");
       theForCommand->SetStringParameter(paramId, mEndString.c_str());
       Parameter* theParam = theGuiInterpreter->GetParameter(mEndString.c_str());
       theForCommand->SetRefObject(theParam, Gmat::PARAMETER, mEndString.c_str());
    }
    else
    {    
       paramId = theForCommand->GetParameterID("EndValue");
       theForCommand->SetRealParameter(paramId, mEndValue);
    }    
}

//------------------------------------------------------------------------------
// void OnTextUpdate(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ForLoopPanel::OnTextUpdate(wxCommandEvent& event)
{
   wxString temp;
   
   if (event.GetEventObject() == indexTextCtrl)
   {
      temp = indexTextCtrl->GetValue();
      Parameter* theParam = theGuiInterpreter->GetParameter(temp.c_str());
      
      if (theParam != NULL)
      {
         mIndexString = indexTextCtrl->GetValue();
         mIndexIsParam = true; 
         mIndexIsSet = true;   
      } 
   }    
   else if (event.GetEventObject() == startTextCtrl)
   {
      temp = startTextCtrl->GetValue();
      Parameter* theParam = theGuiInterpreter->GetParameter(temp.c_str());

      if (theParam == NULL)
      {
          mStartValue = atof(startTextCtrl->GetValue());
          mStartIsParam = false;
      } 
      else
      {
         mStartString = startTextCtrl->GetValue();
         mStartIsParam = true;    
      }    
   }    
   else if (event.GetEventObject() == incrTextCtrl)
   {
      temp = incrTextCtrl->GetValue();
      Parameter* theParam = theGuiInterpreter->GetParameter(temp.c_str());

      if (theParam == NULL)
      {
          mIncrValue = atof(incrTextCtrl->GetValue());
          mIncrIsParam = false;
      } 
      else
      {
         mIncrString = incrTextCtrl->GetValue();
         mIncrIsParam = true;    
      } 
   }    
   else if (event.GetEventObject() == endTextCtrl)
   {
      temp = endTextCtrl->GetValue();
      Parameter* theParam = theGuiInterpreter->GetParameter(temp.c_str());

      if (theParam == NULL)
      {
          mEndValue = atof(endTextCtrl->GetValue());
          mEndIsParam = false;
      } 
      else
      {
         mEndString = endTextCtrl->GetValue();
         mEndIsParam = true;    
      } 
   }    
   
   theApplyButton->Enable(true);
}   

//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ForLoopPanel::OnButtonClick(wxCommandEvent& event)
{
   ParameterSelectDialog paramDlg(this);
   paramDlg.ShowModal();

   if (paramDlg.IsParamSelected())
   {
      wxString newParamName = paramDlg.GetParamName();
      
      if (event.GetEventObject() == indexButton)
         indexTextCtrl->SetValue(newParamName);    
      else if (event.GetEventObject() == startButton)
         startTextCtrl->SetValue(newParamName);
      else if (event.GetEventObject() == stepButton)
         incrTextCtrl->SetValue(newParamName);
      else if (event.GetEventObject() == endButton)
         endTextCtrl->SetValue(newParamName); 
   }      
}    
