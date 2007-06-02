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
 * Implements the TsPlotOptionsDialog window.
 */
//------------------------------------------------------------------------------


#include "TsPlotOptionsDialog.hpp"

TsPlotOptionsDialog::TsPlotOptionsDialog(wxWindow* parent, wxWindowID id, 
                                     const wxString& title, const wxPoint& pos, 
                                     const wxSize& size, long style, 
                                     const wxString& name) :
   wxDialog(parent, id, title, pos, size, style, name)
{
   wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
   
   // Lay out the three columns that appear in the top portion of the dialog
   wxBoxSizer *sideSizer = new wxBoxSizer(wxHORIZONTAL);

   wxStaticBoxSizer *leftSizer = new wxStaticBoxSizer(wxVERTICAL, this, "");
   wxStaticBoxSizer *xAxisSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      "X-Axis Properties");
   wxStaticBoxSizer *yAxisSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      "Y-Axis Properties");
   
   sideSizer->Add(leftSizer, 0, wxALL, 2);
   sideSizer->Add(xAxisSizer, 0, wxALL, 2);
   sideSizer->Add(yAxisSizer, 0, wxALL, 2);

   // Set up the two panels that appear on the left of the dialog
   wxStaticBoxSizer *labelSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      "Plot Labels");
   wxStaticBoxSizer *lineSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      "Line Properties");
   
   leftSizer->Add(labelSizer, 0, wxALL, 2);
   leftSizer->Add(lineSizer, 0, wxALL, 2);

   // Set up the plot labels   
   wxBoxSizer *titleBox = new wxBoxSizer(wxHORIZONTAL);
   titleBox->Add(new wxStaticText(this, -1, "Plot Title:", 
      wxDefaultPosition, wxSize(50,16), wxALIGN_RIGHT), 0, 
      wxALL | wxALIGN_CENTER_VERTICAL, 5);
   titleBox->AddStretchSpacer();
   plotTitle = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(150,22));
   titleBox->Add(plotTitle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *xlabelBox = new wxBoxSizer(wxHORIZONTAL);
   xlabelBox->Add(new wxStaticText(this, -1, "X Label:", 
      wxDefaultPosition, wxSize(50,16), wxALIGN_RIGHT), 0, 
      wxALL | wxALIGN_CENTER_VERTICAL, 5);
   xlabelBox->AddStretchSpacer();
   xAxisLabel = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(150,22));
   xlabelBox->Add(xAxisLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *ylabelBox = new wxBoxSizer(wxHORIZONTAL);
   ylabelBox->Add(new wxStaticText(this, -1, "Y Label:", 
      wxDefaultPosition, wxSize(50,16), wxALIGN_RIGHT), 0, 
      wxALL | wxALIGN_CENTER_VERTICAL, 5);
   ylabelBox->AddStretchSpacer();
   yAxisLabel = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(150,22));
   ylabelBox->Add(yAxisLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   labelSizer->Add(titleBox, 0, wxALL, 5);
   labelSizer->Add(xlabelBox, 0, wxALL, 5);
   labelSizer->Add(ylabelBox, 0, wxALL, 5);

   // Set up the line style options
   wxBoxSizer *lineWidthBox = new wxBoxSizer(wxHORIZONTAL);
   lineWidthBox->Add(new wxStaticText(this, -1, "Line Width:"), 0, 
                                  wxALL | wxALIGN_CENTER_VERTICAL, 5);
   lineWidth = new wxTextCtrl(this, -1);
   lineWidth->Enable(false);
   lineWidthBox->Add(lineWidth, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *lineStyleBox = new wxBoxSizer(wxHORIZONTAL);
   lineStyleBox->Add(new wxStaticText(this, -1, "Line Style:"), 0, 
                                  wxALL | wxALIGN_CENTER_VERTICAL, 5);
   lineStyle = new wxTextCtrl(this, -1);
   lineStyle->Enable(false);
   lineStyleBox->Add(lineStyle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
   
   lineSizer->Add(lineWidthBox, 0, wxALL, 2); 
   lineSizer->Add(lineStyleBox, 0, wxALL, 2); 
   
   // Set up the X-axis options
   wxBoxSizer *xminBox = new wxBoxSizer(wxHORIZONTAL);
   userXMinimum = new wxCheckBox(this, -1, "X Minimum:  ");
   userXMinimum->Enable(false);
   xminBox->Add(userXMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xMinimum = new wxTextCtrl(this, -1, "0.0");
   xMinimum->Enable(false);
   xminBox->Add(xMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *xmaxBox = new wxBoxSizer(wxHORIZONTAL);
   userXMaximum = new wxCheckBox(this, -1, "X Maximum:  ");
   userXMaximum->Enable(false);
   xmaxBox->Add(userXMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xMaximum = new wxTextCtrl(this, -1, "10.0");
   xMaximum->Enable(false);
   xmaxBox->Add(xMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   xLogScale = new wxCheckBox(this, -1, "Logarithmic X Axis");
   xLogScale->Enable(false);

   wxBoxSizer *xTickBox = new wxBoxSizer(wxHORIZONTAL);
   xTickBox->Add(new wxStaticText(this, -1, "Number of X Ticks:"), 0, 
                                   wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xTickCount = new wxTextCtrl(this, -1);
   xTickCount->Enable(false);
   xTickBox->Add(xTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *xMinorTickBox = new wxBoxSizer(wxHORIZONTAL);
   xMinorTickBox->Add(new wxStaticText(this, -1, "# of Minor X Ticks:"), 0, 
                                   wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xMinorTickCount = new wxTextCtrl(this, -1);
   xMinorTickCount->Enable(false);
   xMinorTickBox->Add(xMinorTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   xMinorTickLines = new wxCheckBox(this, -1, "Draw Minor X Lines");
   xMinorTickLines->Enable(false);

   xAxisSizer->Add(xminBox, 0, wxALL, 5);
   xAxisSizer->Add(xmaxBox, 0, wxALL, 5);
   xAxisSizer->Add(xLogScale, 0, wxALL, 5);
   xAxisSizer->Add(xTickBox, 0, wxALL, 5);
   xAxisSizer->Add(xMinorTickBox, 0, wxALL, 5);
   xAxisSizer->Add(xMinorTickLines, 0, wxALL, 5);

   // Set up the Y-axis options
   wxBoxSizer *yminBox = new wxBoxSizer(wxHORIZONTAL);
   userYMinimum = new wxCheckBox(this, -1, "Y Minimum:  ");
   userYMinimum->Enable(false);
   yminBox->Add(userYMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yMinimum = new wxTextCtrl(this, -1, "0.0");
   yMinimum->Enable(false);
   yminBox->Add(yMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *ymaxBox = new wxBoxSizer(wxHORIZONTAL);
   userYMaximum = new wxCheckBox(this, -1, "Y Maximum:  ");
   userYMaximum->Enable(false);
   ymaxBox->Add(userYMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yMaximum = new wxTextCtrl(this, -1, "10.0");
   yMaximum->Enable(false);
   ymaxBox->Add(yMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   yLogScale = new wxCheckBox(this, -1, "Logarithmic Y Axis");
   yLogScale->Enable(false);

   wxBoxSizer *yTickBox = new wxBoxSizer(wxHORIZONTAL);
   yTickBox->Add(new wxStaticText(this, -1, "Number of Y Ticks:"), 0, 
                                   wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yTickCount = new wxTextCtrl(this, -1);
   yTickCount->Enable(false);
   yTickBox->Add(yTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *yMinorTickBox = new wxBoxSizer(wxHORIZONTAL);
   yMinorTickBox->Add(new wxStaticText(this, -1, "# of Minor Y Ticks:"), 0, 
                                   wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yMinorTickCount = new wxTextCtrl(this, -1);
   yMinorTickCount->Enable(false);
   yMinorTickBox->Add(yMinorTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   yMinorTickLines = new wxCheckBox(this, -1, "Draw Minor Y Lines");
   yMinorTickLines->Enable(false);

   yAxisSizer->Add(yminBox, 0, wxALL, 5);
   yAxisSizer->Add(ymaxBox, 0, wxALL, 5);
   yAxisSizer->Add(yLogScale, 0, wxALL, 5);
   yAxisSizer->Add(yTickBox, 0, wxALL, 5);
   yAxisSizer->Add(yMinorTickBox, 0, wxALL, 5);
   yAxisSizer->Add(yMinorTickLines, 0, wxALL, 5);

   // Pull it all together and put buttons at the bottom   
   topSizer->Add(sideSizer, 0, wxALL, 5);
   topSizer->AddStretchSpacer();
   
   wxStaticBoxSizer *buttons = new wxStaticBoxSizer(wxHORIZONTAL, this);
   buttons->AddStretchSpacer();
   buttons->Add(CreateButtonSizer(wxOK), 0, wxALL, 5);
   buttons->AddStretchSpacer();
   buttons->Add(CreateButtonSizer(wxCANCEL), 0, wxALL, 5);
   buttons->AddStretchSpacer();
   topSizer->Add(buttons, 0, wxALIGN_BOTTOM | wxALIGN_CENTER_HORIZONTAL, 15);
   topSizer->AddSpacer(10);

   SetSizer(topSizer);
}

TsPlotOptionsDialog::~TsPlotOptionsDialog()
{
}


std::string TsPlotOptionsDialog::GetPlotTitle()
{
   return plotTitle->GetValue().c_str();
}

std::string TsPlotOptionsDialog::GetXLabel()
{
   return xAxisLabel->GetValue().c_str();
}

std::string TsPlotOptionsDialog::GetYLabel()
{
   return yAxisLabel->GetValue().c_str();
}

void TsPlotOptionsDialog::SetPlotTitle(const std::string &str)
{
   plotTitle->SetValue(str.c_str());
}

void TsPlotOptionsDialog::SetXLabel(const std::string &str)
{
   xAxisLabel->SetValue(str.c_str());
}

void TsPlotOptionsDialog::SetYLabel(const std::string &str)
{
   yAxisLabel->SetValue(str.c_str());
}

