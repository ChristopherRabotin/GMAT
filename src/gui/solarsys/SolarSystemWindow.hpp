//$Id$
//------------------------------------------------------------------------------
//                              SolarSystemWindow
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2003/09/29
/**
 * This class allows user to specify where solar system information is 
 * coming from
 */
//------------------------------------------------------------------------------
#ifndef SolarSystemWindow_hpp
#define SolarSystemWindow_hpp

#include "gmatwxdefs.hpp"
#include "wx/sizer.h"
#include "wx/splitter.h"
#include "wx/listctrl.h"
#include "wx/treectrl.h"
#include "wx/notebook.h"
#include "wx/button.h"
#include "wx/grid.h"

//loj: commented out
//  #define ID_TEXT      20040
//  #define ID_TEXTCTRL  20041
//  #define ID_FILE_NAME 20042
//  #define ID_RADIOBOX  20043        
//  #define ID_BUTTON_POS          20044
//  #define ID_BUTTON_BODYROTATION 20045
//  #define ID_BUTTON_BROWSE       20046
//  #define ID_BUTTON_FIXEDCOORD   20047
//  #define ID_BUTTON_CREATE       20048
//  #define ID_BUTTON_CANCEL       20049

class SolarSystemWindow: public wxScrolledWindow
{
public:
    SolarSystemWindow(wxWindow *parent);
    //wxSizer *SolarSystemSetup( wxWindow *parent, bool call_fit = TRUE, bool set_sizer = TRUE );
      
private:
    wxBoxSizer *item0; 
    wxBoxSizer *item30;          
    wxGridSizer *item1;          
    wxFlexGridSizer *item8;
      
    wxStaticText *item2;
    wxStaticText *item5;
    wxStaticText *item9;
    wxStaticText *item10;
    wxStaticText *item13;  
    wxStaticText *item15;
    wxStaticText *item16;
    wxStaticText *item18;
    wxStaticText *item20;
    wxStaticText *item22;
    wxStaticText *item25;
    wxStaticText *item27;
    wxStaticText *item29;
      
    wxTextCtrl *item3;
    wxTextCtrl *item6;
    wxTextCtrl *item11;
    wxTextCtrl *item14;
    wxTextCtrl *item17;
    wxTextCtrl *item19;
    wxTextCtrl *item21;
    wxTextCtrl *item23;
    wxTextCtrl *item26;
    wxTextCtrl *item28;                           
                                  
    wxButton *item4;             
    wxButton *item7;
    wxButton *item12;
    wxButton *item24;
    wxButton *item31;
    wxButton *item32;
      
    void SolarSystemSetup(wxWindow *parent);
    void OnPositionButton(wxCommandEvent& event);
    void OnBodyRotationButton(wxCommandEvent& event);
    void OnBrowseButton(wxCommandEvent& event);
    void OnFixedCoordinatesButton(wxCommandEvent& event);
    void OnCreateButton(wxCommandEvent& event);
    void OnCancelButton(wxCommandEvent& event);
      
    //wxDialog *CreateRadiobox();
    DECLARE_EVENT_TABLE();

    // IDs for the controls and the menu commands
    enum
    {
        ID_TEXT = 6000,
        ID_TEXTCTRL,
        ID_FILE_NAME,
        ID_RADIOBOX,     
        ID_BUTTON_POS,
        ID_BUTTON_BODYROTATION ,
        ID_BUTTON_BROWSE,
        ID_BUTTON_FIXEDCOORD,
        ID_BUTTON_CREATE,
        ID_BUTTON_CANCEL
    };
};
#endif
