//$Header$
//------------------------------------------------------------------------------
//                              CoordPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/03/11
/**
 * This class contains the Coordinate System Panel for CoordSystemConfigPanel
 * and CoordSysCreateDialog.
 */
//------------------------------------------------------------------------------
#include "CoordPanel.hpp"

#include "AxisSystem.hpp"
#include "SpacePoint.hpp"
#include "TimeSystemConverter.hpp"

//#define DEBUG_COORD_PANEL 1

//------------------------------------------------------------------------------
// CoordPanel()
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
CoordPanel::CoordPanel(wxWindow *parent, bool enableAll)
   : wxPanel(parent)
{
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   mShowPrimaryBody = false;
   mShowSecondaryBody = false;
   mShowEpoch = false;
   mShowXyz = false;
   mShowUpdate = false;
   
   mEnableAll = enableAll;
   
   Create();
}


//------------------------------------------------------------------------------
// ~CoordPanel()
//------------------------------------------------------------------------------
CoordPanel::~CoordPanel()
{
   // Unregisger GUI components
   theGuiManager->UnregisterComboBox("SpacePoint", originComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", primaryComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", secondaryComboBox);
}


//------------------------------------------------------------------------------
// void EnableOptions()
//------------------------------------------------------------------------------
void CoordPanel::EnableOptions()
{
   #if DEBUG_COORD_PANEL
      MessageInterface::ShowMessage("CoordPanel::EnableOptions() type =%s\n",
      typeComboBox->GetStringSelection().c_str());
   #endif

   wxString typeStr = typeComboBox->GetStringSelection();
   
   if (typeStr == "Equator")
   {
      mShowPrimaryBody = false; 
      mShowSecondaryBody = false;
      mShowEpoch = false;
      mShowXyz = false;
      mShowUpdate = false;
   }
   else if (typeStr == "BodyFixed")
   {
      mShowPrimaryBody = false; 
      mShowSecondaryBody = false;
      mShowEpoch = false;
      mShowXyz = false;
      mShowUpdate = true;
   }
   else if (typeStr == "ObjectReferenced")
   {
      mShowPrimaryBody = true;
      mShowSecondaryBody = true;
      mShowEpoch = false;
      mShowXyz = true;
      mShowUpdate = false;
      SetDefaultObjectRefAxis();
   }
   else if ((typeStr == "TOEEq") || (typeStr == "TOEEc"))
            
   {
      mShowPrimaryBody = false;
      mShowSecondaryBody = false;
      mShowEpoch = true;
      mShowXyz = false;
      mShowUpdate = true;
      SetDefaultEpochRefAxis();
   }
   else if ((typeStr == "TODEq") || (typeStr == "TODEc"))

   {
      mShowPrimaryBody = false;
      mShowSecondaryBody = false;
      mShowEpoch = false;
      mShowXyz = false;
      mShowUpdate = true;
      SetDefaultEpochRefAxis();
   }
   else if((typeStr == "MOEEq") || (typeStr == "MOEEc"))
   {
      mShowPrimaryBody = false;
      mShowSecondaryBody = false;
      mShowEpoch = true;
      mShowXyz = false;
      mShowUpdate = false;
      SetDefaultEpochRefAxis();
   }
   else if(typeStr == "GSE") //loj: 9/13/05 Added
   {
      mShowPrimaryBody = true;
      mShowSecondaryBody = true;
      mShowEpoch = false;
      mShowXyz = false;
      mShowUpdate = false;
   }
   else if(typeStr == "GSM")
   {
      mShowPrimaryBody = true;
      mShowSecondaryBody = true;
      mShowEpoch = false;
      mShowXyz = false;
      mShowUpdate = true;
   }
   else
   {
      mShowPrimaryBody = false;
      mShowSecondaryBody = false;
      mShowEpoch = false;
      mShowXyz = false;
      mShowUpdate = false;
   }
   
   if (mEnableAll)
   {
      primaryStaticText->Enable(mShowPrimaryBody);
      primaryComboBox->Enable(mShowPrimaryBody);
      secondaryStaticText->Enable(mShowSecondaryBody);
      secondaryComboBox->Enable(mShowSecondaryBody);
      formatStaticText->Enable(mShowEpoch);
      formatComboBox->Enable(mShowEpoch);
      epochStaticText->Enable(mShowEpoch);
      epochTextCtrl->Enable(mShowEpoch);
      xStaticText->Enable(mShowXyz);
      xComboBox->Enable(mShowXyz);
      yStaticText->Enable(mShowXyz);
      yComboBox->Enable(mShowXyz);
      zStaticText->Enable(mShowXyz);
      zComboBox->Enable(mShowXyz);
      updateStaticText->Enable(mShowUpdate);
      secStaticText->Enable(mShowUpdate);
      intervalTextCtrl->Enable(mShowUpdate);
      
      // disable some items (loj: 9/13/05 Added)
      if (typeStr == "GSE" || typeStr == "GSM")
      {
         primaryComboBox->SetStringSelection("Earth");
         secondaryComboBox->SetStringSelection("Sun");
         
         primaryStaticText->Enable(false);
         primaryComboBox->Enable(false);
         secondaryStaticText->Enable(false);
         secondaryComboBox->Enable(false);
      }
   }
   else  // disable all of them
   {
      originStaticText->Enable(false);
      typeStaticText->Enable(false);
      primaryStaticText->Enable(false);
      formatStaticText->Enable(false);
      secondaryStaticText->Enable(false);
      epochStaticText->Enable(false);
      originComboBox->Enable(false);
      typeComboBox->Enable(false);
      primaryComboBox->Enable(false);
      formatComboBox->Enable(false);
      secondaryComboBox->Enable(false);
      epochTextCtrl->Enable(false);
      xStaticText->Enable(false);
      xComboBox->Enable(false);
      yStaticText->Enable(false);
      yComboBox->Enable(false);
      zStaticText->Enable(false);
      zComboBox->Enable(false);
      updateStaticText->Enable(false);
      secStaticText->Enable(false);
      intervalTextCtrl->Enable(false);
   }
}


//------------------------------------------------------------------------------
// void SetDefaultAxis()
//------------------------------------------------------------------------------
void CoordPanel::SetDefaultAxis()
{
   // default settings
   typeComboBox->SetValue("MJ2000Eq");
   originComboBox->SetValue("Earth");
   primaryComboBox->SetValue("Earth");
   secondaryComboBox->SetValue("Luna");
   formatComboBox->SetValue("TAIModJulian");
   epochTextCtrl->SetValue("21545");
   xComboBox->SetValue("R");
   yComboBox->SetValue("");;
   zComboBox->SetValue("N");
   intervalTextCtrl->SetValue("60");
}


//------------------------------------------------------------------------------
// void SetDefaultEpochRefAxis()
//------------------------------------------------------------------------------
void CoordPanel::SetDefaultEpochRefAxis()
{
   // default settings
   formatComboBox->SetValue("TAIModJulian");
   epochTextCtrl->SetValue("21545");
}


//------------------------------------------------------------------------------
// void SetDefaultObjectRefAxis()
//------------------------------------------------------------------------------
void CoordPanel::SetDefaultObjectRefAxis()
{
   // default settings
   primaryComboBox->SetValue("Earth");
   secondaryComboBox->SetValue("Luna");
   xComboBox->SetValue("R");
   yComboBox->SetValue("");;
   zComboBox->SetValue("N");
}


//------------------------------------------------------------------------------
// void ShowAxisData(AxisSystem *axis)
//------------------------------------------------------------------------------
void CoordPanel::ShowAxisData(AxisSystem *axis)
{
   try
   {
      int sel = typeComboBox->FindString(axis->GetTypeName().c_str());
      typeComboBox->SetSelection(sel);
      EnableOptions();
      
      if (GetShowPrimaryBody())
      {
         // Primary/Secondary edits to just get the names
         SpacePoint *primaryObj = axis->GetPrimaryObject();
         if (primaryObj != NULL)
            primaryComboBox->SetStringSelection(primaryObj->GetName().c_str());
         else
            primaryComboBox->
               SetStringSelection(axis->GetStringParameter("Primary").c_str());
      }
      
      if (GetShowSecondaryBody())
      {
         SpacePoint *secondObj = axis->GetSecondaryObject();
         if (secondObj != NULL)
            secondaryComboBox->SetStringSelection(secondObj->GetName().c_str());
         else
            secondaryComboBox->
               SetStringSelection(axis->GetStringParameter("Secondary").c_str());
      }
      
      if (GetShowEpoch())
      {
         Real epoch = axis->GetEpoch().Get();
         std::string epochFormat = axis->GetEpochFormat();
         
         formatComboBox->SetStringSelection(epochFormat.c_str());
         wxString wxEpochStr;
         wxEpochStr.Printf("%9.9f", epoch);
         std::string epochString = wxEpochStr.c_str();
         
         // convert if epoch is not in TAIModJulian
         if (epochFormat != "TAIModJulian")
         {
            Real taiEpoch = TimeConverterUtil::ConvertToTaiMjd
               ("A1Mjd", epoch, GmatTimeUtil::JD_JAN_5_1941);
            
            wxString taiEpochStr;
            taiEpochStr.Printf("%9.9f", taiEpoch);
            epochString =
               mTimeConverter.Convert(taiEpochStr.c_str(), "TAIModJulian",
                                      epochFormat);
         }
         
         epochTextCtrl->SetValue(epochString.c_str());
      }
      
      if (GetShowXyz())
      {
         xComboBox->SetStringSelection(axis->GetXAxis().c_str());
         yComboBox->SetStringSelection(axis->GetYAxis().c_str());
         zComboBox->SetStringSelection(axis->GetZAxis().c_str());
      }
      
      if (GetShowUpdateInterval())
      {
         /// @todo:
         Real updateInterval = axis->GetRealParameter("UpdateInterval");

         wxString updateStr;
         std::stringstream buffer;
         buffer.precision(18);
         buffer << updateInterval;
         updateStr.Printf ("%s",buffer.str().c_str());

         intervalTextCtrl->SetValue(updateStr);
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CoordPanel::ShowAxisData() error occurred in getting data!\n%s\n",
          e.GetMessage().c_str());
   }
}


//------------------------------------------------------------------------------
// AxisSystem* CreateAxis()
//------------------------------------------------------------------------------
AxisSystem* CoordPanel::CreateAxis()
{
   wxString priName = primaryComboBox->GetValue().Trim();
   wxString secName = secondaryComboBox->GetValue().Trim();
   wxString axisType = typeComboBox->GetValue().Trim();
   wxString epochFormat = formatComboBox->GetValue().Trim();
   wxString epochStr = epochTextCtrl->GetValue().Trim();
   wxString updateStr = intervalTextCtrl->GetValue().Trim();
   wxString xStr = xComboBox->GetValue();
   wxString yStr = yComboBox->GetValue();
   wxString zStr = zComboBox->GetValue();
   
   AxisSystem *axis = NULL;
   
   if (IsValidAxis(axisType, priName, secName, xStr, yStr, zStr))
   {
      // Create AxisSystem
      axis = (AxisSystem *)theGuiInterpreter->
         CreateAxisSystem(std::string(axisType.c_str()), "");
      
      if (axis != NULL)
      {
         try
         {
            if (priName != "")
            {
               SpacePoint *primary = (SpacePoint *)theGuiInterpreter->
                  GetConfiguredItem(std::string(priName.c_str()));
               axis->SetPrimaryObject(primary);
            }
            
            if (secName != "")
            {
               SpacePoint *secondary = (SpacePoint *)theGuiInterpreter->
                  GetConfiguredItem(std::string(secName.c_str()));
               axis->SetSecondaryObject(secondary);
            }
            
            // set the x, y, and z
            axis->SetXAxis(std::string(xStr.c_str()));
            axis->SetYAxis(std::string(yStr.c_str()));
            axis->SetZAxis(std::string(zStr.c_str()));
            
            axis->SetEpochFormat(std::string(epochFormat.c_str()));
            axis->SetRealParameter("UpdateInterval", atof(updateStr.c_str()));
            
            // convert epoch to a1mjd
            std::string taiEpochStr = mTimeConverter.Convert
               (std::string(epochStr.c_str()), std::string(epochFormat.c_str()),
                "TAIModJulian");
            Real epoch = TimeConverterUtil::ConvertFromTaiMjd
               ("A1Mjd", atof(taiEpochStr.c_str()), GmatTimeUtil::JD_JAN_5_1941);
            axis->SetEpoch(epoch);
            
         }
         catch (BaseException &e)
         {
            MessageInterface::ShowMessage
               ("CoordPanel::CreateAxis() error occurred in setting data!\n%s\n",
                e.GetMessage().c_str());
            
            delete axis;
            axis = NULL;
         }
      }
   }
   
   return axis;
}


//------------------------------------------------------------------------------
// void ChangeEpoch(wxString &oldFormat)
//------------------------------------------------------------------------------
void CoordPanel::ChangeEpoch(wxString &oldFormat)
{
   wxString newFormat = formatComboBox->GetStringSelection().Trim();

   #if DEBUG_COORD_PANEL
   MessageInterface::ShowMessage
      ("CoordPanel::ChangeEpoch() oldFormat=%s, newFormat=%s\n",
       oldFormat.c_str(), newFormat.c_str());
   #endif
   
   if (newFormat != oldFormat)
   {
      std::string newEpoch =
         mTimeConverter.Convert(epochTextCtrl->GetValue().c_str(),
                                oldFormat.c_str(), newFormat.c_str());
      epochTextCtrl->SetValue(newEpoch.c_str());
      oldFormat = newFormat;
   }
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// bool IsValidAxis(const wxString &axisType, const wxString &priName,
//                  const wxString &secName, const wxString &xStr,
//                  const wxString &yStr, const wxString &zStr)
//------------------------------------------------------------------------------
bool CoordPanel::IsValidAxis(const wxString &axisType, const wxString &priName,
                             const wxString &secName, const wxString &xStr,
                             const wxString &yStr, const wxString &zStr)
{
   if (axisType == "")
   {
      MessageInterface::PopupMessage(Gmat::INFO_, "Please select Axis.");
      return false;
   }
   
   if (axisType == "ObjectReferenced")
   {
      if (priName == "" && secName == "")
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "ObjectReferenced must have a primary and secondary body.");
         return false;
      }
      else if (priName == secName)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "The primary and the secondary body must be different.");
         return false;
      }
      
      return IsValidXYZ(xStr, yStr, zStr);
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool IsValidXYZ(const wxString &xStr, const wxString &yStr,
//                 const wxString &zStr)
//------------------------------------------------------------------------------
bool CoordPanel::IsValidXYZ(const wxString &xStr, const wxString &yStr,
                            const wxString &zStr)
{
   // Check to see if x,y,z are valid axes
   if (xStr.IsSameAs("") && (yStr.IsSameAs("") || zStr.IsSameAs("")))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_,  "Please select 2 coordinates from X, Y, and Z.");
      return false;
   }
   else if (xStr.Contains("R") && (yStr.Contains("R") || zStr.Contains("R")))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axis must be orthognal.");
      return false;
   }
   else if (xStr.Contains("V") && (yStr.Contains("V") || zStr.Contains("V")))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axis must be orthognal.");
      return false;
   }
   else if (xStr.Contains("N") && (yStr.Contains("N") || zStr.Contains("N")))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axis must be orthognal.");
      return false;
   }
   
   if (yStr.Contains("R") && zStr.Contains("R"))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axis must be orthognal.");
      return false;
   }
   else if (yStr.Contains("V") && zStr.Contains("V"))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axis must be orthognal.");
      return false;
   }
   else if (yStr.Contains("N") && zStr.Contains("N"))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axis must be orthognal.");
      return false;
   }
   else if (yStr.IsSameAs("") && zStr.IsSameAs(""))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axis must be orthognal.");
      return false;
   }
   
   // Check to make sure at least one is blank
   if (xStr.IsSameAs("") || yStr.IsSameAs("") || zStr.IsSameAs(""))
      return true;
   else
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_,  "One coordinate must be a blank string.");
      return false;
   }
}


//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void CoordPanel::Create()
{
   Setup(this);
   LoadData();
}


//------------------------------------------------------------------------------
// void Setup( wxWindow *parent)
//------------------------------------------------------------------------------
void CoordPanel::Setup( wxWindow *parent)
{
    // wxStaticText
   originStaticText = new wxStaticText( parent, ID_TEXT, wxT("Origin"),
      wxDefaultPosition, wxDefaultSize, 0 );
   typeStaticText = new wxStaticText( parent, ID_TEXT, wxT("Type"),
      wxDefaultPosition, wxDefaultSize, 0 );
   primaryStaticText = new wxStaticText( parent, ID_TEXT, wxT("Primary"),
      wxDefaultPosition, wxDefaultSize, 0 );
   formatStaticText = new wxStaticText( parent, ID_TEXT, wxT("Epoch Format"),
      wxDefaultPosition, wxDefaultSize, 0 );
   secondaryStaticText = new wxStaticText( parent, ID_TEXT, wxT("Secondary"),
      wxDefaultPosition, wxDefaultSize, 0 );
   epochStaticText = new wxStaticText( parent, ID_TEXT, wxT("Epoch"),
      wxDefaultPosition, wxDefaultSize, 0 );
   updateStaticText = new wxStaticText( parent, ID_TEXT, wxT("Update Interval"),
      wxDefaultPosition, wxDefaultSize, 0 );
   secStaticText = new wxStaticText( parent, ID_TEXT, wxT("seconds"),
      wxDefaultPosition, wxDefaultSize, 0 );

   xStaticText = new wxStaticText( parent, ID_TEXT, wxT("X: "),
      wxDefaultPosition, wxDefaultSize, 0 );
   yStaticText = new wxStaticText( parent, ID_TEXT, wxT("Y: "),
      wxDefaultPosition, wxDefaultSize, 0 );
   zStaticText = new wxStaticText( parent, ID_TEXT, wxT("Z: "),
      wxDefaultPosition, wxDefaultSize, 0 );
	  
   #if __WXMAC__
   wxStaticText *title1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Axes"),
                        wxDefaultPosition, wxSize(120,20),
                        wxBOLD);
   title1StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
									true, _T(""), wxFONTENCODING_SYSTEM));
   #endif

   // wxTextCtrl
   wxString emptyList[] = {};

   // wxComboBox
   originComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);
   typeComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition,
      wxSize(120,-1), 0, emptyList, wxCB_DROPDOWN );
   primaryComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);
   formatComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition,
      wxSize(120,-1), 0, emptyList, wxCB_DROPDOWN );
   secondaryComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);
   xComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition,
      wxSize(50,-1), 0, emptyList, wxCB_DROPDOWN );
   yComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition,
      wxSize(50,-1), 0, emptyList, wxCB_DROPDOWN );
   zComboBox = new wxComboBox( parent, ID_COMBO, wxT(""), wxDefaultPosition,
      wxSize(50,-1), 0, emptyList, wxCB_DROPDOWN );

   //wxTextCtrl
   epochTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
   intervalTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(45,-1), 0 );

   // wx*Sizers
   wxBoxSizer *theMainSizer = new wxBoxSizer( wxVERTICAL );
   #if __WXMAC__
   wxBoxSizer *boxsizer4 = new wxBoxSizer( wxVERTICAL );
   #else
   wxStaticBox *staticbox1 = new wxStaticBox( parent, -1, wxT("Axes") );
   wxStaticBoxSizer *staticboxsizer1 = new wxStaticBoxSizer( staticbox1,
      wxVERTICAL );
   #endif
   wxFlexGridSizer *flexgridsizer1 = new wxFlexGridSizer( 3, 4, 0, 0 );
   wxBoxSizer *boxsizer1 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxsizer2 = new wxBoxSizer( wxHORIZONTAL );
   wxBoxSizer *boxsizer3 = new wxBoxSizer( wxHORIZONTAL );

   boxsizer1->Add( originStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer1->Add( originComboBox, 0, wxALIGN_CENTER|wxALL, 5 );

   // row 1
   flexgridsizer1->Add( typeStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( typeComboBox, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, 5 );

   // row 2
   flexgridsizer1->Add( primaryStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( primaryComboBox, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( secondaryStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( secondaryComboBox, 0, wxALIGN_LEFT|wxALL, 5 );

   // row 3
   flexgridsizer1->Add( formatStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( formatComboBox, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( epochStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( epochTextCtrl, 0, wxALIGN_LEFT|wxALL, 5 );

   boxsizer2->Add(xStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(xComboBox, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(yStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(yComboBox, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(zStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(zComboBox, 0, wxALIGN_CENTER|wxALL, 5 );

   boxsizer3->Add(updateStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer3->Add(intervalTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer3->Add(secStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

   #if __WXMAC__
   boxsizer4->Add( flexgridsizer1, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer4->Add( boxsizer2, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer4->Add( boxsizer3, 0, wxALIGN_CENTER|wxALL, 5 );
   
   theMainSizer->Add(boxsizer1, 0, wxALIGN_CENTRE|wxALL, 5);
   theMainSizer->Add(title1StaticText, 0, wxALIGN_LEFT|wxALL, 5);
   theMainSizer->Add(boxsizer4, 0, wxALIGN_CENTRE|wxALL, 5);
   #else
   staticboxsizer1->Add( flexgridsizer1, 0, wxALIGN_CENTER|wxALL, 5 );
   staticboxsizer1->Add( boxsizer2, 0, wxALIGN_CENTER|wxALL, 5 );
   staticboxsizer1->Add( boxsizer3, 0, wxALIGN_CENTER|wxALL, 5 );

   theMainSizer->Add(boxsizer1, 0, wxALIGN_CENTRE|wxALL, 5);
   theMainSizer->Add(staticboxsizer1, 0, wxALIGN_CENTRE|wxALL, 5);
   #endif
   
   if (!mEnableAll)
   {
      wxStaticText *msg =
         new wxStaticText(parent, ID_TEXT,
                          wxT("This is default Coordinate "
                              "System and cannot be modified."),
                          wxDefaultPosition, wxDefaultSize, 0);
      msg->SetForegroundColour(*wxRED);
      theMainSizer->Add(msg, 0, wxALIGN_CENTRE|wxALL, 5);
   }
   
   this->SetAutoLayout( true );
   this->SetSizer( theMainSizer );
   theMainSizer->Fit( this );
   theMainSizer->SetSizeHints( this );
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CoordPanel::LoadData()
{
   try
   {
      // Load axes types
      StringArray itemNames =
         theGuiInterpreter->GetListOfFactoryItems(Gmat::AXIS_SYSTEM);
      for (unsigned int i = 0; i<itemNames.size(); i++)
         typeComboBox->Append(wxString(itemNames[i].c_str()));
      
      // insert a blank option for secondary
      secondaryComboBox->Append("");
      
      // Load epoch types - hard coded for now
      wxString epochStrs[] =
      {
         wxT("TAIModJulian"),
         wxT("UTCModJulian"),
         wxT("TAIGregorian"),
         wxT("UTCGregorian"),
      };
      
      for (unsigned int i = 0; i<4; i++)
         formatComboBox->Append(wxString(epochStrs[i].c_str()));
      
      wxString xyzStrs[] =
      {
         wxT(""),
         wxT("R"),
         wxT("-R"),
         wxT("V"),
         wxT("-V"),
         wxT("N"),
         wxT("-N"),
      };
      
      for (unsigned int i=0; i<7; i++)
      {
         xComboBox->Append(wxString(xyzStrs[i].c_str()));
         yComboBox->Append(wxString(xyzStrs[i].c_str()));
         zComboBox->Append(wxString(xyzStrs[i].c_str()));
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CoordPanel:LoadData() error occurred!\n%s\n",
            e.GetMessage().c_str());
   }
}

//loj: 8/31/05 Added
//------------------------------------------------------------------------------
// bool SaveData(const std::string &coordName, AxisSystem *axis,
//               const wxString &epochFormat)
//------------------------------------------------------------------------------
bool CoordPanel::SaveData(const std::string &coordName, AxisSystem *axis,
                          wxString &epochFormat)
{
   #if DEBUG_COORD_PANEL
   MessageInterface::ShowMessage
      ("oordPanel::SaveData() coordName=%s, epochFormat=%s\n",
       coordName.c_str(), epochFormat.c_str());
   #endif
   
   bool canClose = true;
   
   try
   {
      // create CoordinateSystem if not exist
      CoordinateSystem *coordSys = theGuiInterpreter->GetCoordinateSystem(coordName);
      
      if (coordSys == NULL)
      {
         coordSys = theGuiInterpreter->CreateCoordinateSystem(coordName);
         #if DEBUG_COORD_PANEL
         MessageInterface::ShowMessage
            ("CoordPanel::SaveData() coordName=%s created.\n",
             coordName.c_str());
         #endif
      }
      
      //-------------------------------------------------------
      // set origin and Axis
      //-------------------------------------------------------
      wxString originName = originComboBox->GetValue().Trim();
      coordSys->SetStringParameter("Origin", std::string(originName.c_str()));
      coordSys->SetRefObject(axis, Gmat::AXIS_SYSTEM, "");
      
      SpacePoint *origin = (SpacePoint*)theGuiInterpreter->
         GetConfiguredItem(originName.c_str());
      
      coordSys->SetOrigin(origin);
      
      CelestialBody *j2000body = (CelestialBody*)theGuiInterpreter->
         GetConfiguredItem("Earth");
      
      // set Earth as J000Body if NULL
      if (origin->GetJ2000Body() == NULL)
      {
         j2000body->SetJ2000Body(j2000body);
         origin->SetJ2000Body(j2000body);
      }
      
      coordSys->SetJ2000Body(j2000body);
      
      //-------------------------------------------------------
      // set primary and secondary 
      //-------------------------------------------------------
      // Set primary body if exist
      if (primaryComboBox->IsEnabled())
      {
         wxString primaryName = primaryComboBox->GetValue().Trim();
         SpacePoint *primary = (SpacePoint*)theGuiInterpreter->
            GetConfiguredItem(primaryName.c_str());
         
         axis->SetStringParameter("Primary", primaryName.c_str());
         axis->SetPrimaryObject(primary);
      }
      
      // set secondary body if exist
      if (secondaryComboBox->IsEnabled())
      {
         wxString secondaryName = secondaryComboBox->GetValue().Trim();
         SpacePoint *secondary = (SpacePoint*)theGuiInterpreter->
            GetConfiguredItem(secondaryName.c_str());
         
         axis->SetStringParameter("Secondary", secondaryName.c_str());
         axis->SetSecondaryObject(secondary);
      }
      
      //-------------------------------------------------------
      // set new direction
      //-------------------------------------------------------
      // set X value if exist
      if (xComboBox->IsEnabled())
         axis->SetXAxis(xComboBox->GetValue().Trim().c_str());
      
      // set Y value if exist
      if (yComboBox->IsEnabled())
         axis->SetYAxis(yComboBox->GetValue().Trim().c_str());
      
      // set z value if exist
      if (zComboBox->IsEnabled())
         axis->SetZAxis(zComboBox->GetValue().Trim().c_str());
      
      //-------------------------------------------------------
      // set new epoch format and epoch
      //-------------------------------------------------------
      if (epochTextCtrl->IsEnabled())
      {     
         wxString newEpochFormat = formatComboBox->GetValue().Trim();
         std::string epochStr = epochTextCtrl->GetValue().c_str();
         Real epoch;
         
         if (epochTextCtrl->GetValue().ToDouble(&epoch))
         {
            if (epochFormat != newEpochFormat)
            {
               axis->SetEpochFormat(newEpochFormat.c_str());
               epochFormat = newEpochFormat;
               
               //convert epoch to a1mjd if not in this format
               if (newEpochFormat != "TAIModJulian")
               {
                  std::string taiEpochStr = mTimeConverter.Convert
                     (epochStr, newEpochFormat.c_str(), "TAIModJulian");
                  epoch = TimeConverterUtil::ConvertFromTaiMjd
                     ("A1Mjd", atof(taiEpochStr.c_str()), GmatTimeUtil::JD_JAN_5_1941);
               }
            }
            
            axis->SetEpoch(epoch);
         }
         else
         {
            wxLogError("Invalid epoch value entered.");
            canClose = false;
         }
      }
      
      //-------------------------------------------------------
      // set new update interval
      //-------------------------------------------------------
      if (intervalTextCtrl->IsEnabled())
      {
         Real interval;
         if (intervalTextCtrl->GetValue().ToDouble(&interval))
         {
            axis->SetRealParameter("UpdateInterval", interval);
         }
         else
         {
            wxLogError("Invalid update interval value entered.");
            canClose = false;
         }
      }
      
      coordSys->SetSolarSystem(theGuiInterpreter->GetDefaultSolarSystem());
      coordSys->Initialize();

   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("*** Error *** %s\n", e.GetMessage().c_str());
      canClose = false;
   }
   
   return canClose;
}

