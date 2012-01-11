//$Id$
//------------------------------------------------------------------------------
//                              CoordPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "TimeSystemConverter.hpp"  // for Convert()
#include "StringUtil.hpp"           // for ToReal()
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include <wx/config.h>
#include <sstream>

//#define DEBUG_COORD_PANEL
//#define DEBUG_COORD_PANEL_CREATE
//#define DEBUG_COORD_PANEL_LOAD
//#define DEBUG_COORD_PANEL_SAVE
//#define DEBUG_COORD_EPOCH
//#define DEBUG_COORD_PANEL_PRIMARY_SECONDARY

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
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   mShowPrimaryBody = false;
   mShowSecondaryBody = false;
   mShowEpoch = false;
   mShowXyz = false;
   mShowUpdate = false;
   
   mEnableAll = enableAll;
   
////   epochValue = "21545";
//   std::stringstream mjdStr("");
//   mjdStr << GmatTimeConstants::MJD_OF_J2000;
//   epochValue = mjdStr.str();
//   epochFormatValue = "A1ModJulian";
   
   Create();
   LoadData();
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
// void EnableOptions(AxisSystem *axis)
//------------------------------------------------------------------------------
void CoordPanel::EnableOptions(AxisSystem *axis)
{
   #ifdef DEBUG_COORD_PANEL
   MessageInterface::ShowMessage
      ("CoordPanel::EnableOptions() axis=(%p)%s\n", axis,
       typeComboBox->GetStringSelection().c_str());
   #endif
   
   // save epoch value locally
   epochValue = epochTextCtrl->GetValue();
   
   wxString typeStr = typeComboBox->GetStringSelection();
   AxisSystem* tmpAxis = NULL;
   
   if (typeStr == "")
      typeStr = "MJ2000Eq";

   if (axis == NULL)
      // create a temp axis to use flags
      tmpAxis = (AxisSystem *)theGuiInterpreter->
         CreateObject(typeStr.c_str(), ""); // Use no name
   else
      tmpAxis = axis;
   
   if (tmpAxis == NULL)
      return;
   
   if (tmpAxis->UsesPrimary() == GmatCoordinate::NOT_USED)
      mShowPrimaryBody = false;
   else
      mShowPrimaryBody = true; 
   
   if (tmpAxis->UsesSecondary() == GmatCoordinate::NOT_USED)
      mShowSecondaryBody = false;
   else
      mShowSecondaryBody = true; 
   
   if (tmpAxis->UsesEpoch() == GmatCoordinate::NOT_USED)
      mShowEpoch = false;
   else
   {
      mShowEpoch = true; 
      
      // get the epoch format and value from tmpAxis
      Real epoch = tmpAxis->GetEpoch().Get();
      epochValue = theGuiManager->ToWxString(epoch);
//      epochFormatValue = wxString(tmpAxis->GetEpochFormat().c_str());

      #ifdef DEBUG_COORD_PANEL
      MessageInterface::ShowMessage
         ("CoordPanel::EnableOptions() about to set epoch value to %12.10f (string = %s)\n",
               epoch, epochValue.c_str());
      #endif
      // set the text ctrl
      epochTextCtrl->SetValue(epochValue);
//      formatComboBox->SetValue(epochFormatValue);
   }
   #ifdef DEBUG_COORD_EPOCH
      MessageInterface::ShowMessage("mShowEpoch = %s\n", (mShowEpoch? "true" : "false"));
   #endif
   
   if ((tmpAxis->UsesXAxis() == GmatCoordinate::NOT_USED) &&
       (tmpAxis->UsesYAxis() == GmatCoordinate::NOT_USED) &&
       (tmpAxis->UsesZAxis() == GmatCoordinate::NOT_USED))
      mShowXyz = false;
   else
      mShowXyz = true; 
   
   if (tmpAxis->UsesNutationUpdateInterval() == GmatCoordinate::NOT_USED)
      mShowUpdate = false;
   else
      mShowUpdate = true; 

   if (typeStr == "ObjectReferenced")
      SetDefaultObjectRefAxis();
   else if ((typeStr == "TOEEq") || (typeStr == "TOEEc"))
      SetDefaultEpochRefAxis();
   else if ((typeStr == "TODEq") || (typeStr == "TODEc"))
      SetDefaultEpochRefAxis();
   else if((typeStr == "MOEEq") || (typeStr == "MOEEc"))
      SetDefaultEpochRefAxis();
   
   if (mEnableAll)
   {
      primaryStaticText->Enable(mShowPrimaryBody);
      primaryComboBox->Enable(mShowPrimaryBody);
      secondaryStaticText->Enable(mShowSecondaryBody);
      secondaryComboBox->Enable(mShowSecondaryBody);
//      formatStaticText->Enable(mShowEpoch);
      // arg: 1/23/05 - for now never enable
//      formatComboBox->Enable(false);
      epochStaticText->Enable(mShowEpoch);
      epochTextCtrl->Enable(mShowEpoch);
      xStaticText->Enable(mShowXyz);
      xComboBox->Enable(mShowXyz);
      yStaticText->Enable(mShowXyz);
      yComboBox->Enable(mShowXyz);
      zStaticText->Enable(mShowXyz);
      zComboBox->Enable(mShowXyz);
//      updateStaticText->Enable(mShowUpdate);
//      secStaticText->Enable(mShowUpdate);
//      intervalTextCtrl->Enable(mShowUpdate);
      
      // disable some items
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
//      formatStaticText->Enable(false);
      secondaryStaticText->Enable(false);
      epochStaticText->Enable(false);
      originComboBox->Enable(false);
      typeComboBox->Enable(false);
      primaryComboBox->Enable(false);
//      formatComboBox->Enable(false);
      secondaryComboBox->Enable(false);
      epochTextCtrl->Enable(false);
      xStaticText->Enable(false);
      xComboBox->Enable(false);
      yStaticText->Enable(false);
      yComboBox->Enable(false);
      zStaticText->Enable(false);
      zComboBox->Enable(false);
//      updateStaticText->Enable(false);
//      secStaticText->Enable(false);
//      intervalTextCtrl->Enable(false);
   }
   
   // 07/18/2006 commented out because it caused a crash
   //theGuiInterpreter->RemoveItemIfNotUsed(Gmat::AXIS_SYSTEM, "tmpAxis");
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
//   formatComboBox->SetValue(epochFormatValue);
////   epochTextCtrl->SetValue("21545");
//   std::stringstream mjdStr("");
//   mjdStr << GmatTimeConstants::MJD_OF_J2000;
//   epochTextCtrl->SetValue(mjdStr.str());
   epochTextCtrl->SetValue(epochValue);
   xComboBox->SetValue("R");
   yComboBox->SetValue("");;
   zComboBox->SetValue("N");
//   intervalTextCtrl->SetValue("60");
}


//------------------------------------------------------------------------------
// void SetDefaultEpochRefAxis()
//------------------------------------------------------------------------------
void CoordPanel::SetDefaultEpochRefAxis()
{
   // default settings
//   formatComboBox->SetValue(epochFormatValue);
////   epochTextCtrl->SetValue("21545");
//   std::stringstream mjdStr("");
//   mjdStr << GmatTimeConstants::MJD_OF_J2000;
//   epochTextCtrl->SetValue(mjdStr.str());
    epochTextCtrl->SetValue(epochValue);
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
   #ifdef DEBUG_COORD_PANEL
   MessageInterface::ShowMessage
      ("CoordPanel::ShowAxisData() axis=(%p)%s\n", axis,
       axis->GetTypeName().c_str());
   #endif
   
   try
   {
      int sel = typeComboBox->FindString(axis->GetTypeName().c_str());
      typeComboBox->SetSelection(sel);
      EnableOptions(axis);
      
      #ifdef DEBUG_COORD_PANEL
      MessageInterface::ShowMessage
         ("mShowPrimaryBody=%d, mShowSecondaryBody=%d, mShowEpoch=%d, "
          "mShowXyz=%d, mShowUpdate\n", mShowPrimaryBody, mShowSecondaryBody,
          mShowEpoch, mShowXyz, mShowUpdate);
      #endif
      
      if (mShowPrimaryBody)
      {
         #ifdef DEBUG_COORD_PANEL_PRIMARY_SECONDARY
            MessageInterface::ShowMessage("Primary is %s\n", (axis->GetStringParameter("Primary")).c_str());
         #endif
         primaryComboBox->
            SetStringSelection(axis->GetStringParameter("Primary").c_str());
      }
      
      if (mShowSecondaryBody)
      {
         #ifdef DEBUG_COORD_PANEL_PRIMARY_SECONDARY
            MessageInterface::ShowMessage("Secondary is %s\n", (axis->GetStringParameter("Secondary")).c_str());
         #endif
         secondaryComboBox->
            SetStringSelection(axis->GetStringParameter("Secondary").c_str());
      }
      
      if (mShowEpoch)
      {
//         std::string epochFormat = axis->GetEpochFormat();
//         formatComboBox->SetStringSelection(epochFormat.c_str());
         
         Real epoch = axis->GetEpoch().Get();
         #ifdef DEBUG_COORD_PANEL
            MessageInterface::ShowMessage
               ("CoordPanel::ShowAxisData() about to set the value of epoch to %12.10f\n", epoch);
         #endif
         epochTextCtrl->SetValue(theGuiManager->ToWxString(epoch));
      }
      
      if (mShowXyz)
      {
         xComboBox->SetStringSelection(axis->GetXAxis().c_str());
         yComboBox->SetStringSelection(axis->GetYAxis().c_str());
         zComboBox->SetStringSelection(axis->GetZAxis().c_str());
      }
      
//      if (mShowUpdate)
//      {
//         /// @todo:
//         Real updateInterval = axis->GetRealParameter("UpdateInterval");
//
//         wxString updateStr;
//         std::stringstream buffer;
//         buffer.precision(18);
//         buffer << updateInterval;
//         updateStr.Printf ("%s",buffer.str().c_str());
//
//         intervalTextCtrl->SetValue(updateStr);
//      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CoordPanel::ShowAxisData() error occurred in getting data!\n%s\n",
          e.GetFullMessage().c_str());
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
//   wxString epochFormat = formatComboBox->GetValue().Trim();
   wxString epochStr = epochTextCtrl->GetValue().Trim();
//   wxString updateStr = intervalTextCtrl->GetValue().Trim();
   wxString xStr = xComboBox->GetValue();
   wxString yStr = yComboBox->GetValue();
   wxString zStr = zComboBox->GetValue();
   
   AxisSystem *axis = NULL;
   
   if (IsValidAxis(axisType, priName, secName, xStr, yStr, zStr))
   {
      // Create AxisSystem
      axis = (AxisSystem *)theGuiInterpreter->
         //CreateAxisSystem(std::string(axisType.c_str()), "");
         CreateObject(axisType.c_str(), "");
      
      if (axis != NULL)
      {
         try
         {
            if (axis->UsesPrimary()) //  && priName != "")
            {
               SpacePoint *primary = (SpacePoint *)theGuiInterpreter->
                  GetConfiguredObject(std::string(priName.c_str()));
               axis->SetPrimaryObject(primary);
            }
            
            if (axis->UsesSecondary()) //  && secName != "")
            {
               SpacePoint *secondary = (SpacePoint *)theGuiInterpreter->
                  GetConfiguredObject(std::string(secName.c_str()));
               axis->SetSecondaryObject(secondary);
            }
            
            if (axis->UsesXAxis() || axis->UsesYAxis() || axis->UsesZAxis())
            {
               // set the x, y, and z
               axis->SetXAxis(std::string(xStr.c_str()));
               axis->SetYAxis(std::string(yStr.c_str()));
               axis->SetZAxis(std::string(zStr.c_str()));
            }
            
//            axis->SetEpochFormat(std::string(epochFormat.c_str()));
//            axis->SetRealParameter("UpdateInterval", atof(updateStr.c_str()));
            
            // convert epoch to a1mjd
            // if Epoch is not in A1ModJulian, convert to A1ModJulian(loj: 1/23/07)
            if (axis->UsesEpoch())
            {
               Real a1mjd;
               GmatStringUtil::ToReal(epochStr.c_str(), a1mjd);

   //            if (epochFormat != "" && epochFormat != "A1ModJulian")
   //            {
   //               // Use TimsSystemConverter instead of TimeConverter
   //               Real inputMjd = -999.999;
   //               Real a1mjd;
   //               std::string a1mjdStr;
   //               TimeConverterUtil::Convert(epochFormat.c_str(), inputMjd,
   //                                          epochStr.c_str(), "A1ModJulian",
   //                                          a1mjd, a1mjdStr);
   //
   //               //std::string taiEpochStr = mTimeConverter.Convert
   //               //   (std::string(epochStr.c_str()), std::string(epochFormat.c_str()),
   //               //    "TAIModJulian");
   //
   //               //Real epoch = TimeConverterUtil::ConvertFromTaiMjd
   //               //   (TimeConverterUtil::A1MJD, atof(taiEpochStr.c_str()),
   //               //    GmatTimeConstants::JD_JAN_5_1941);
   //            }

               #ifdef DEBUG_COORD_PANEL
                  MessageInterface::ShowMessage
                     ("CoordPanel::CreateAxis() about to set the value of epoch on axis to %12.10f\n",
                           a1mjd);
               #endif
               axis->SetEpoch(a1mjd);
            }
            
         }
         catch (BaseException &e)
         {
            MessageInterface::ShowMessage
               ("CoordPanel::CreateAxis() error occurred in setting data!\n%s\n",
                e.GetFullMessage().c_str());
            
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
/*   wxString newFormat = formatComboBox->GetStringSelection().Trim();

   #ifdef DEBUG_COORD_PANEL
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
   }*/
   
//      std::string toEpochFormat = formatComboBox->GetStringSelection().c_str();    
//      std::string epochStr = epochTextCtrl->GetValue().c_str();
//      theSpacecraft->SetDateFormat(toEpochFormat);
//      epochTextCtrl->SetValue(theSpacecraft->GetStringParameter("Epoch").c_str());
//      oldFormat = toEpochFormat;
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
// void Setup( wxWindow *parent)
//------------------------------------------------------------------------------
void CoordPanel::Create()
{
   #ifdef DEBUG_COORD_PANEL_CREATE
   MessageInterface::ShowMessage("CoordPanel::Create() entered\n");
   #endif
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Coordinate System"));

    // wxStaticText
   originStaticText = new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Origin"),
      wxDefaultPosition, wxDefaultSize, 0 );
   typeStaticText = new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Type"),
      wxDefaultPosition, wxDefaultSize, 0 );
   primaryStaticText = new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Primary"),
      wxDefaultPosition, wxDefaultSize, 0 );
//   formatStaticText = new wxStaticText( this, ID_TEXT, wxT("Epoch "GUI_ACCEL_KEY"Format"),
//      wxDefaultPosition, wxDefaultSize, 0 );
   secondaryStaticText = new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Secondary"),
      wxDefaultPosition, wxDefaultSize, 0 );
   epochStaticText = new wxStaticText( this, ID_TEXT, wxT("A1MJD "GUI_ACCEL_KEY"Epoch"),
      wxDefaultPosition, wxDefaultSize, 0 );
//   updateStaticText = new wxStaticText( this, ID_TEXT, wxT("Update "GUI_ACCEL_KEY"Interval"),
//      wxDefaultPosition, wxDefaultSize, 0 );
//   secStaticText = new wxStaticText( this, ID_TEXT, wxT("seconds"),
//      wxDefaultPosition, wxDefaultSize, 0 );

   xStaticText = new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"X: "),
      wxDefaultPosition, wxDefaultSize, 0 );
   yStaticText = new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Y: "),
      wxDefaultPosition, wxDefaultSize, 0 );
   zStaticText = new wxStaticText( this, ID_TEXT, wxT(GUI_ACCEL_KEY"Z: "),
      wxDefaultPosition, wxDefaultSize, 0 );
          
   #if __WXMAC__
   wxStaticText *title1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Axes"),
                        wxDefaultPosition, wxSize(120,20),
                        wxBOLD);
   title1StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                    true, _T(""), wxFONTENCODING_SYSTEM));
   #endif

   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;

   // wxComboBox
   originComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);
   originComboBox->SetToolTip(pConfig->Read(_T("OriginHint")));
   typeComboBox = new wxComboBox
      ( this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(150,-1), //0,
        emptyList, wxCB_DROPDOWN|wxCB_READONLY );
   typeComboBox->SetToolTip(pConfig->Read(_T("TypeHint")));
   primaryComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);
   primaryComboBox->SetToolTip(pConfig->Read(_T("PrimaryHint")));
//   formatComboBox = new wxComboBox
//      ( this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(120,-1), //0,
//        emptyList, wxCB_DROPDOWN|wxCB_READONLY );
//   formatComboBox->SetToolTip(pConfig->Read(_T("EpochFormatHint")));
   secondaryComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);
   secondaryComboBox->SetToolTip(pConfig->Read(_T("SecondaryHint")));
   xComboBox = new wxComboBox
      ( this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(60,-1), //0,
        emptyList, wxCB_DROPDOWN|wxCB_READONLY );
   xComboBox->SetToolTip(pConfig->Read(_T("XHint")));
   yComboBox = new wxComboBox
      ( this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(60,-1), //0,
        emptyList, wxCB_DROPDOWN|wxCB_READONLY );
   yComboBox->SetToolTip(pConfig->Read(_T("YHint")));
   zComboBox = new wxComboBox
      ( this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(60,-1), //0,
        emptyList, wxCB_DROPDOWN|wxCB_READONLY );
   zComboBox->SetToolTip(pConfig->Read(_T("ZHint")));

   //wxTextCtrl
   epochTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
   epochTextCtrl->SetToolTip(pConfig->Read(_T("EpochHint")));
//   intervalTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
//      wxDefaultPosition, wxSize(45,-1), 0 );
//   intervalTextCtrl->SetToolTip(pConfig->Read(_T("UpdateIntervalHint")));

   // wx*Sizers
   wxBoxSizer *theMainSizer = new wxBoxSizer( wxVERTICAL );
   #if __WXMAC__
   wxBoxSizer *boxsizer4 = new wxBoxSizer( wxVERTICAL );
   #else
   wxStaticBox *staticbox1 = new wxStaticBox( this, -1, wxT("Axes") );
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
//   flexgridsizer1->Add( formatStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
//   flexgridsizer1->Add( formatComboBox, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( epochStaticText, 0, wxALIGN_LEFT|wxALL, 5 );
   flexgridsizer1->Add( epochTextCtrl, 0, wxALIGN_LEFT|wxALL, 5 );

   boxsizer2->Add(xStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(xComboBox, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(yStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(yComboBox, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(zStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
   boxsizer2->Add(zComboBox, 0, wxALIGN_CENTER|wxALL, 5 );

//   boxsizer3->Add(updateStaticText, 0, wxALIGN_CENTER|wxALL, 5 );
//   boxsizer3->Add(intervalTextCtrl, 0, wxALIGN_CENTER|wxALL, 5 );
//   boxsizer3->Add(secStaticText, 0, wxALIGN_CENTER|wxALL, 5 );

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
         new wxStaticText(this, ID_TEXT,
                          wxT("This is a default Coordinate "
                              "System and cannot be modified."),
                          wxDefaultPosition, wxDefaultSize, 0);
      msg->SetForegroundColour(*wxRED);
      theMainSizer->Add(msg, 0, wxALIGN_CENTRE|wxALL, 5);
   }
   
   this->SetAutoLayout( true );
   this->SetSizer( theMainSizer );
   theMainSizer->Fit( this );
   theMainSizer->SetSizeHints( this );
   
   #ifdef DEBUG_COORD_PANEL_CREATE
   MessageInterface::ShowMessage("CoordPanel::Create() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void CoordPanel::LoadData()
{
   #ifdef DEBUG_COORD_PANEL_LOAD
   MessageInterface::ShowMessage("CoordPanel::LoadData() entered\n");
   #endif
   
   try
   {
      // Load axes types
      StringArray itemNames =
         theGuiInterpreter->GetListOfFactoryItems(Gmat::AXIS_SYSTEM);
      for (unsigned int i = 0; i<itemNames.size(); i++)
         typeComboBox->Append(wxString(itemNames[i].c_str()));
      
//      // insert a blank option for secondary
//      secondaryComboBox->Append("");
      
      StringArray reps = TimeConverterUtil::GetValidTimeRepresentations();
      
/*      // Load epoch types - hard coded for now
      wxString epochStrs[] =
      {
         wxT("TAIModJulian"),
         wxT("UTCModJulian"),
         wxT("TAIGregorian"),
         wxT("UTCGregorian"),
      };
      
      for (unsigned int i = 0; i<4; i++)
         formatComboBox->Append(wxString(epochStrs[i].c_str()));*/
      
//      for (unsigned int i = 0; i < reps.size(); i++)
//         formatComboBox->Append(reps[i].c_str());
      
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
            e.GetFullMessage().c_str());
   }
   
   #ifdef DEBUG_COORD_PANEL_LOAD
   MessageInterface::ShowMessage("CoordPanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// bool SaveData(const std::string &coordName, AxisSystem *axis,
//               const wxString &epochFormat)
//------------------------------------------------------------------------------
bool CoordPanel::SaveData(const std::string &coordName, AxisSystem *axis,
                          wxString &epochFormat)
{
   #ifdef DEBUG_COORD_PANEL_SAVE
   MessageInterface::ShowMessage
      ("oordPanel::SaveData() entered, coordName=%s, epochFormat=%s, epoch = %s\n",
       coordName.c_str(), epochFormat.c_str(), epochValue.c_str());
   #endif
   
   bool canClose = true;
   
   try
   {
      std::string inputString;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \""
                         + coordName + 
                        "\" is not an allowed value. \n"
                        "The allowed values are: [%s].";                        

      // create CoordinateSystem if not exist
      CoordinateSystem *coordSys =
         (CoordinateSystem*)theGuiInterpreter->GetConfiguredObject(coordName);
      
      if (coordSys == NULL)
      {
         coordSys = (CoordinateSystem*)
            theGuiInterpreter->CreateObject("CoordinateSystem", coordName);
         
         if (coordSys)
         {
            #ifdef DEBUG_COORD_PANEL
            MessageInterface::ShowMessage
               ("CoordPanel::SaveData() coordName=%s created.\n",
                coordName.c_str());
            #endif
         }
         else
         {
            canClose = false;
            return false;
         }
      }
      
      //-------------------------------------------------------
      // set origin and Axis
      //-------------------------------------------------------
      wxString originName = originComboBox->GetValue().Trim();
      coordSys->SetStringParameter("Origin", std::string(originName.c_str()));
      coordSys->SetRefObject(axis, Gmat::AXIS_SYSTEM, "");
      
      SpacePoint *origin =
         (SpacePoint*)theGuiInterpreter->GetConfiguredObject(originName.c_str());
      
      coordSys->SetOrigin(origin);
      
      CelestialBody *j2000body =
         (CelestialBody*)theGuiInterpreter->GetConfiguredObject("Earth");
      
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
            GetConfiguredObject(primaryName.c_str());
         
         axis->SetStringParameter("Primary", primaryName.c_str());
         axis->SetPrimaryObject(primary);
      }
      
      // set secondary body if exist
      if (secondaryComboBox->IsEnabled())
      {
         wxString secondaryName = secondaryComboBox->GetValue().Trim();

         axis->SetStringParameter("Secondary", secondaryName.c_str());
         
         if (secondaryName != "")
         {
            SpacePoint *secondary = (SpacePoint*)theGuiInterpreter->
               GetConfiguredObject(secondaryName.c_str());
         
            axis->SetSecondaryObject(secondary);
            if (secondary->GetJ2000Body() == NULL)
               secondary->SetJ2000Body(j2000body);
         }
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
         Real epoch, a1mjd;
         std::string savedEpoch = epochValue.c_str();
         #ifdef DEBUG_COORD_PANEL_SAVE
            MessageInterface::ShowMessage("In CoordPanel::SaveData, saving current epoch value (%s)\n",
                  savedEpoch.c_str());
         #endif
         
         inputString = epochTextCtrl->GetValue();
         if ((GmatStringUtil::ToReal(inputString, &epoch)) &&
             (epoch >= DateUtil::EARLIEST_VALID_MJD_VALUE) && (epoch <= DateUtil::LATEST_VALID_MJD_VALUE))
         {
            epochValue = epochTextCtrl->GetValue();
            a1mjd      = epoch;
//            if (epochFormat != newEpochFormat)
//            {
//               axis->SetEpochFormat(newEpochFormat.c_str());
//               epochFormat = newEpochFormat;
//
//               //convert epoch to A1ModJulian if not in this format
//               //if (newEpochFormat != "TAIModJulian")
//               if (newEpochFormat != "" && newEpochFormat != "A1ModJulian")
//               {
//                  std::string a1mjdStr;
//                  TimeConverterUtil::Convert(epochFormat.c_str(), epoch,
//                                             "", "A1ModJulian",
//                                             a1mjd, a1mjdStr);
//
//                  //std::string taiEpochStr = mTimeConverter.Convert
//                  //   (epochStr, newEpochFormat.c_str(), "TAIModJulian");
//                  //epoch = TimeConverterUtil::ConvertFromTaiMjd
//                  //   (TimeConverterUtil::A1MJD, atof(taiEpochStr.c_str()),
//                  //    GmatTimeConstants::JD_JAN_5_1941);
//               }
//            }
            #ifdef DEBUG_COORD_PANEL_SAVE
               MessageInterface::ShowMessage("In CoordPanel::SaveData, setting epoch on axis to %12.10f\n",
                     a1mjd);
            #endif
            axis->SetEpoch(a1mjd);
         }
         else
         {
//            epochTextCtrl->SetValue(epochValue);
            std::string warnmsg = DateUtil::EARLIEST_VALID_MJD;
            warnmsg += " <= Real Number <= " + DateUtil::LATEST_VALID_MJD;
            MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
               inputString.c_str(),"Epoch",warnmsg.c_str());
            canClose = false;
         }
      }
      
      //-------------------------------------------------------
      // set new update interval
      //-------------------------------------------------------
//      if (intervalTextCtrl->IsEnabled())
//      {
//         Real interval;
//         inputString = intervalTextCtrl->GetValue();
//         if ((GmatStringUtil::ToReal(inputString,&interval)) &&
//             (interval >= 0.0))
//         {
//            axis->SetRealParameter("UpdateInterval", interval);
//         }
//         else
//         {
//            MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(),
//               inputString.c_str(),"Update Interval","Real Number >= 0.0");
//            canClose = false;
//         }
//      }
      
      // set solar system
      coordSys->SetSolarSystem(theGuiInterpreter->GetSolarSystemInUse());
      coordSys->Initialize();

   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("*** Error *** %s\n", e.GetFullMessage().c_str());
      canClose = false;
   }

   #ifdef DEBUG_COORD_PANEL_SAVE
   MessageInterface::ShowMessage
      ("CoordPanel::SaveData() returning canClose = %d\n", canClose);
   #endif
   
   return canClose;
}

