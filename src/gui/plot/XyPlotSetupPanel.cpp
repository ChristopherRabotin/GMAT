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

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(XyPlotSetupPanel, GmatPanel)
    EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
    EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
    EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
    EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
    EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
    
    EVT_BUTTON(XY_PLOT_ADD_X, XyPlotSetupPanel::OnAddX)
    EVT_BUTTON(XY_PLOT_ADD_Y, XyPlotSetupPanel::OnAddY)
    EVT_BUTTON(XY_PLOT_REMOVE_X, XyPlotSetupPanel::OnRemoveX)
    EVT_BUTTON(XY_PLOT_REMOVE_Y, XyPlotSetupPanel::OnRemoveY)
    EVT_BUTTON(XY_PLOT_CLEAR_Y, XyPlotSetupPanel::OnClearY)
    EVT_BUTTON(XY_PLOT_CHECKBOX, XyPlotSetupPanel::OnPlotCheckBoxChange)

    EVT_CHECKBOX(XY_PLOT_CHECKBOX, XyPlotSetupPanel::OnPlotCheckBoxChange)
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
    
    theSubscriber =
        theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));

    theParamList = NULL;
    
    Create();
    Show();
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void OnAddX(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnAddX(wxCommandEvent& event)
{
    // empty listbox first, only one parameter is allowed
    xSelectedListBox->Clear();
    
    wxString s = paramListBox->GetStringSelection();
    xSelectedListBox->Append(s);    
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
        ySelectedListBox->Append(s);
    
}

//------------------------------------------------------------------------------
// void OnRemoveX(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnRemoveX(wxCommandEvent& event)
{
    int sel = xSelectedListBox->GetSelection();
    xSelectedListBox->Delete(sel);
}

//------------------------------------------------------------------------------
// void OnRemoveY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnRemoveY(wxCommandEvent& event)
{
    int sel = ySelectedListBox->GetSelection();
    ySelectedListBox->Delete(sel);
}

//------------------------------------------------------------------------------
// void OnClearY(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnClearY(wxCommandEvent& event)
{
    ySelectedListBox->Clear();
}

//------------------------------------------------------------------------------
// void OnPlotCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnPlotCheckBoxChange(wxCommandEvent& event)
{
    theApplyButton->Enable();
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

    pageBoxSizer = new wxBoxSizer(wxVERTICAL);
    paramGridSizer = new wxFlexGridSizer(6, 0, 0);

    // empty text for spacing line
    emptyText = new wxStaticText(this, XY_PLOT_TEXT, wxT(""),
                                 wxDefaultPosition, wxSize(100, -1), 0);
    
    //------------------------------------------------------
    // plot option, (1st column)
    //------------------------------------------------------
    plotCheckBox = new wxCheckBox(this, XY_PLOT_CHECKBOX, wxT("Show Plot"),
                                  wxDefaultPosition, wxSize(100, -1), 0);
            
    optionBoxSizer = new wxBoxSizer(wxVERTICAL);
    optionBoxSizer->Add(plotCheckBox, 0, wxALIGN_LEFT|wxALL, 5);
        
    //------------------------------------------------------
    // X box label (2nd column)
    //------------------------------------------------------
    titleXText = new wxStaticText(this, XY_PLOT_TEXT, wxT("Selected X"),
                                  wxDefaultPosition, wxSize(80,-1), 0);
    
    xSelectedListBox = new wxListBox(this, XY_PLOT_LISTBOX, wxDefaultPosition,
                                     wxSize(150,200), 0, emptyList, wxLB_SINGLE);

    
    // get X parameter
    wxString *xParam = new wxString[1];
    xParam[0] = theSubscriber->GetStringParameter("XParamName").c_str();
    if (xParam[0] != "STRING_PARAMETER_UNDEFINED")
        xSelectedListBox->Set(1, xParam);
    
    xSelelectedBoxSizer = new wxBoxSizer(wxVERTICAL);
    xSelelectedBoxSizer->Add(titleXText, 0, wxALIGN_CENTRE|wxALL, 5);
    xSelelectedBoxSizer->Add(xSelectedListBox, 0, wxALIGN_CENTRE|wxALL, 5);
            
    //------------------------------------------------------
    // add, remove X buttons (3rd column)
    //------------------------------------------------------
    addXButton = new wxButton(this, XY_PLOT_ADD_X, wxT("<--"),
                              wxDefaultPosition, wxSize(20,20), 0);

    removeXButton = new wxButton(this, XY_PLOT_REMOVE_X, wxT("-->"),
                                 wxDefaultPosition, wxSize(20,20), 0);

    xButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
    xButtonsBoxSizer->Add(30, 20, 0, wxALIGN_CENTRE|wxALL, 5);
    xButtonsBoxSizer->Add(addXButton, 0, wxALIGN_CENTRE|wxALL, 5);
    xButtonsBoxSizer->Add(removeXButton, 0, wxALIGN_CENTRE|wxALL, 5);
      
    //------------------------------------------------------
    // parameters box (4th column)
    //------------------------------------------------------
    paramBoxSizer = new wxBoxSizer(wxVERTICAL);
    
    titleAvailbleText = new wxStaticText(this, XY_PLOT_TEXT, wxT("Variables"),
                                         wxDefaultPosition, wxSize(80,-1), 0);
    
    theGuiManager->UpdateParameter();
    
    paramListBox =
        theGuiManager->GetConfigParameterListBox(this, wxSize(150,200));
    
    paramBoxSizer->Add(titleAvailbleText, 0, wxALIGN_CENTRE|wxALL, 5);
    paramBoxSizer->Add(paramListBox, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // add, remove, clear Y buttons (5th column)
    //------------------------------------------------------
    addYButton = new wxButton(this, XY_PLOT_ADD_Y, wxT("-->"),
                              wxDefaultPosition, wxSize(20,20), 0);

    removeYButton = new wxButton(this, XY_PLOT_REMOVE_Y, wxT("<--"),
                                 wxDefaultPosition, wxSize(20,20), 0);
    
    clearYButton = new wxButton(this, XY_PLOT_CLEAR_Y, wxT("<="),
                                wxDefaultPosition, wxSize(20,20), 0);
    
    yButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
    yButtonsBoxSizer->Add(addYButton, 0, wxALIGN_CENTRE|wxALL, 5);
    yButtonsBoxSizer->Add(removeYButton, 0, wxALIGN_CENTRE|wxALL, 5);
    yButtonsBoxSizer->Add(clearYButton, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // Y box label (6th column)
    //------------------------------------------------------
    titleYText = new wxStaticText(this, XY_PLOT_TEXT, wxT("Selected Y"),
                                  wxDefaultPosition, wxSize(80,-1), 0);

    ySelectedListBox = new wxListBox(this, XY_PLOT_LISTBOX, wxDefaultPosition,
                                     wxSize(150,200), 0, emptyList, wxLB_SINGLE);
    
    //loj: for B2, 1 Y axis parameter
    wxString *yParam = new wxString[1];
    yParam[0] = theSubscriber->GetStringParameter("YParamName").c_str();
    if (yParam[0] != "STRING_PARAMETER_UNDEFINED")
        ySelectedListBox->Set(1, yParam);
    
    ySelelectedBoxSizer = new wxBoxSizer(wxVERTICAL);
    ySelelectedBoxSizer->Add(titleYText, 0, wxALIGN_CENTRE|wxALL, 5);
    ySelelectedBoxSizer->Add(ySelectedListBox, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // put in the order
    //------------------------------------------------------
    paramGridSizer->Add(optionBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    paramGridSizer->Add(xSelelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    paramGridSizer->Add(xButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    paramGridSizer->Add(paramBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    paramGridSizer->Add(yButtonsBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    paramGridSizer->Add(ySelelectedBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);
    
    pageBoxSizer->Add(paramGridSizer, 0, wxALIGN_CENTRE|wxALL, 5);
        
    //------------------------------------------------------
    // add to parent sizer
    //------------------------------------------------------
    theMiddleSizer->Add(pageBoxSizer, 0, wxALIGN_CENTRE|wxALL, 5);

    delete xParam;
    delete yParam;
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::LoadData()
{
    // load data from the core engine
    plotCheckBox->SetValue(theSubscriber->IsActive());

}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::SaveData()
{
    // save data to core engine
    theSubscriber->Activate(plotCheckBox->IsChecked());

    // set X parameter
    // set Y parameters
    
    //loj: for B2, do not plot, it doesn't work!!!
    //theSubscriber->Activate(false);
}

