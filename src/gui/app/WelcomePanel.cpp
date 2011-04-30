//$Id$
//------------------------------------------------------------------------------
//                            WelcomePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Thomas Grubb
// Created: 2010/09/09
//------------------------------------------------------------------------------
#include "WelcomePanel.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GuiInterpreter.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"
#include "GmatAppData.hpp"
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/config.h>
#include <wx/hyperlink.h>
#include <wx/statline.h>
#include <wx/gdicmn.h>

//#define DEBUG_FILL_GROUP

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(WelcomePanel, wxFrame)
   EVT_HYPERLINK(ID_BUTTON_RECENT, WelcomePanel::OnOpenRecentScript)
   EVT_CHECKBOX(ID_CHECKBOX, WelcomePanel::OnShowWelcomePanelClicked)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// WelcomePanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs WelcomePanel object.
 */
//------------------------------------------------------------------------------
WelcomePanel::WelcomePanel(wxFrame *frame, const wxString& title,
      int x, int y, int w, int h)
: wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h),
      (wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX)) | wxFRAME_FLOAT_ON_PARENT)
{           
   SetBackgroundColour(wxNullColour);
   Create();
   CenterOnScreen(wxBOTH);
   SetFocus();
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void WelcomePanel::Create()
{
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("WelcomePanel::Create() entered\n");
   #endif
   
   wxStaticLine *line1;
   wxBitmap bitmap;
   wxBitmapButton *aboutButton;
   // Border size
   bsize = 4;
   
   // get the config object
   wxFileConfig *pConfig;
   
   // Load GMAT ICON
   FileManager *fm = FileManager::Instance();
   std::string iconFile = fm->GetFullPathname("ICON_PATH") + "GMATIcon.jpg";
   
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("WelcomePanel::Create() iconFile='%s'\n", iconFile.c_str());
   #endif
   
   if (GmatFileUtil::DoesFileExist(iconFile))
   {
      #ifdef DEBUG_CREATE
      MessageInterface::ShowMessage
         ("   Loading iconFile '%s'\n", iconFile.c_str());
      #endif
      bitmap = LoadBitmap( iconFile.c_str(), 200, 200 );
      aboutButton = new wxBitmapButton(this, -1, bitmap, wxDefaultPosition,
                          wxSize(200,200));
   }
   else
   {
      #ifdef DEBUG_CREATE
      MessageInterface::ShowMessage
         ("   the iconFile '%s' doesnot exist, so creating default\n", iconFile.c_str());
      #endif
      aboutButton = new wxBitmapButton(this, -1, NULL, wxDefaultPosition,
                           wxSize(200,200));
   }
   
   wxColourDatabase cdb;
   wxColour gmatColor = cdb.Find("NAVY");
   // title, build date
   wxStaticText *gmatText =
      new wxStaticText(this, -1, "General Mission Analysis Tool");
   wxFont font1 = wxFont();

   #ifdef __WXMAC__
   font1.SetPointSize(20);
   #else
   font1.SetPointSize(20);
   #endif

   font1.SetWeight(wxFONTWEIGHT_BOLD);
   gmatText->SetOwnFont(font1);
   gmatText->SetOwnForegroundColour(gmatColor);

   // website and contact email
   wxStaticText *webText = new wxStaticText(this, -1, "Website: ");
   wxString gmatUrl = "http://gmat.gsfc.nasa.gov";
   wxHyperlinkCtrl *webLink = new wxHyperlinkCtrl(this, -1, gmatUrl, gmatUrl);
   
   wxFlexGridSizer *contactSizer = new wxFlexGridSizer(2);
   contactSizer->Add(webText, 0, wxALIGN_RIGHT|wxALL, 2);
   contactSizer->Add(webLink, 0, wxALIGN_LEFT|wxALL, 2);
   
   wxBoxSizer *gmatTextSizer = new wxBoxSizer(wxVERTICAL);
   gmatTextSizer->Add(gmatText, 0, wxALIGN_LEFT|wxALL, bsize);
   gmatTextSizer->Add(3, 3);
   gmatTextSizer->Add(contactSizer, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, bsize);
   
   wxBoxSizer *headerSizer = new wxBoxSizer(wxHORIZONTAL);
   headerSizer->Add(aboutButton, 0, wxALIGN_LEFT|wxALL, bsize);
   headerSizer->Add(gmatTextSizer, 0, wxALIGN_LEFT|wxALL, bsize);

   //-----------------------------------------------------------------
   // now Links and Resources
   //-----------------------------------------------------------------
   wxStaticText *resourcesText =
      new wxStaticText(this, -1, "Links and Resources");
   resourcesText->SetOwnFont(font1);
   resourcesText->SetOwnForegroundColour(gmatColor);
   wxBoxSizer *resourcesSizer = new wxBoxSizer(wxVERTICAL);
   resourcesSizer->Add(resourcesText, 0, wxALIGN_LEFT|wxALL, bsize);
   resourcesSizer->AddSpacer(bsize*2);
   // add the recent scripts
   pConfig = (wxFileConfig *) wxConfigBase::Get();
//   MessageInterface::PopupMessage(Gmat::INFO_, "Getting Welcome Links");
   resourcesSizer->Add(FillGroup("/Welcome/Links", "", 3, ID_URL, false, pConfig), 0, wxALIGN_LEFT|wxALL, bsize*2);
//   MessageInterface::PopupMessage(Gmat::INFO_, "Got Welcome Links");

   //-----------------------------------------------------------------
   // now Getting Started
   //-----------------------------------------------------------------
   wxStaticText *getStartedText =
      new wxStaticText(this, -1, "Getting Started");
   getStartedText->SetOwnFont(font1);
   getStartedText->SetOwnForegroundColour(gmatColor);
   wxBoxSizer *gettingStartedSizer = new wxBoxSizer(wxVERTICAL);
   gettingStartedSizer->Add(getStartedText, 0, wxALIGN_LEFT|wxALL, bsize);
   gettingStartedSizer->AddSpacer(bsize*2);
   gettingStartedSizer->Add(FillGroup("/GettingStarted/Tutorials", "/GettingStarted/Tutorials/Icons",
                                      1, ID_URL, false, pConfig), 0, wxALIGN_LEFT|wxALL, bsize*2);
   //gettingStartedSizer->AddSpacer(bsize*2);

   //-----------------------------------------------------------------
   // now recent scripts
   //-----------------------------------------------------------------
   wxStaticText *recentScriptsText =
      new wxStaticText(this, -1, "Recent Scripts");
   recentScriptsText->SetOwnFont(font1);
   recentScriptsText->SetOwnForegroundColour(gmatColor);
   wxBoxSizer *recentSizer = new wxBoxSizer(wxVERTICAL);
   recentSizer->Add(recentScriptsText, 0, wxALIGN_LEFT|wxALL, bsize);
   recentSizer->AddSpacer(bsize*2);
   // add the recent scripts
   pConfig = (wxFileConfig *) GmatAppData::Instance()->GetPersonalizationConfig();
   recentSizer->Add(FillGroup("/RecentFiles", "", 1, ID_BUTTON_RECENT, true, pConfig), 0, wxALIGN_LEFT|wxALL, bsize*2);


   //-----------------------------------------------------------------
   // Now put it all together
   //-----------------------------------------------------------------
   wxBoxSizer *contentSizer = new wxBoxSizer(wxHORIZONTAL);
   contentSizer->Add(recentSizer, 0, wxALIGN_LEFT|wxALL, bsize*2);
   line1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
   contentSizer->Add(line1, 0, wxGROW|wxALIGN_CENTRE, 20);
   contentSizer->Add(gettingStartedSizer, 1, wxGROW|wxALIGN_LEFT|wxALL, bsize*2);

   wxSizer *theMiddleSizer = (wxSizer*)(new wxBoxSizer(wxVERTICAL));
   theMiddleSizer->Add(headerSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize*2);
   line1 = new wxStaticLine(this);
   theMiddleSizer->Add(line1, 0, wxEXPAND|wxALIGN_CENTRE, bsize*2);
   theMiddleSizer->Add(contentSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL);
   line1 = new wxStaticLine(this);
   theMiddleSizer->Add(line1, 0, wxEXPAND|wxALIGN_CENTRE, bsize*2);
   theMiddleSizer->Add(resourcesSizer, 0, wxALIGN_LEFT|wxEXPAND|wxALL, bsize*2);
   line1 = new wxStaticLine(this);
   theMiddleSizer->Add(line1, 0, wxEXPAND|wxALIGN_CENTRE, bsize*2);

   wxCheckBox *cbShow = new wxCheckBox( this, ID_CHECKBOX, wxT("&Show Welcome Page On Startup"));
   wxString showWelcomePanel;
   pConfig->Read("/Main/ShowWelcomeOnStart", &showWelcomePanel, "true");
   cbShow->SetValue( showWelcomePanel.Lower() == "true" );
   theMiddleSizer->Add(cbShow, 0, wxALIGN_LEFT|wxALL, bsize*2);

   wxSizer *thePanelSizer = new wxBoxSizer(wxVERTICAL);
   thePanelSizer->Add(theMiddleSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize*2);
   SetSizer(thePanelSizer);
   thePanelSizer->SetSizeHints(this);
   
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
   
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("WelcomePanel::Create() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// ~WelcomePanel()
//------------------------------------------------------------------------------
WelcomePanel::~WelcomePanel()
{
   GmatAppData::Instance()->GetMainFrame()->CloseWelcomePanel();
}


//------------------------------------------------------------------------------
// void OnExit(wxCommandEvent& WXUNUSED(event) )
//------------------------------------------------------------------------------
void WelcomePanel::OnExit(wxCommandEvent& WXUNUSED(event) )
{
   Close(true);
}


//------------------------------------------------------------------------------
// void OnShowWelcomePanelClicked(wxCommandEvent& WXUNUSED(event) )
//------------------------------------------------------------------------------
void WelcomePanel::OnShowWelcomePanelClicked(wxCommandEvent& event)
{
   wxFileConfig *pConfig = (wxFileConfig *) GmatAppData::Instance()->GetPersonalizationConfig();
   if (event.IsChecked())
      pConfig->Write("/Main/ShowWelcomeOnStart", "true");
   else
      pConfig->Write("/Main/ShowWelcomeOnStart", "false");
}


//------------------------------------------------------------------------------
// wxFlexGridSizer *FillGroup( wxString INIGroup, int maxRows, wxFileConfig *config )
//------------------------------------------------------------------------------
/**
 * Handles filling a sizer with entries from INI group.
 *
 * @param INIGroup name of group (should include any preceding slashes, e.g. "/RecentFiles")
 * @param maxCols maximum number of columns
 * @param config configuration file to read
 */
//------------------------------------------------------------------------------
wxFlexGridSizer *WelcomePanel::FillGroup( wxString INIGroup, wxString INIIconGroup,
                                          int maxCols, wxWindowID id, bool isFileList,
                                          wxFileConfig *config )
{
   #ifdef DEBUG_FILL_GROUP
   MessageInterface::ShowMessage
      ("WelcomePanel::FillGroup() entered, INIGroup='%s', INIIconGroup='%s'\n",
       INIGroup.c_str(), INIIconGroup.c_str());
   #endif
   
   wxFlexGridSizer *aSizer;
   wxFlexGridSizer *aIconNTextSizer;
   wxArrayString linkLabels;
   wxArrayString linkURLs;
   wxArrayString linkIcons;
   size_t i;
   wxBitmapButton *abitmapButton = NULL;

   wxString aKey;
   long dummy;
   int aCol = 1;
   wxHyperlinkCtrl *aButton;
   wxStaticLine *line1;


   // get the links from the config
   config->SetPath(INIGroup);
   // read filenames from config object
   if (config->GetFirstEntry(aKey, dummy))
   {
      if (isFileList)
         linkLabels.Add(GmatFileUtil::ParseFileName(config->Read(aKey).c_str()).c_str());
      else
         linkLabels.Add(aKey);
      linkURLs.Add(config->Read(aKey));
      while (config->GetNextEntry(aKey, dummy))
      {
         if (isFileList)
            linkLabels.Add(GmatFileUtil::ParseFileName(config->Read(aKey).c_str()).c_str());
         else
            linkLabels.Add(aKey);
         linkURLs.Add(config->Read(aKey));
      }
   }
   // get the icons if section exists
   if (INIIconGroup != "")
   {
      config->SetPath(INIIconGroup);
      for (i=0; i<linkLabels.size(); i++)
         linkIcons.Add(config->Read(linkLabels[i]));
   }

   if (maxCols > 1)
   {
      maxCols = maxCols * 2;
      aSizer = new wxFlexGridSizer(maxCols, 10, 40);
   }
   else
      aSizer = new wxFlexGridSizer(maxCols, 10, 40);
   
   #ifdef DEBUG_FILL_GROUP
   MessageInterface::ShowMessage("   There are %d linkLables\n", linkLabels.size());
   #endif
   
   if (linkLabels.size() > 0)
   {
      FileManager *fm = FileManager::Instance();
      for (i=0; i<linkLabels.size(); i++)
      {
         aButton = new wxHyperlinkCtrl
            (this, id, linkLabels[i], linkURLs[i], wxDefaultPosition, wxDefaultSize,
             wxNO_BORDER|wxHL_ALIGN_CENTRE);
         aButton->SetToolTip(linkURLs[i]);
         if (linkIcons.size() > 0)
         {
            aIconNTextSizer = new wxFlexGridSizer(2, 20, 20);
            std::string fullPath = fm->GetFullPathname("ICON_PATH") + linkIcons[i].c_str();
            #ifdef DEBUG_FILL_GROUP
            MessageInterface::ShowMessage("   fullPath='%s'\n", fullPath.c_str());
            #endif
            if (GmatFileUtil::DoesFileExist(fullPath.c_str()))
            {
               wxBitmap bitmap = LoadBitmap( fullPath.c_str(), 50, 50 );
               abitmapButton = new wxBitmapButton(this, -1, bitmap, wxDefaultPosition,
                                                  wxSize(50,50));
            }
            else
            {
               MessageInterface::ShowMessage
                  ("*** WARNING *** Can't load image from file '%s'\n", fullPath.c_str());
               abitmapButton = NULL;
            }
            
            aIconNTextSizer->Add(abitmapButton, 0, wxALIGN_LEFT, bsize);
            aIconNTextSizer->Add(aButton, 0, wxALIGN_LEFT, bsize);
            aSizer->Add(aIconNTextSizer, 0, wxALIGN_LEFT, bsize*2);
         }
         else
            aSizer->Add(aButton, 0, wxALIGN_LEFT, bsize*3);
         aCol = (aCol + 1) % maxCols;
         if ((maxCols > 1) && (aCol < (maxCols-1)))
         {
            line1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
            aSizer->Add(line1, 0, wxALIGN_CENTRE, 20);
         }
         aCol = (aCol + 1) % maxCols;
      }
   }
   
   #ifdef DEBUG_FILL_GROUP
   MessageInterface::ShowMessage
      ("WelcomePanel::FillGroup() returning aSizer<%p>\n", aSizer);
   #endif
   
   return aSizer;
}


//------------------------------------------------------------------------------
// void OnOpenRecentScript(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void WelcomePanel::OnOpenRecentScript(wxHyperlinkEvent& event)
{
   GmatAppData::Instance()->GetMainFrame()->OpenRecentScript(event.GetURL(), event);
}


//------------------------------------------------------------------------------
// wxBitmap *LoadBitmap( wxString filename )
//------------------------------------------------------------------------------
/**
 * Loads a JPG bitmap from a file
 *
 * @param filename name of the image file
 */
//------------------------------------------------------------------------------
wxBitmap WelcomePanel::LoadBitmap( wxString filename, int width, int height )
{
   wxBitmap bitmap;
   // Check for file exist first, otherwise the system hangs for non-existent file
   if (GmatFileUtil::DoesDirectoryExist(filename.c_str(), false))
   {
      wxImage::AddHandler(new wxJPEGHandler);
      if (bitmap.LoadFile(filename, wxBITMAP_TYPE_JPEG))
      {
         wxImage image = bitmap.ConvertToImage();
         bitmap = wxBitmap(image.Scale(width, height));
      }
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** Can't load image from '%s'\n", filename.c_str());
   }
   
   return bitmap;
}

