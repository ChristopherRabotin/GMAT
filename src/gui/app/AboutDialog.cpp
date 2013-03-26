//$Id$
//------------------------------------------------------------------------------
//                              AboutDialog
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
// $Copyright$
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2007/08/22
//
/**
 * Implements AboutDialog class.
 */
//------------------------------------------------------------------------------

#include "AboutDialog.hpp"
#include "GmatBaseException.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include "ViewTextDialog.hpp"
#include "FileUtil.hpp"            // for GmatFileUtil::DoesFileExist()
#include <wx/hyperlink.h>
#include <wx/statline.h>
#include <wx/gdicmn.h>


//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(AboutDialog, wxDialog)
   EVT_BUTTON(ID_BUTTON_OK, AboutDialog::OnOK)
   EVT_HYPERLINK(ID_HYPERLINK, AboutDialog::OnHyperLinkClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// AboutDialog(wxWindow *parent, wxWindowID id, const wxString& title,
//            GmatBase *obj = NULL, const wxPoint& pos = wxDefaultPosition,
//            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
//------------------------------------------------------------------------------
/**
 * Constructs AboutDialog object.
 *
 * @param  parent  parent window
 * @param  id      window id
 * @param  title   window title
 *
 */
//------------------------------------------------------------------------------
AboutDialog::AboutDialog(wxWindow *parent, wxWindowID id, const wxString& title,
                         const wxPoint& pos, const wxSize& size, long style)
   : wxDialog(parent, id, title, pos, size, style, title)
{
   #ifdef DEBUG_ICONFILE
   MessageInterface::ShowMessage("AboutDialog::AboutDialog() entered\n");
   #endif
   wxBitmap bitmap;
   wxBitmapButton *aboutButton;
   
   // if icon file available, use it
   FileManager *fm = FileManager::Instance();
   std::string iconFile = (fm->GetFullPathname("ICON_PATH") + "GMATAboutIcon.png");
   #ifdef DEBUG_ICONFILE
   MessageInterface::ShowMessage("   About iconFile='%s'\n", iconFile.c_str());
   #endif
   if (fm->DoesFileExist(iconFile))
   {
      //bitmap.LoadFile(iconFile.c_str(), wxBITMAP_TYPE_JPEG);
      bitmap.LoadFile(iconFile.c_str(), wxBITMAP_TYPE_PNG);
      wxImage image = bitmap.ConvertToImage();
      #ifdef DEBUG_ICONFILE
      MessageInterface::ShowMessage("   Scaling and creating bitmap button\n");
      #endif
      bitmap = wxBitmap(image.Scale(100, 100), wxIMAGE_QUALITY_HIGH);
      aboutButton = new wxBitmapButton(this, -1, bitmap, wxDefaultPosition,
                                       wxSize(100, 100), wxBORDER_NONE);
   }
   else
   {
      MessageInterface::ShowMessage
         ("About GMAT icon file '%s' does not exist.\n", iconFile.c_str());
      aboutButton = new wxBitmapButton(this, -1, NULL, wxDefaultPosition,
                                       wxSize(100, 100));
   }
   
   #ifdef DEBUG_ICONFILE
   MessageInterface::ShowMessage("   Bitmap button created\n");
   #endif
   
   wxColourDatabase cdb;
   wxColour gmatColor = cdb.Find("NAVY");
      
   wxStaticLine *line1 = new wxStaticLine(this);
   wxStaticLine *line2 = new wxStaticLine(this);
   
   // title, build date   
   wxStaticText *gmatText =
      new wxStaticText(this, -1, "General Mission Analysis Tool");
   wxFont font1 = wxFont();
   
   #ifdef __WXMAC__
   font1.SetPointSize(20);
   #else
   font1.SetPointSize(11);
   #endif
   
   font1.SetWeight(wxFONTWEIGHT_BOLD);
   gmatText->SetOwnFont(font1);
   gmatText->SetOwnForegroundColour(gmatColor);
   
   wxString buildDate;
   buildDate.Printf("Build Date: %s %s\n", __DATE__, __TIME__);
   
   wxStaticText *buildText = new wxStaticText(this, -1, buildDate);
   
   #ifdef __WXMAC__
   font1.SetPointSize(12);
   #else
   font1.SetPointSize(8);
   #endif
   
   font1.SetWeight(wxFONTWEIGHT_LIGHT);
   buildText->SetFont(font1);
   
   // website and contact email
   wxStaticText *webText = new wxStaticText(this, -1, "Website: ");
   wxString gmatUrl = "http://gmat.gsfc.nasa.gov";
   wxHyperlinkCtrl *webLink = new wxHyperlinkCtrl(this, -1, gmatUrl, gmatUrl);
   wxStaticText *contactText = new wxStaticText(this, -1, "Contact: ");
   wxStaticText *emailText = new wxStaticText(this, -1, "gmat@gsfc.nasa.gov");
   
   wxFlexGridSizer *contactSizer = new wxFlexGridSizer(2);
   contactSizer->Add(webText, 0, wxALIGN_RIGHT|wxALL, 2);
   contactSizer->Add(webLink, 0, wxALIGN_LEFT|wxALL, 2);
   contactSizer->Add(contactText, 0, wxALIGN_RIGHT|wxALL, 2);
   contactSizer->Add(emailText, 0, wxALIGN_LEFT|wxALL, 2);
   
   wxBoxSizer *gmatSizer = new wxBoxSizer(wxVERTICAL);
   gmatSizer->Add(gmatText, 0, wxALIGN_CENTRE|wxALL, 4);
   gmatSizer->Add(buildText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 4);
   gmatSizer->Add(3, 3);
   gmatSizer->Add(contactSizer, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 4);
   
   wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
   topSizer->Add(aboutButton, 0, wxALIGN_CENTRE|wxALL, 4);
   topSizer->Add(gmatSizer, 0, wxALIGN_CENTRE|wxALL, 4);
   
   // licence and thrid party
   wxStaticText *licenceText = new wxStaticText(this, -1, " - Licensed under");
   //wxString agreement = "NASA Open Source Agreement";
   wxString agreement = "Apache License, Version 2.0";
   theLicenseLink =
      new wxHyperlinkCtrl(this, ID_HYPERLINK, agreement, "");
   wxBoxSizer *licenceSizer = new wxBoxSizer(wxHORIZONTAL);
   licenceSizer->Add(licenceText, 0, wxALIGN_CENTRE|wxALL, 1);
   licenceSizer->Add(theLicenseLink, 0, wxALIGN_CENTRE|wxALL, 1);
   
   wxString use;
   use = use + " - Uses " + wxVERSION_STRING + "\n";
   use = use + " - Uses TSPlot\n";
   //use = use + " - Uses Perl Compatible Regular Expressions\n";
   use = use + " - Uses JPL SPICE Library\n";
   use = use + " - Uses IAU SOFA Library\n";
   use = use + " - Planetary images courtesy of JPL/Caltech/USGS, Celestia \n";
   use = use + "   Motherlode, Bjorn Jonsson, and NASA World Wind";
   wxStaticText *useText = new wxStaticText(this, -1, use);
   
   wxBoxSizer *useSizer = new wxBoxSizer(wxVERTICAL);
   useSizer->Add(licenceSizer, 0, wxALIGN_LEFT|wxALL, 2);
   useSizer->Add(useText, 0, wxALIGN_CENTRE|wxALL, 4);
   
   // OK button
   theOkButton = new wxButton(this, ID_BUTTON_OK, "OK");
   
   // Add to page sizer and set sizer to this dialog
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(topSizer, 0, wxALIGN_CENTRE|wxALL, 4);
   pageSizer->Add(10, 10);
   pageSizer->Add(line1, 0, wxGROW|wxALIGN_CENTRE|wxLEFT|wxRIGHT, 20);
   pageSizer->Add(useSizer, 0, wxALIGN_CENTRE|wxALL, 4);
   pageSizer->Add(line2, 0, wxGROW|wxALIGN_CENTRE|wxLEFT|wxRIGHT, 20);
   pageSizer->Add(10, 10);
   pageSizer->Add(theOkButton, 0, wxALIGN_CENTRE|wxALL, 5);
   
   SetAutoLayout(TRUE);
   SetSizer(pageSizer);
   pageSizer->Fit(this);
   pageSizer->SetSizeHints(this);
   
   // Set icon if icon file is in the start up file
   try
   {
      wxString iconfile = fm->GetFullPathname("MAIN_ICON_FILE").c_str();
      #if defined __WXMSW__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_ICO));
      #elif defined __WXGTK__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_XPM));
      #elif defined __WXMAC__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_PICT_RESOURCE));
      #endif
   }
   catch (GmatBaseException &/*e*/)
   {
      //MessageInterface::ShowMessage(e.GetMessage());
   }
   
   CenterOnScreen(wxBOTH);
   
   #ifdef DEBUG_ICONFILE
   MessageInterface::ShowMessage("AboutDialog::AboutDialog() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnOK(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Closes the dialog
 */
//------------------------------------------------------------------------------
void AboutDialog::OnOK(wxCommandEvent &event)
{
   Close();
}


//------------------------------------------------------------------------------
// void OnHyperLinkClick(wxHyperlinkEvent &even)
//------------------------------------------------------------------------------
/**
 * Opens up hyper link text.
 */
//------------------------------------------------------------------------------
void AboutDialog::OnHyperLinkClick(wxHyperlinkEvent &event)
{
   if (event.GetEventObject() == theLicenseLink)
   {
      ViewTextDialog *dlg =
         //new ViewTextDialog(this, _T("NASA Open Source Agreement"), false,
         //                   wxDefaultPosition, wxSize(450, 300));
         new ViewTextDialog(this, _T("Apache License, Version 2.0"), false,
                            wxDefaultPosition, wxSize(400, 300));
      
      wxString rootPath = FileManager::Instance()->GetRootPath().c_str();
      wxString fileName = rootPath + "License.txt";
      if (!GmatFileUtil::DoesFileExist(fileName.c_str()))
         fileName = "../License.txt";
      
      wxTextCtrl *text = dlg->GetTextCtrl();
      text->LoadFile(fileName);
      dlg->ShowModal();
   }
}

