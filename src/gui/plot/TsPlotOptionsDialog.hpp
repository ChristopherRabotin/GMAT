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
#include <wx/spinctrl.h>

#include <string>


class TsPlotOptionsDialog : public wxDialog
{
public:
	TsPlotOptionsDialog(const std::string xLabel, const std::string yLabel,
                     wxWindow* parent, wxWindowID id, 
                     const wxString& title = "TS Plot Options", 
                     const wxPoint& pos = wxDefaultPosition, 
                     const wxSize& size = wxSize(780,380), 
                     long style = wxDEFAULT_DIALOG_STYLE, 
                     const wxString& name = "TSPlotOptions");
	virtual ~TsPlotOptionsDialog();
   
   // Accessors
   std::string       GetPlotTitle();
   std::string       GetXLabel();
   std::string       GetYLabel();
   int               GetWidth();
   int               GetStyle();
   void              SetPlotTitle(const std::string &str);
   void              SetXLabel(const std::string &str);
   void              SetYLabel(const std::string &str);
   void              SetWidth(int lw);
   void              SetStyle(int ls);
   
   void              SetXPrecision(int xPrec);
   void              SetYPrecision(int yPrec);
   int               GetXPrecision();
   int               GetYPrecision();
   
   void              SetXName(std::string nomme);
   void              SetYName(std::string nomme);
   void              UpdateLabels();
   
protected:
//   wxBoxSizer           *labels;
   
   // Controls that are accessed to set plot properties
   wxTextCtrl           *plotTitle;
   wxTextCtrl           *xAxisLabel;
   wxTextCtrl           *yAxisLabel;
   wxSpinCtrl           *lineWidth;
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
   wxSpinCtrl           *xPrecision;

   // Y axis options
   wxCheckBox           *userYMinimum;
   wxTextCtrl           *yMinimum;
   wxCheckBox           *userYMaximum;
   wxTextCtrl           *yMaximum;
   wxCheckBox           *yLogScale;
   wxTextCtrl           *yTickCount;
   wxTextCtrl           *yMinorTickCount;
   wxCheckBox           *yMinorTickLines;
   wxSpinCtrl           *yPrecision;
   
   std::string          xName;
   std::string          yName;
};

#endif /*PLOTOPTIONSDIALOG_HPP_*/
