//$Header$
//------------------------------------------------------------------------------
//                              XyPlotSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/02
/**
 * Implements XyPlotSetupPanel class.
 */
//------------------------------------------------------------------------------

#include "XyPlotSetupPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

#include "wx/colordlg.h"   // for wxColourDialog

#define DEBUG_XYPLOT_PANEL 0

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(XyPlotSetupPanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
    
   EVT_BUTTON(ADD_X, XyPlotSetupPanel::OnAddX)
   EVT_BUTTON(ADD_Y, XyPlotSetupPanel::OnAddY)
   EVT_BUTTON(REMOVE_X, XyPlotSetupPanel::OnRemoveX)
   EVT_BUTTON(REMOVE_Y, XyPlotSetupPanel::OnRemoveY)
   EVT_BUTTON(CLEAR_Y, XyPlotSetupPanel::OnClearY)
   EVT_BUTTON(LINE_COLOR_BUTTON, XyPlotSetupPanel::OnLineColorClick)
   EVT_LISTBOX(Y_SEL_LISTBOX, XyPlotSetupPanel::OnSelectY)
   EVT_CHECKBOX(PLOT_CHECKBOX, XyPlotSetupPanel::OnPlotCheckBoxChange)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------
//------------------------------------------------------------------------------
// XyPlotSetupPanel(wxWindow *parent, const wxString &subscriberName)
//------------------------------------------------------------------------------
/**
 * Constructs XyPlotSetupPanel object.
 *
 * @param <parent> input parent.
 *
 * @note Creates the XyPlotSetupPanel GUI
 */
//------------------------------------------------------------------------------
XyPlotSetupPanel::XyPlotSetupPanel(wxWindow *parent,
                                   const wxString &subscriberName)
   : GmatPanel(parent)
{
   //MessageInterface::ShowMessage("XyPlotSetupPanel() entering...\n");
   //MessageInterface::ShowMessage("XyPlotSetupPanel() subscriberName = " +
   //                              std::string(subscriberName.c_str()) + "\n");
    
   mSubscriber =
      theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));

   mXyPlot = (XyPlot*)mSubscriber;
   
   mXParamChanged = false;
   mYParamChanged = false;
   mIsColorChanged = false;
   mNumXParams = 0;
   mNumYParams = 0;
    
   mLineColor.Set(0,0,0); //loj: 6/2/04 set to Black for now
   mColorMap.clear();
   
   Create();
   Show();

    // force saving data
   if (mNumXParams == 0 || mNumYParams <= 0)
   {
      theApplyButton->Enable();
      mXParamChanged = true;
      mYParamChanged = true;
   }
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnAddX(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnAddX(wxCommandEvent& event)
{
   wxString oldParam = xSelectedListBox->GetStringSelection();
   wxString newParam = paramListBox->GetStringSelection();

   if (!oldParam.IsSameAs(newParam))
   {
      // empty listbox first, only one parameter is allowed
      xSelectedListBox->Clear();
      xSelectedListBox->Append(newParam);

      mXParamChanged = true;
      theApplyButton->Enable();
   }
}

//------------------------------------------------------------------------------
// void OnAddY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnAddY(wxCommandEvent& event)
{
   // get string in first list and then search for it
   // in the second list
   wxString s = paramListBox->GetStringSelection();
   int found = ySelectedListBox->FindString(s);
    
    // if the string wasn't found in the second list, insert it
   if (found == wxNOT_FOUND)
   {
      ySelectedListBox->Append(s);
      ySelectedListBox->SetStringSelection(s);
      ShowParameterOption(s, true);
      mYParamChanged = true;
      theApplyButton->Enable();
   }
}

//------------------------------------------------------------------------------
// void OnRemoveX(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnRemoveX(wxCommandEvent& event)
{
   int sel = xSelectedListBox->GetSelection();
   xSelectedListBox->Delete(sel);
   mXParamChanged = true;
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnRemoveY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnRemoveY(wxCommandEvent& event)
{
   int sel = ySelectedListBox->GetSelection();
   ySelectedListBox->Delete(sel);
   
   if (sel-1 < 0)
   {
      ySelectedListBox->SetSelection(0);
      if (ySelectedListBox->GetCount() == 0)
         ShowParameterOption("", false); // hide parameter color, etc
      else
         ShowParameterOption(ySelectedListBox->GetStringSelection(), true);
   }
   else
   {
      ySelectedListBox->SetSelection(sel-1);
      ShowParameterOption(ySelectedListBox->GetStringSelection(), true);
   }

   mYParamChanged = true;
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnClearY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnClearY(wxCommandEvent& event)
{
   ySelectedListBox->Clear();
   ShowParameterOption("", false);
   mYParamChanged = true;
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnSelectY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnSelectY(wxCommandEvent& event)
{
   ShowParameterOption(ySelectedListBox->GetStringSelection(), true);
}

//------------------------------------------------------------------------------
// void OnPlotCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnPlotCheckBoxChange(wxCommandEvent& event)
{
   theApplyButton->Enable();
}

//------------------------------------------------------------------------------
// void OnLineColorClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnLineColorClick(wxCommandEvent& event)
{
   wxColourData data;
   data.SetColour(mLineColor);
   wxColourDialog dialog(this, &data);
   //dialog.CenterOnParent();
   dialog.Center();
   
   if (dialog.ShowModal() == wxID_OK)
   {
      mSelYName = std::string(ySelectedListBox->GetStringSelection().c_str());
      
      mLineColor = dialog.GetColourData().GetColour();
      mLineColorButton->SetBackgroundColour(mLineColor);
      
      mColorMap[mSelYName].Set(mLineColor.Red(),
                               mLineColor.Green(),
                               mLineColor.Blue());
      
      theApplyButton->Enable();
      mIsColorChanged = true;
   }
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::Create()
{
   //MessageInterface::ShowMessage("XyPlotSetupPanel::Create() entering...\n");
   wxString emptyList[] = {};
   Integer bsize = 3; // border size
   
   wxBoxSizer *pageBoxSizer = new wxBoxSizer(wxVERTICAL);
   mFlexGridSizer = new wxFlexGridSizer(5, 0, 0);

   // empty text for spacing line
   wxStaticText *emptyText =
      new wxStaticText(this, -1, wxT(""),
                       wxDefaultPosition, wxSize(-1, -1), 0);
   
   //------------------------------------------------------
   // plot option
   //------------------------------------------------------
   plotCheckBox =
      new wxCheckBox(this, PLOT_CHECKBOX, wxT("Show Plot"),
                     wxDefaultPosition, wxSize(100, -1), 0);
            
   wxBoxSizer *plotOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   plotOptionBoxSizer->Add(plotCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
        
   //------------------------------------------------------
   // X box label (1st column)
   //------------------------------------------------------
   wxStaticText *titleXText =
      new wxStaticText(this, -1, wxT("Selected X"),
                       wxDefaultPosition, wxSize(80,-1), 0);
    
   xSelectedListBox =
      new wxListBox(this, LISTBOX, wxDefaultPosition,
                    wxSize(150,200), 0, emptyList, wxLB_SINGLE);
    
   wxBoxSizer *xSelelectedBoxSizer = new wxBoxSizer(wxVERTICAL);
   xSelelectedBoxSizer->Add(titleXText, 0, wxALIGN_CENTRE|wxALL, bsize);
   xSelelectedBoxSizer->Add(xSelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
            
    //------------------------------------------------------
    // add, remove X buttons (2nd column)
    //------------------------------------------------------
   addXButton = new wxButton(this, ADD_X, wxT("<--"),
                             wxDefaultPosition, wxSize(20,20), 0);

   removeXButton = new wxButton(this, REMOVE_X, wxT("-->"),
                                wxDefaultPosition, wxSize(20,20), 0);

   wxBoxSizer *xButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   xButtonsBoxSizer->Add(30, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   xButtonsBoxSizer->Add(addXButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   xButtonsBoxSizer->Add(removeXButton, 0, wxALIGN_CENTRE|wxALL, bsize);
      
   //------------------------------------------------------
   // parameters box (3rd column)
   //------------------------------------------------------
   wxBoxSizer *paramBoxSizer = new wxBoxSizer(wxVERTICAL);
    
   wxStaticText *titleAvailbleText =
      new wxStaticText(this, -1, wxT("Variables"),
                       wxDefaultPosition, wxSize(80,-1), 0);
    
   theGuiManager->UpdateParameter();
    
   paramListBox =
      theGuiManager->GetConfigParameterListBox(this, wxSize(150,200));
   paramListBox->SetSelection(0);
   paramBoxSizer->Add(titleAvailbleText, 0, wxALIGN_CENTRE|wxALL, bsize);
   paramBoxSizer->Add(paramListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // add, remove, clear Y buttons (4th column)
   //------------------------------------------------------
   addYButton = new wxButton(this, ADD_Y, wxT("-->"),
                             wxDefaultPosition, wxSize(20,20), 0);

   removeYButton = new wxButton(this, REMOVE_Y, wxT("<--"),
                                wxDefaultPosition, wxSize(20,20), 0);
    
   clearYButton = new wxButton(this, CLEAR_Y, wxT("<="),
                               wxDefaultPosition, wxSize(20,20), 0);
    
   wxBoxSizer *yButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
   yButtonsBoxSizer->Add(addYButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   yButtonsBoxSizer->Add(removeYButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   yButtonsBoxSizer->Add(clearYButton, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // Y box label (5th column)
   //------------------------------------------------------
   wxStaticText *titleYText =
      new wxStaticText(this, -1, wxT("Selected Y"),
                       wxDefaultPosition, wxSize(80,-1), 0);

   ySelectedListBox = new wxListBox(this, Y_SEL_LISTBOX, wxDefaultPosition,
                                    wxSize(150,200), 0, emptyList, wxLB_SINGLE);
        
   wxBoxSizer *ySelelectedBoxSizer = new wxBoxSizer(wxVERTICAL);
   ySelelectedBoxSizer->Add(titleYText, 0, wxALIGN_CENTRE|wxALL, bsize);
   ySelelectedBoxSizer->Add(ySelectedListBox, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   //------------------------------------------------------
   // line color (6th column)
   //------------------------------------------------------
   wxStaticText *titleColor =
      new wxStaticText(this, -1, wxT("Color"),
                       wxDefaultPosition, wxSize(40,20), wxALIGN_CENTRE);
   
   mLineColorButton = new wxButton(this, LINE_COLOR_BUTTON, "",
                                   wxDefaultPosition, wxSize(25,20), 0);

   mLineColorButton->SetBackgroundColour(mLineColor);
   wxFlexGridSizer *paramOptionBoxSizer1 = new wxFlexGridSizer(2, 0, 0);
   paramOptionBoxSizer1->Add(titleColor, 0, wxALIGN_LEFT|wxALL, bsize);
   paramOptionBoxSizer1->Add(mLineColorButton, 0, wxALIGN_LEFT|wxALL, bsize);

   mParamOptionBoxSizer = new wxBoxSizer(wxVERTICAL);
   mParamOptionBoxSizer->Add(paramOptionBoxSizer1, 0, wxALIGN_LEFT|wxALL, bsize);

   //------------------------------------------------------
   // put in the order
   //------------------------------------------------------
   mFlexGridSizer->Add(xSelelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(xButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(paramBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(yButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   mFlexGridSizer->Add(ySelelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   mFlexGridSizer->Add(plotOptionBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   mFlexGridSizer->Add(emptyText, 0, wxALIGN_LEFT|wxALL, bsize);
   mFlexGridSizer->Add(emptyText, 0, wxALIGN_LEFT|wxALL, bsize);
   mFlexGridSizer->Add(emptyText, 0, wxALIGN_LEFT|wxALL, bsize);
   mFlexGridSizer->Add(mParamOptionBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
    
   pageBoxSizer->Add(mFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   //pageBoxSizer->Add(plotOptionBoxSizer, 0, wxALIGN_LEFT|wxALL, bsize);
        
   //------------------------------------------------------
   // add to parent sizer
   //------------------------------------------------------
   theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);

}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::LoadData()
{
   // load data from the core engine

   //loj: 3/18/04 added try block
   try
   {
      //MessageInterface::ShowMessage("XyPlotSetupPanel::LoadData() entered\n");
    
      plotCheckBox->SetValue(mSubscriber->IsActive());
    
      // get X parameter
      wxString *xParamNames = new wxString[1];
      xParamNames[0] = mSubscriber->GetStringParameter("IndVar").c_str();
        
      //MessageInterface::ShowMessage("XyPlotSetupPanel::LoadData() xParamNames[0] = %s\n",
      //                              xParamNames[0].c_str());
        
      if (!xParamNames[0].IsSameAs(""))
      {
         mNumXParams = 1;
         xSelectedListBox->Set(1, xParamNames);
      }
    
      // get Y parameters
      StringArray yParamList = mSubscriber->GetStringArrayParameter("DepVarList");
      mNumYParams = yParamList.size();
      //MessageInterface::ShowMessage("XyPlotSetupPanel::LoadData() mNumYParams = %d\n",
      //                              mNumYParams);

      delete xParamNames;
    
      if (mNumYParams > 0)
      {
         wxString *yParamNames = new wxString[mNumYParams];
         Parameter *param;
         
         for (int i=0; i<mNumYParams; i++)
         {
#if DEBUG_XYPLOT_PANEL
            MessageInterface::ShowMessage("XyPlotSetupPanel::LoadData() y param = %s\n",
                                          yParamList[i].c_str());
#endif
            yParamNames[i] = yParamList[i].c_str();

            // get parameter color
            //mColorMap[yParamList[i]]
            //   = RgbColor(mXyPlot->GetUnsignedIntParameter("Color", yParamList[i]));

            param = theGuiInterpreter->GetParameter(yParamList[i]);
            if (param != NULL)
            {
               mColorMap[yParamList[i]]
                  = RgbColor(param->GetUnsignedIntParameter("Color"));               
            }
         }
    
         ySelectedListBox->Set(mNumYParams, yParamNames);
         ySelectedListBox->SetSelection(0);
         delete yParamNames;

         // show parameter option
         ShowParameterOption(ySelectedListBox->GetStringSelection(), true);
      }
      else
      {
         ShowParameterOption("", false);
      }
   }
   catch (...)
   {
      MessageInterface::ShowMessage("XyPlotSetupPanel:LoadData() Unknown error occurred!\n");
   }
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::SaveData()
{
   // save data to core engine
    
   mSubscriber->Activate(plotCheckBox->IsChecked());

   // set X parameter
   if (mXParamChanged)
   {
      if (xSelectedListBox->GetCount() == 0 && plotCheckBox->IsChecked())
      {
         wxLogMessage(wxT("X parameter not selected. The plot will not be activated."));
         mSubscriber->Activate(false);
      }
      else
      {
         mSubscriber->
            SetStringParameter("IndVar",
                               std::string(xSelectedListBox->GetString(0).c_str()));
      }

      mXParamChanged = false;
   }

   // set Y parameters
   if (mYParamChanged)
   {
      mYParamChanged = false;
      mIsColorChanged = true;
      
      int numYParams = ySelectedListBox->GetCount();

      mNumYParams = numYParams;
      if (mNumYParams == 0 && plotCheckBox->IsChecked())
      {
         wxLogMessage(wxT("Y parameters not selected. The plot will not be activated."));
         mSubscriber->Activate(false);
      }
      else if (numYParams > 6)
      {
         wxLogMessage(wxT("Selected Y parameter count is greater than 6.\n"
                          "First 6 parameters will be plotted"));

         mNumYParams = 6;
      }
      else
      {
         numYParams = numYParams;
      }

      if (mNumYParams >= 0) // >=0 because the list needs to be cleared
      {
         mSubscriber->SetBooleanParameter("ClearDepVarList", true);
         for (int i=0; i<mNumYParams; i++)
         {
#if DEBUG_XYPLOT_PANEL
            MessageInterface::ShowMessage("XyPlotSetupPanel::SaveData() DepVar = %s\n",
                                          ySelectedListBox->GetString(i).c_str());
#endif
            mSubscriber->
               SetStringParameter("Add",
                                  std::string(ySelectedListBox->GetString(i).c_str()));
         }
      }
   }
   
   // save color
   if (mIsColorChanged)
   {
      mIsColorChanged = false;
      Parameter *param;
      UnsignedInt intColor;
      
      for (int i=0; i<mNumYParams; i++)
      {
         mSelYName = std::string(ySelectedListBox->GetString(i).c_str());
         param = theGuiInterpreter->GetParameter(mSelYName);

         if (param != NULL)
         {
            intColor = mColorMap[mSelYName].GetIntColor();
            param->SetUnsignedIntParameter("Color", intColor);
         }
         
         //mXyPlot->
         //   SetUnsignedIntParameter("Color", mSelYName,
         //                           mColorMap[mSelYName].GetIntColor());
      }
   }
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void ShowParameterOption(const wxString &name, bool show = true)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::ShowParameterOption(const wxString &name, bool show)
{
#if DEBUG_XYPLOT_PANEL
   MessageInterface::ShowMessage
      ("XyPlotSetupPanel::ShowParameterOption() name=%s\n", name.c_str());
#endif

   if (!name.IsSameAs(""))
   {
      mSelYName = std::string(name.c_str());
      RgbColor color;
      
      // if parameter not in the map
      if (mColorMap.find(mSelYName) == mColorMap.end())
      {
         Parameter *param = theGuiInterpreter->GetParameter(mSelYName);

         if (param != NULL)
         {
            UnsignedInt intColor = param->GetUnsignedIntParameter("Color");
            color.Set(intColor);

            mColorMap[mSelYName] = color;
         }
      }
      else
      {
         color = mColorMap[mSelYName];
      }
      
#if DEBUG_XYPLOT_PANEL
      MessageInterface::ShowMessage("ShowParameterOption() intColor=%d",
                                    mColorMap[mSelYName].GetIntColor());
#endif
      mLineColor.Set(color.Red(), color.Green(), color.Blue());
      mLineColorButton->SetBackgroundColour(mLineColor);
      
      mFlexGridSizer->Show(mParamOptionBoxSizer, show);
   }
   else
   {
      mFlexGridSizer->Show(mParamOptionBoxSizer, false);
   }
   
   mFlexGridSizer->Layout();
}
