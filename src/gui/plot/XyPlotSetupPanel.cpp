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
//#include "MessageInterface.hpp"

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(XyPlotSetupPanel, wxPanel)
   EVT_BUTTON(XY_PLOT_ADD_X,     XyPlotSetupPanel::OnAddX)
   EVT_BUTTON(XY_PLOT_ADD_Y,     XyPlotSetupPanel::OnAddY)
   EVT_BUTTON(XY_PLOT_REMOVE_X,  XyPlotSetupPanel::OnRemoveX)
   EVT_BUTTON(XY_PLOT_REMOVE_Y,  XyPlotSetupPanel::OnRemoveY)
   EVT_BUTTON(XY_PLOT_CLEAR_Y,   XyPlotSetupPanel::OnClearY)
   EVT_BUTTON(XY_PLOT_ENABLE,    XyPlotSetupPanel::OnEnablePlot)
   EVT_BUTTON(XY_PLOT_SELECT_SC, XyPlotSetupPanel::OnSelectSc)
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
    theSubscriber =
        theGuiInterpreter->GetSubscriber(std::string(subscriberName.c_str()));
    
    Create(this);
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
// void OnEnablePlot(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnEnablePlot(wxCommandEvent& event)
{
    theSubscriber->Activate(plotCheckBox->IsChecked());
}

//------------------------------------------------------------------------------
// void OnSelectSc(wxCommandEvent& event)
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnSelectSc(wxCommandEvent& event)
{
    theSubscriber->Activate(plotCheckBox->IsChecked());
}

//----------------------------------
// methods inherited from GmatPanel
//----------------------------------

//------------------------------------------------------------------------------
// void Create(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @param <parent> input parent.
 * @param <scName> input spacecraft name.
 *
 * @note Creates the notebook for spacecraft information
 */
//------------------------------------------------------------------------------
void XyPlotSetupPanel::Create(wxWindow *parent)
{
    unsigned int i;
    wxString emptyList[] = {};

    pageBoxSizer = new wxBoxSizer(wxVERTICAL);
    //paramGridSizer = new wxFlexGridSizer(2, 5, 0, 0);
    paramGridSizer = new wxFlexGridSizer(6, 0, 0);

    // empty text for spacing line
    emptyText = new wxStaticText(parent, XY_TEXT, wxT(""),
                                  wxDefaultPosition, wxSize(100, -1), 0);
    
    //------------------------------------------------------
    // plot option, select spacecraft (1st column)
    //------------------------------------------------------
    plotCheckBox = new wxCheckBox(parent, XY_PLOT_ENABLE, wxT("Show Plot"),
                                   wxDefaultPosition, wxSize(100, -1), 0);
    
    // spacecraft label
    titleScText = new wxStaticText(parent, XY_TEXT, wxT("Spacecraft"),
                                    wxDefaultPosition, wxSize(100, -1), 0);
    // list of spacecrafts
    StringArray &listSat = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
    int listSize = listSat.size();
    
    if (listSize > 0)  // check to see if any spacecrafts exist
    {
        wxString choices[listSize];
        
        for (int i=0; i<listSize; i++)
            choices[i] = wxString(listSat[i].c_str());
        
        // combo box for avaliable spacecrafts 
        scComboBox = new wxComboBox(parent, XY_PLOT_SELECT_SC, wxT(""), wxDefaultPosition, 
                                    wxSize(100, -1), listSize, choices, wxCB_DROPDOWN);
    }
    else
    {
        // combo box for avaliable spacecrafts 
        wxString emptyList[] =
        {
            wxT("No Spacecrafts Available"), 
        }; 
        scComboBox = new wxComboBox(parent, XY_PLOT_SELECT_SC, wxT(""), wxDefaultPosition, 
                                    wxSize(100, -1), 1, emptyList, wxCB_DROPDOWN);
    }

    // show first spacecraft
    scComboBox->SetSelection(0);
    
    optionBoxSizer = new wxBoxSizer(wxVERTICAL);
    optionBoxSizer->Add(plotCheckBox, 0, wxALIGN_LEFT|wxALL, 5);
    optionBoxSizer->Add(emptyText, 0, wxALIGN_LEFT|wxALL, 5);
    optionBoxSizer->Add(titleScText, 0, wxALIGN_LEFT|wxALL, 5);
    optionBoxSizer->Add(scComboBox, 0, wxALIGN_LEFT|wxALL, 5);
        
    //------------------------------------------------------
    // parameters box (4th column)
    //------------------------------------------------------
    paramBoxSizer = new wxBoxSizer(wxVERTICAL);
    
    StringArray items =
        theGuiInterpreter->GetListOfFactoryItems(Gmat::PARAMETER);
    unsigned int count = items.size();

    wxString *paramList = new wxString[count];
    
    for (i=0; i<count; i++)
    {
        paramList[i] = items[i].c_str();
        //MessageInterface::ShowMessage("XyPlotSetupPanel::Create() " + items[i] + "\n");
    }
    
    titleAvailbleText = new wxStaticText(parent, XY_TEXT, wxT("Parameters"),
                                          wxDefaultPosition, wxSize(80,-1), 0);
    paramBoxSizer->Add(titleAvailbleText, 0, wxALIGN_CENTRE|wxALL, 5);
    paramListBox = new wxListBox(parent, XY_LISTBOX, wxDefaultPosition,
                                  wxSize(140,125), count, paramList, wxLB_SINGLE);
    paramBoxSizer->Add(paramListBox, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // add, remove X buttons (3rd column)
    //------------------------------------------------------
    addXButton = new wxButton(parent, XY_PLOT_ADD_X, wxT("<--"),
                               //wxDefaultPosition, wxDefaultSize, 0);
                               wxDefaultPosition, wxSize(20,20), 0);

    removeXButton = new wxButton(parent, XY_PLOT_REMOVE_X, wxT("-->"),
                                  wxDefaultPosition, wxSize(20,20), 0);

    xButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
    xButtonsBoxSizer->Add(30, 20, 0, wxALIGN_CENTRE|wxALL, 5);
    xButtonsBoxSizer->Add(addXButton, 0, wxALIGN_CENTRE|wxALL, 5);
    xButtonsBoxSizer->Add(removeXButton, 0, wxALIGN_CENTRE|wxALL, 5);
    
    //------------------------------------------------------
    // add, remove, clear Y buttons (5th column)
    //------------------------------------------------------
    addYButton = new wxButton(parent, XY_PLOT_ADD_Y, wxT("-->"),
                              //wxDefaultPosition, wxDefaultSize, 0);
                              wxDefaultPosition, wxSize(20,20), 0);

    removeYButton = new wxButton(parent, XY_PLOT_REMOVE_Y, wxT("<--"),
                                 wxDefaultPosition, wxSize(20,20), 0);
    
    clearYButton = new wxButton(parent, XY_PLOT_CLEAR_Y, wxT("<="),
                                wxDefaultPosition, wxSize(20,20), 0);
    
    yButtonsBoxSizer = new wxBoxSizer(wxVERTICAL);
    yButtonsBoxSizer->Add(30, 20, 0, wxALIGN_CENTRE|wxALL, 5);
    yButtonsBoxSizer->Add(addYButton, 0, wxALIGN_CENTRE|wxALL, 5);
    yButtonsBoxSizer->Add(removeYButton, 0, wxALIGN_CENTRE|wxALL, 5);
    yButtonsBoxSizer->Add(clearYButton, 0, wxALIGN_CENTRE|wxALL, 5);

    //------------------------------------------------------
    // X box label (2nd column)
    //------------------------------------------------------
    titleXText = new wxStaticText(parent, XY_TEXT, wxT("Selected X"),
                                  wxDefaultPosition, wxSize(80,-1), 0);
    
    xSelectedListBox = new wxListBox(parent, XY_LISTBOX, wxDefaultPosition,
                                     wxSize(140,125), 0, emptyList, wxLB_SINGLE);
        
    xSelelectedBoxSizer = new wxBoxSizer(wxVERTICAL);
    xSelelectedBoxSizer->Add(titleXText, 0, wxALIGN_CENTRE|wxALL, 5);
    xSelelectedBoxSizer->Add(xSelectedListBox, 0, wxALIGN_CENTRE|wxALL, 5);
        
    //------------------------------------------------------
    // Y box label (6th column)
    //------------------------------------------------------
    titleYText = new wxStaticText(parent, XY_TEXT, wxT("Selected Y"),
                                  wxDefaultPosition, wxSize(80,-1), 0);

    ySelectedListBox = new wxListBox(parent, XY_LISTBOX, wxDefaultPosition,
                                     wxSize(140,125), 0, emptyList, wxLB_SINGLE);
    
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
    Show();
    
//      parent->SetAutoLayout(TRUE);
//      parent->SetSizer(pageBoxSizer);
//      pageBoxSizer->Fit(parent);
//      pageBoxSizer->SetSizeHints(parent);

    LoadData();

    delete paramList;
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::LoadData()
{
    // load data from core engine
    plotCheckBox->SetValue(theSubscriber->IsActive());

}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::SaveData()
{
    // save data
}

//------------------------------------------------------------------------------
// virtual void OnHelp()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnHelp()
{
    // open the window
    GmatPanel::OnHelp();

    // fill help text
}

//------------------------------------------------------------------------------
// virtual void OnScript()
//------------------------------------------------------------------------------
void XyPlotSetupPanel::OnScript()
{
    // open the window
    GmatPanel::OnScript();

    // fill scripts
}
