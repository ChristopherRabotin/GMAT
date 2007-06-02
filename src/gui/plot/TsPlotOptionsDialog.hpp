// $Header$
//------------------------------------------------------------------------------
//                           TsPlotOptionsDialog
//------------------------------------------------------------------------------
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/05/20
//
// This code is Copyright (c) 2005, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Declares the TsPlotOptionsDialog window.
 */
//------------------------------------------------------------------------------


#ifndef PLOTOPTIONSDIALOG_HPP_
#define PLOTOPTIONSDIALOG_HPP_

#include <wx/wx.h>
//#include <wx/statline.h>
//#include <wx/button.h>
//#include <wx/sizer.h>
//#include <wx/textctrl.h>

#include <string>


class TsPlotOptionsDialog : public wxDialog
{
public:
	TsPlotOptionsDialog(wxWindow* parent, wxWindowID id, 
                     const wxString& title = "TS Plot Options", 
                     const wxPoint& pos = wxDefaultPosition, 
                     const wxSize& size = wxSize(720,380), 
                     long style = wxDEFAULT_DIALOG_STYLE, 
                     const wxString& name = "TSPlotOptions");
	virtual ~TsPlotOptionsDialog();
   
   // Accessors
   std::string       GetPlotTitle();
   std::string       GetXLabel();
   std::string       GetYLabel();
   void              SetPlotTitle(const std::string &str);
   void              SetXLabel(const std::string &str);
   void              SetYLabel(const std::string &str);
   
protected:
//   wxBoxSizer           *labels;
   
   // Controls that are accessed to set plot properties
   wxTextCtrl           *plotTitle;
   wxTextCtrl           *xAxisLabel;
   wxTextCtrl           *yAxisLabel;
   wxTextCtrl           *lineWidth;
   wxTextCtrl           *lineStyle;
   
   // X axis options
   wxCheckBox           *userXMinimum;
   wxTextCtrl           *xMinimum;
   wxCheckBox           *userXMaximum;
   wxTextCtrl           *xMaximum;
   wxCheckBox           *xLogScale;
   wxTextCtrl           *xTickCount;
   wxTextCtrl           *xMinorTickCount;
   wxCheckBox           *xMinorTickLines;

   // Y axis options
   wxCheckBox           *userYMinimum;
   wxTextCtrl           *yMinimum;
   wxCheckBox           *userYMaximum;
   wxTextCtrl           *yMaximum;
   wxCheckBox           *yLogScale;
   wxTextCtrl           *yTickCount;
   wxTextCtrl           *yMinorTickCount;
   wxCheckBox           *yMinorTickLines;
};

#endif /*PLOTOPTIONSDIALOG_HPP_*/
