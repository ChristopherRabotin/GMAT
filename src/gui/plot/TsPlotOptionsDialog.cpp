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

TsPlotOptionsDialog::TsPlotOptionsDialog(const std::string xLabel, 
                                     const std::string yLabel, 
                                     wxWindow* parent, wxWindowID id, 
                                     const wxString& title, const wxPoint& pos, 
                                     const wxSize& size, long style, 
                                     const wxString& name) :
   wxDialog       (parent, id, title, pos, size, style, name),
   xName          (xLabel),
   yName          (yLabel)
{
   wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
   
   // Lay out the three columns that appear in the top portion of the dialog
   wxBoxSizer *sideSizer = new wxBoxSizer(wxHORIZONTAL);

   wxStaticBoxSizer *leftSizer = new wxStaticBoxSizer(wxVERTICAL, this, "");
   wxStaticBoxSizer *xAxisSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      (xName + "-Axis Properties").c_str());
   wxStaticBoxSizer *yAxisSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      (yName + "-Axis Properties").c_str());
   
   // Set up the two panels that appear on the left of the dialog
   wxStaticBoxSizer *labelSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      "Plot Labels");
   wxStaticBoxSizer *lineSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      "Line Properties");
   
   leftSizer->Add(labelSizer, 0, wxALL, 2);
   leftSizer->Add(lineSizer, 0, wxALL, 2);

   // Set up the plot labels   
   wxBoxSizer *titleBox = new wxBoxSizer(wxHORIZONTAL);

   std::string xLabelStr = xName + " Label:";
   std::string yLabelStr = yName + " Label:";
   long int w, h, wid;

   wxClientDC dc(this);
   dc.GetTextExtent(wxString(xLabelStr.c_str()), &w, &h);
   wid = w;
   dc.GetTextExtent(wxString(yLabelStr.c_str()), &w, &h);
   if (w > wid)
      wid = w;
   dc.GetTextExtent(wxString("Plot Title:"), &w, &h);
   if (w > wid)
      wid = w;

   wxSize lExtent(w,h);

   titleBox->Add(new wxStaticText(this, -1, "Plot Title:", 
      wxDefaultPosition, lExtent, wxALIGN_RIGHT), 0, 
      wxALL | wxALIGN_CENTER_VERTICAL, 5);
   //titleBox->AddStretchSpacer();
   plotTitle = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(150,22));
   titleBox->Add(plotTitle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *xlabelBox = new wxBoxSizer(wxHORIZONTAL);
   xlabelBox->Add(new wxStaticText(this, -1, xLabelStr.c_str(), 
      wxDefaultPosition, lExtent, wxALIGN_RIGHT), 0, 
      wxALL | wxALIGN_CENTER_VERTICAL, 5);
   //xlabelBox->AddStretchSpacer();
   xAxisLabel = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(150,22));
   xlabelBox->Add(xAxisLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *ylabelBox = new wxBoxSizer(wxHORIZONTAL);
   ylabelBox->Add(new wxStaticText(this, -1, yLabelStr.c_str(), 
      wxDefaultPosition, lExtent, wxALIGN_RIGHT), 0, 
      wxALL | wxALIGN_CENTER_VERTICAL, 5);
   //ylabelBox->AddStretchSpacer();
   yAxisLabel = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(150,22));
   ylabelBox->Add(yAxisLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   labelSizer->Add(titleBox, 0, wxALL, 5);
   labelSizer->Add(xlabelBox, 0, wxALL, 5);
   labelSizer->Add(ylabelBox, 0, wxALL, 5);

   // Set up the line style options
   wxBoxSizer *lineWidthBox = new wxBoxSizer(wxHORIZONTAL);
   lineWidthBox->Add(new wxStaticText(this, -1, "Line Width:"), 0, 
                                  wxALL | wxALIGN_CENTER_VERTICAL, 5);
   lineWidth = new wxSpinCtrl(this, -1, "1");
   lineWidth->SetRange(1,10);
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
   userXMinimum = new wxCheckBox(this, -1, (xName + " Minimum:  ").c_str());
   userXMinimum->Enable(false);
   xminBox->Add(userXMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xMinimum = new wxTextCtrl(this, -1, "0.0");
   xMinimum->Enable(false);
   xminBox->Add(xMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *xmaxBox = new wxBoxSizer(wxHORIZONTAL);
   userXMaximum = new wxCheckBox(this, -1, (xName + " Maximum:  ").c_str());
   userXMaximum->Enable(false);
   xmaxBox->Add(userXMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xMaximum = new wxTextCtrl(this, -1, "10.0");
   xMaximum->Enable(false);
   xmaxBox->Add(xMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   xLogScale = new wxCheckBox(this, -1, 
                              ("Logarithmic " + xName + " Axis").c_str());
   xLogScale->Enable(false);

   wxBoxSizer *xTickBox = new wxBoxSizer(wxHORIZONTAL);
   xTickBox->Add(new wxStaticText(this, -1, 
                                  ("Number of " + xName + " Ticks:").c_str()), 
                                  0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xTickCount = new wxTextCtrl(this, -1);
   xTickCount->Enable(false);
   xTickBox->Add(xTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *xMinorTickBox = new wxBoxSizer(wxHORIZONTAL);
   xMinorTickBox->Add(new wxStaticText(this, -1, 
                                   ("# of Minor " + xName + " Ticks:").c_str()), 
                                   0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xMinorTickCount = new wxTextCtrl(this, -1);
   xMinorTickCount->Enable(false);
   xMinorTickBox->Add(xMinorTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   xMinorTickLines = new wxCheckBox(this, -1, 
                                    ("Draw Minor " + xName + " Lines").c_str());
   xMinorTickLines->Enable(false);

   wxBoxSizer *xPrecisionBox = new wxBoxSizer(wxHORIZONTAL);
   xPrecisionBox->Add(new wxStaticText(this, -1, 
                                   (xName + " Label Precision:").c_str()), 0, 
                                   wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xPrecision = new wxSpinCtrl(this, -1, "6");
   xPrecision->SetRange(2,16);
   xPrecisionBox->Add(xPrecision, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

   xAxisSizer->Add(xminBox, 0, wxALL, 5);
   xAxisSizer->Add(xmaxBox, 0, wxALL, 5);
   xAxisSizer->Add(xLogScale, 0, wxALL, 5);
   xAxisSizer->Add(xTickBox, 0, wxALL, 5);
   xAxisSizer->Add(xMinorTickBox, 0, wxALL, 5);
   xAxisSizer->Add(xMinorTickLines, 0, wxALL, 5);
   xAxisSizer->Add(xPrecisionBox, 0, wxALL, 5);

   // Set up the Y-axis options
   wxBoxSizer *yminBox = new wxBoxSizer(wxHORIZONTAL);
   userYMinimum = new wxCheckBox(this, -1, (yName + " Minimum:  ").c_str());
   userYMinimum->Enable(false);
   yminBox->Add(userYMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yMinimum = new wxTextCtrl(this, -1, "0.0");
   yMinimum->Enable(false);
   yminBox->Add(yMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *ymaxBox = new wxBoxSizer(wxHORIZONTAL);
   userYMaximum = new wxCheckBox(this, -1, (yName + " Maximum:  ").c_str());
   userYMaximum->Enable(false);
   ymaxBox->Add(userYMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yMaximum = new wxTextCtrl(this, -1, "10.0");
   yMaximum->Enable(false);
   ymaxBox->Add(yMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   yLogScale = new wxCheckBox(this, -1, 
                              ("Logarithmic " + yName + " Axis").c_str());
   yLogScale->Enable(false);

   wxBoxSizer *yTickBox = new wxBoxSizer(wxHORIZONTAL);
   yTickBox->Add(new wxStaticText(this, -1, 
                                  ("Number of " + yName + " Ticks:").c_str()), 0, 
                                  wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yTickCount = new wxTextCtrl(this, -1);
   yTickCount->Enable(false);
   yTickBox->Add(yTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *yMinorTickBox = new wxBoxSizer(wxHORIZONTAL);
   yMinorTickBox->Add(new wxStaticText(this, -1, 
                                   ("# of Minor " + yName + " Ticks:").c_str()), 
                                   0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yMinorTickCount = new wxTextCtrl(this, -1);
   yMinorTickCount->Enable(false);
   yMinorTickBox->Add(yMinorTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   yMinorTickLines = new wxCheckBox(this, -1, 
                                    ("Draw Minor " + yName + " Lines").c_str());
   yMinorTickLines->Enable(false);

   wxBoxSizer *yPrecisionBox = new wxBoxSizer(wxHORIZONTAL);
   yPrecisionBox->Add(new wxStaticText(this, -1, 
                                   (yName + " Label Precision:").c_str()), 0, 
                                   wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yPrecision = new wxSpinCtrl(this, -1, "6");
   yPrecision->SetRange(2,16);
   yPrecisionBox->Add(yPrecision, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

   yAxisSizer->Add(yminBox, 0, wxALL, 5);
   yAxisSizer->Add(ymaxBox, 0, wxALL, 5);
   yAxisSizer->Add(yLogScale, 0, wxALL, 5);
   yAxisSizer->Add(yTickBox, 0, wxALL, 5);
   yAxisSizer->Add(yMinorTickBox, 0, wxALL, 5);
   yAxisSizer->Add(yMinorTickLines, 0, wxALL, 5);
   yAxisSizer->Add(yPrecisionBox, 0, wxALL, 5);

   // Pull it all together and put buttons at the bottom   
   sideSizer->Add(leftSizer, 0, wxALL, 2);
   sideSizer->Add(xAxisSizer, 1, wxALL, 2);
   sideSizer->Add(yAxisSizer, 1, wxALL, 2);

   // Configure buttons for bottom of dialog
   wxStaticBoxSizer *buttons = new wxStaticBoxSizer(wxHORIZONTAL, this);
   buttons->AddStretchSpacer();
   buttons->Add(CreateButtonSizer(wxOK), 0, wxALL | wxALIGN_CENTER, 5);
   buttons->Add(80, 0, 0);
   buttons->Add(CreateButtonSizer(wxCANCEL), 0, wxALL | wxALIGN_CENTER, 5);
   buttons->AddStretchSpacer();

   topSizer->Add(sideSizer, 1, wxALL | wxGROW, 5);
   topSizer->Add(buttons, 0, wxALL | wxGROW, 5);

   SetSizer(topSizer);
   topSizer->Fit(this);
}

TsPlotOptionsDialog::~TsPlotOptionsDialog()
{
}

void TsPlotOptionsDialog::UpdateLabels()
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

int TsPlotOptionsDialog::GetWidth()
{
   return lineWidth->GetValue();
}

int TsPlotOptionsDialog::GetStyle()
{
   return wxSOLID;
}

void TsPlotOptionsDialog::SetWidth(int lw)
{
   lineWidth->SetValue(lw);
}

void TsPlotOptionsDialog::SetStyle(int ls)
{
}

void TsPlotOptionsDialog::SetXPrecision(int xPrec)
{
   xPrecision->SetValue(xPrec);
}

void TsPlotOptionsDialog::SetYPrecision(int yPrec)
{
   yPrecision->SetValue(yPrec);
}

int TsPlotOptionsDialog::GetXPrecision()
{
   return xPrecision->GetValue();
}

int TsPlotOptionsDialog::GetYPrecision()
{
   return yPrecision->GetValue();
}

void TsPlotOptionsDialog::SetXName(std::string nomme)
{
   xName = nomme;
}

void TsPlotOptionsDialog::SetYName(std::string nomme)
{
   yName = nomme;
}
