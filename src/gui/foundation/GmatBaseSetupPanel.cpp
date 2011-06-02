//$Id$
//------------------------------------------------------------------------------
//                           GmatBaseSetupPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Linda Jun
// Created: 2008/11/19
/**
 * This class is a generic setup panel used by objects derived from GmatBase
 */
//------------------------------------------------------------------------------

#include "GmatBaseSetupPanel.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"         // for GetPathname()
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/config.h>
#include "StringUtil.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "FileUtil.hpp"
#include <map>
#include <algorithm>
#include <utility> // make_pair
#include "GmatBaseException.hpp"
#include "UtilityException.hpp"
#include "bitmaps/OpenFolder.xpm"

//#define DEBUG_BASEPANEL_LOAD
//#define DEBUG_BASEPANEL_CREATE
//#define DEBUG_SAVE_CONTROL

//-----------------------------------------
// static members
//-----------------------------------------
const wxString GmatBaseSetupPanel::TF_SCHEMES[2] =
   {
      wxT("Off"),
      wxT("On")
   };

/// wxWidget event mappings for the panel
BEGIN_EVENT_TABLE(GmatBaseSetupPanel, GmatPanel)
   EVT_COMBOBOX(ID_COMBOBOX, GmatBaseSetupPanel::OnComboBoxChange)
   EVT_TEXT(ID_COMBOBOX, GmatBaseSetupPanel::OnComboBoxTextChange)
   EVT_TEXT(ID_TEXTCTRL, GmatBaseSetupPanel::OnTextChange)
   EVT_CHECKBOX(ID_CHECKBOX, GmatBaseSetupPanel::OnComboBoxChange)
   EVT_BUTTON(ID_BUTTON_BROWSE, GmatBaseSetupPanel::OnBrowseButton)
END_EVENT_TABLE()


//-----------------------------------------
// public methods
//-----------------------------------------

//------------------------------------------------------------------------------
// GmatBaseSetupPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Panel constructor
 *
 * @param parent Owner for this panel
 * @param name Name of the object that is to be configured
 */
//------------------------------------------------------------------------------
GmatBaseSetupPanel::GmatBaseSetupPanel(wxWindow *parent, const wxString &name)
   : GmatPanel(parent)
{
   #ifdef DEBUG_BASEPANEL_CREATE
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::Constructor() entered, name='%s'", name.c_str());
   #endif
   mObject = theGuiInterpreter->GetConfiguredObject(name.c_str());

   if (mObject != NULL)
   {
      Create();
      Show();
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The object named \"%s\" does not exist\n", name.c_str());
   }
}


//------------------------------------------------------------------------------
// ~GmatBaseSetupPanel()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GmatBaseSetupPanel::~GmatBaseSetupPanel()
{
   // Unregister automatically registered ComboBoxes
   std::map<wxString, wxComboBox *>::iterator iter;
   for (iter = managedComboBoxMap.begin(); iter != managedComboBoxMap.end(); ++iter)
      theGuiManager->UnregisterComboBox(iter->first, iter->second);
   delete localObject;
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Inherited function that is called to create the panel.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::Create()
{
   // create local copy of mObject
   localObject = mObject->Clone();

   #ifdef DEBUG_BASEPANEL_CREATE
   MessageInterface::ShowMessage
      (", localObject=<%p><%s> '%s'\n",
       localObject, localObject->GetTypeName().c_str(), localObject->GetName().c_str());
   #endif

   wxFlexGridSizer *mainSizer = new wxFlexGridSizer(1);
   CreateControls(mainSizer, localObject);

   // fix tab order
   FixTabOrder( NULL, mainSizer );
   theMiddleSizer->Add(mainSizer, 0, wxALL|wxALIGN_LEFT, 5);
}


//------------------------------------------------------------------------------
// SizerMapType* CreateGroups(wxFlexGridSizer *mainSizer, wxFileConfig *config)
//------------------------------------------------------------------------------
SizerMapType* GmatBaseSetupPanel::CreateGroups(wxFlexGridSizer *mainSizer,
                                               wxFileConfig *config)
{
   SizerMapType *groups = new SizerMapType;
   wxString groupName;
   wxSizer *sizer;
   wxSizer *parentSizer;
   wxString groupProp;
   wxString parentName;
   long dummy;
   bool doDefaultAction;
   SizerMapType::iterator sizerItem, item;

   // first get all of the groups
   bool bCont = config->GetFirstGroup(groupName, dummy);
   while (bCont)
   {
      // if not Main group, then it is a group/sizer
      if ((groupName != "main") && (config->Read("/"+groupName+"/Type", &groupProp)))
      {
         // "Type" can be VERTICAL, HORIZONTAL, FLEX, or GRID
         groupProp = groupProp.Lower();
         if (groupProp == "vertical")
           sizer = new GmatStaticBoxSizer( wxVERTICAL, this, config->Read("/"+groupName+"/Label") );
         else if (groupProp == "horizontal")
           sizer = new GmatStaticBoxSizer( wxHORIZONTAL, this, config->Read("/"+groupName+"/Label") );
         else if (groupProp == "flex")
         {
           sizer = new wxFlexGridSizer( config->Read("/"+groupName+"/Rows", (long) 0),
                           config->Read("/"+groupName+"/Columns", (long) 0),
                           config->Read("/"+groupName+"/VerticalGap", (long) 0),
                           config->Read("/"+groupName+"/HorizontalGap", (long) 0));
         }
         else if (groupProp == "grid")
         {
           sizer = new wxGridSizer( config->Read("/"+groupName+"/Rows", (long) 0),
                           config->Read("/"+groupName+"/Columns", (long) 0),
                           config->Read("/"+groupName+"/VerticalGap", (long) 0),
                           config->Read("/"+groupName+"/HorizontalGap", (long) 0));
         }
         else // property or unknown type
            continue;
         groups->insert(std::make_pair(groupName.Lower(), sizer));
      }
      // get next group
      bCont = config->GetNextGroup(groupName, dummy);
   }

   // create an ordered list of groups
   StringArray groupNames;
   for (item = groups->begin(); item != groups->end(); ++item)
   {
           groupName = item->first;
           groupNames.push_back(groupName.Lower().c_str());

   }
   SortGroups(&groupNames, config);

   // now, for all the groups, add them to their parent
   for (unsigned int i = 0; i < groupNames.size(); i++)
   {
      item = groups->find(groupNames[i].c_str());
      groupName = item->first;
      sizer = (wxSizer *) item->second;
      // set the parent of the sizer, if doesn't exist, then it is the main sizer
      if (config->Read("/"+groupName+"/Parent", &parentName))
      {
         sizerItem = groups->find(parentName.Lower());
         doDefaultAction = sizerItem == groups->end();
         if (!doDefaultAction)
         {
            parentSizer = ((wxSizer *) sizerItem->second);
            parentSizer->Add(sizer, 0, wxALL|wxALIGN_LEFT|wxEXPAND, border);
         }
         else
            mainSizer->Add(sizer, 0, wxALL|wxALIGN_LEFT|wxEXPAND, border);
      }
      else
         mainSizer->Add(sizer, 0, wxALL|wxALIGN_LEFT|wxEXPAND, border);
   }
   return groups;
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Populates the panel with the configurable property data in the Solver
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::LoadData()
{
   // load data from the core engine
   try
   {
      std::string label;
      Integer propertyCount = localObject->GetParameterCount();

      #ifdef DEBUG_BASEPANEL_LOAD
      MessageInterface::ShowMessage
         ("GmatBaseSetupPanel::LoadData() There are %d properties\n", propertyCount);
      #endif
      
      for (Integer i = 0; i < propertyCount; ++i)
      {
         if (localObject->IsParameterReadOnly(i) == false)
         {
            label = localObject->GetParameterText(i);
            LoadControl(localObject, label);
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("GmatBaseSetupPanel:LoadData() error occurred!\n%s\n",
          e.GetFullMessage().c_str());
   }

   // explicitly disable apply button
   // it is turned on in each of the panels
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Passes configuration data from the panel to the Solver object
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::SaveData()
{
   canClose = true;

   try
   {
      // copy the input to a local variable first to see if there are
      // any errors
      for (std::map<Integer, wxControl *>::iterator i = controlMap.begin();
            i != controlMap.end(); ++i)
      {
         SaveControl(localObject, localObject->GetParameterText(i->first), true);
         if (!canClose)
            return;
      }
      if (!localObject->Validate())
          throw GmatBaseException("Invalid input\n");

      // if no errors, copy the object again
      for (std::map<Integer, wxControl *>::iterator i = controlMap.begin();
            i != controlMap.end(); ++i)
      {
         SaveControl(mObject, localObject->GetParameterText(i->first), false);
         if (!canClose)
            return;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
   EnableUpdate(false);
}


//------------------------------------------------------------------------------
// void CreateControls(wxFlexGridSizer *mainSizer, GmatBase *theObject)
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::CreateControls(wxFlexGridSizer *mainSizer, GmatBase *theObject)
{
   wxFileConfig *pConfig;
   SizerMapType *groups;
   StringArray propertyNames;
   std::vector<wxString> propertyGroups;
   
   #ifdef DEBUG_BASEPANEL_CREATE
   //MessageInterface::PopupMessage
   //   (Gmat::INFO_, "Creating controls for %s...\n", theObject->GetTypeName().c_str());
   MessageInterface::ShowMessage
      ("Creating controls for %s...\n", theObject->GetTypeName().c_str());
   #endif
   GetLayoutConfig( theObject, &pConfig );
   
   wxFlexGridSizer *mainItemSizer = new wxFlexGridSizer(1); // for properties that don't have parents
   mainSizer->Add(mainItemSizer, 0, wxALL|wxALIGN_LEFT, border);
   
   // create the groups
   groups = CreateGroups(mainSizer, pConfig);
   
   // get the property names
   Integer propertyCount = theObject->GetParameterCount();
   for (Integer i = 0; i < propertyCount; ++i)
   {
      #ifdef DEBUG_BASEPANEL_CREATE
      if (theObject->IsParameterReadOnly(i))
         MessageInterface::ShowMessage
           ("   ParameterText(%d)='%s' (ReadOnly=True)\n", i, theObject->GetParameterText(i).c_str());
      else
        MessageInterface::ShowMessage
          ("   ParameterText(%d)='%s' ReadOnly=False\n", i, theObject->GetParameterText(i).c_str());
      #endif
      if (!theObject->IsParameterReadOnly(i))
         propertyNames.push_back(theObject->GetParameterText(i));
   }
   
   // Create the properties
   CreateProperties(mainItemSizer, theObject, &propertyNames, groups, pConfig);
   
   delete pConfig;
   delete groups;
}


//------------------------------------------------------------------------------
// wxControl* CreateControls(wxWindow *parent, Integer index, ...)
//------------------------------------------------------------------------------
/**
 * Creates label, control, and unit widgets for a property
 *
 * @param index The index for the property that the constructed control
 *              represents
 *
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::CreateControls(GmatBase *theObject, Integer index,
                                        wxStaticText **aLabel, wxControl **aControl,
                                        wxControl **aUnit, wxFileConfig *config)
{
    wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);
    #if __WXMAC__
    int buttonWidth = 40;
    #else
    int buttonWidth = 25;
    #endif
    std::string labelText;
    // set the path to the section that contains the parameter's items
    config->SetPath(wxT(("/"+theObject->GetParameterText(index)).c_str()));
    labelText = GetParameterLabel(theObject, index, config);
    labelText = AssignAcceleratorKey(labelText);
    
    if (theObject->GetParameterType(index) != Gmat::BOOLEAN_TYPE)
       *aLabel = new wxStaticText(this, ID_TEXT, labelText.c_str());
    else
       *aLabel = new wxStaticText(this, ID_TEXT, "");
    
    *aControl = BuildControl(this, theObject, index, labelText, config);
    (*aControl)->Enable(theObject->IsParameterEnabled(index));
    
    if (theObject->GetParameterType(index) == Gmat::FILENAME_TYPE)
    {
       *aUnit =
          new wxBitmapButton(this, ID_BUTTON_BROWSE, openBitmap, wxDefaultPosition,
                             wxSize(buttonWidth, 20));
    }
    else
       *aUnit = new wxStaticText(this, ID_TEXT,
                                 wxT(GetParameterUnit(theObject, index, config).c_str()));
}


//------------------------------------------------------------------------------
// wxControl* CreateProperties(wxFlexGridSizer *mainSizer,
//            StringArray propertyNames, SizerMapType *groups, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Creates controls for all the properties of an object and
 * put them in the right groups
 *
 *
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::CreateProperties(wxFlexGridSizer *mainSizer,
                                          GmatBase *theObject, StringArray *propertyNames,
                                          SizerMapType *groups, wxFileConfig *config)
{
   Integer j = 0;
   Integer i = 0;
   wxString groupProp;
   wxFlexGridSizer *itemSizer;
   wxSizer *sizer;
   std::vector<wxString> propertyGroups;
   wxStaticText *aLabel;
   wxControl *aControl;
   wxControl *aUnit;
   /// Labels used for the configurable properties
   std::vector<wxStaticText*>       propertyDescriptors;
   /// GUI controls that are used to configure the properties
   std::vector<wxControl*>          propertyControls;
   /// Units used for the configurable properties
   std::vector<wxControl*>          propertyUnits;
   
   
   // sort the properties per the config file
   SortProperties( propertyNames, config );
   
   // now go through the properties and create their controls
   StringArray::iterator propertyItem;
   for(propertyItem = propertyNames->begin(), j=0;
       propertyItem != propertyNames->end(); ++propertyItem, ++j)
   {
      i = theObject->GetParameterID(*propertyItem);
      CreateControls(theObject, i, &aLabel, &aControl, &aUnit, config);
      propertyDescriptors.push_back(aLabel);
      controlMap[i] = aControl;
      inverseControlMap[aControl] = i;
      propertyControls.push_back(aControl);
      propertyUnits.push_back(aUnit);
   }
   
   // three columns: description, control, unit
   std::vector<wxStaticText*>::iterator item;
   
   // find the best minimum size for the description/unit for each sizer/group
   
   // Add the 3 columns to a sizer
   SizerMapType::iterator sizerItem;
   bool doDefaultAction;
   
   for(item = propertyDescriptors.begin(), j = 0;
       item != propertyDescriptors.end(); ++item, ++j)
   {
      itemSizer = new wxFlexGridSizer(3);
      itemSizer->Add(*item, 0, wxALL|wxALIGN_RIGHT, border);
      itemSizer->Add(propertyControls[j], 0, wxALL|wxALIGN_LEFT, border);
      itemSizer->Add(propertyUnits[j], 0, wxALL|wxALIGN_LEFT, border);
      // set the path to the section that contains the parameter's items
      config->SetPath(wxT(("/"+(*propertyNames)[j])).c_str());
      groupProp = "Parent";
      doDefaultAction = !(config->Read(groupProp, &groupProp));
      if (!doDefaultAction)
      {
         // save the group for normalizing labels later
         propertyGroups.push_back(groupProp);
         // get the group and its itemsizer inside
         sizerItem = groups->find(groupProp.Lower());
         doDefaultAction = sizerItem == groups->end();
         if (!doDefaultAction)
         {
            sizer = ((wxSizer *) sizerItem->second);
            sizer->Add(itemSizer, 0, wxALL|wxALIGN_LEFT, 0);
         }
      }
      if (doDefaultAction)
      {
         // save the group for normalizing labels later
         propertyGroups.push_back("Main");
         mainSizer->Add(itemSizer, 0, wxALL|wxALIGN_LEFT, 0);
      }
   }

   
   
//    // see if there are any LoadSubProperties
//    wxString areSubProperties;
//    std::string refObjectName;
//    GmatBase *subObject;
//    for(propertyItem = propertyNames->begin(), j=0;
//        propertyItem != propertyNames->end(); ++propertyItem, ++j)
//    {
//       // set the path to the section that contains the parameter's items
//       config->SetPath(wxT(("/"+(*propertyNames)[j])).c_str());
//       doDefaultAction = config->Read(wxT("LoadSubProperties"), &areSubProperties);
//       if ((doDefaultAction) && (areSubProperties.Lower() == "true"))
//       {
//          groupProp = "SubPropertiesParent";
//          doDefaultAction = !(config->Read(groupProp, &groupProp));
//          if (!doDefaultAction)
//          {
//             // get the group and its itemsizer inside
//             sizerItem = groups->find(groupProp.Lower());
//             doDefaultAction = sizerItem == groups->end();
//             if (!doDefaultAction)
//             {
//                sizer = ((wxSizer *) sizerItem->second);
//                itemSizer = new wxFlexGridSizer(1);
//                sizer->Add(itemSizer, 0, wxALL|wxALIGN_LEFT, 0);
//                try
//                {
//                   refObjectName = theObject->
//                      GetRefObjectName(theObject->GetPropertyObjectType
//                                       (theObject->GetParameterID((*propertyNames)[j])));
//                   if (refObjectName != "")
//                   {
//                      subObject = theObject->GetRefObject
//                         (theObject->GetPropertyObjectType
//                          (theObject->GetParameterID((*propertyNames)[j])), refObjectName);
//                      if (subObject == NULL)
//                         MessageInterface::ShowMessage
//                            ("Programmer error: Object property is NULL for %s...\n",
//                             refObjectName.c_str());
//                      else
//                         CreateControls(mainSizer, subObject);
//                   }
//                }
//                catch (BaseException &e)
//                {
//                   MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
//                }
//             }
//          }
//          else
//          {
            
//          }
//       }
//    }
   
   
   NormalizeLabels( *propertyNames, propertyGroups, propertyDescriptors, propertyControls, propertyUnits );

}

//------------------------------------------------------------------------------
// wxControl* BuildControl(wxWindow *parent, Integer index)
//------------------------------------------------------------------------------
/**
 * Builds a wxWidget control for an object property
 *
 * @param parent The window that owns the control
 * @param index The index for the property that the constructed control
 *              represents
 *
 * @return The new control
 */
//------------------------------------------------------------------------------
wxControl *GmatBaseSetupPanel::BuildControl(wxWindow *parent, GmatBase *theObject,
                                            Integer index, const std::string &label,
                                            wxFileConfig *config)
{
   wxControl *control = NULL;
   
   Gmat::ParameterType type = theObject->GetParameterType(index);
   
   #ifdef DEBUG_BASEPANEL_CREATE
   MessageInterface::ShowMessage
      ("Building control %s (type=%s)\n", label.c_str(),
       theObject->GetParameterTypeString(index).c_str());
   #endif
   switch (type)
   {
   case Gmat::ON_OFF_TYPE:
      {
         wxComboBox *cbControl =
            new wxComboBox(parent, ID_COMBOBOX, "true",
                           wxDefaultPosition, wxDefaultSize, 2, TF_SCHEMES,
                           wxCB_READONLY);
         
         control = cbControl;
      }
      break;
   case Gmat::BOOLEAN_TYPE:
      {
         wxCheckBox *cbControl = new wxCheckBox( this, ID_CHECKBOX, wxT(label.c_str()));
         cbControl->SetToolTip(config->Read(_T("Hint")));
         control = cbControl;
      }
      break;
   case Gmat::OBJECT_TYPE:
      {
         Gmat::ObjectType type = theObject->GetPropertyObjectType(index);
         #ifdef DEBUG_BASEPANEL_CREATE
         MessageInterface::ShowMessage
            ("    object type is %s\n", GmatBase::GetObjectTypeString(type).c_str());
         #endif
         if (type == Gmat::SPACE_POINT)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX,
                                                    wxSize(180,-1), false);
            managedComboBoxMap.insert(std::make_pair("SpacePoint", cbControl));
            control = cbControl;
         }
         else if (type == Gmat::CELESTIAL_BODY)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
            theGuiManager->GetCelestialBodyComboBox(this, ID_COMBOBOX,
                                                    wxSize(180,-1));
            managedComboBoxMap.insert(std::make_pair("CelestialBody", cbControl));
            control = cbControl;
         }
         else if (type == Gmat::SPACECRAFT)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetSpacecraftComboBox(this, ID_COMBOBOX,
                                                    wxSize(180,-1));
            managedComboBoxMap.insert(std::make_pair("Spacecraft", cbControl));
            control = cbControl;
         }
         else if (type == Gmat::COORDINATE_SYSTEM)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX,
                                                  wxSize(180,-1));
            managedComboBoxMap.insert(std::make_pair("CoordinateSystem", cbControl));
            control = cbControl;
         }
         else if (type == Gmat::ANTENNA)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetAntennaComboBox(this, ID_COMBOBOX,
                                                  wxSize(180,-1));
            managedComboBoxMap.insert(std::make_pair("Antenna", cbControl));
            control = cbControl;
         }
         else if (type == Gmat::SENSOR)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetSensorComboBox(this, ID_COMBOBOX,
                                                  wxSize(180,-1));
            managedComboBoxMap.insert(std::make_pair("Sensor", cbControl));
            control = cbControl;
         }
         else
         {

            // Check if enumeration strings available for owned object types
            wxArrayString enumList;
            StringArray enumStrings = theGuiInterpreter->GetListOfObjects(type);
            //StringArray enumStrings = theObject->GetPropertyEnumStrings(index);
            for (UnsignedInt i=0; i<enumStrings.size(); i++)
               enumList.Add(enumStrings[i].c_str());
            control = new wxComboBox(parent, ID_COMBOBOX, wxT(""),
                                     wxDefaultPosition, wxSize(180,-1), enumList,
                                     wxCB_READONLY);
         }
      }
      break;
   case Gmat::OBJECTARRAY_TYPE:
      {
         //@todo Create ListBox later
         control = new wxTextCtrl(parent, ID_TEXTCTRL,
                                  wxT(""), wxDefaultPosition, wxSize(180,-1));
      }
      break;
   case Gmat::ENUMERATION_TYPE:
      {
         StringArray enumStrings = theObject->GetPropertyEnumStrings(index);
         wxArrayString enumList;
         for (UnsignedInt i=0; i<enumStrings.size(); i++)
            enumList.Add(enumStrings[i].c_str());
         
         wxComboBox *cbControl = NULL;
         if (enumStrings.size() == 1)
         {
            // Show the value even if value is not in the list
            cbControl =
               new wxComboBox(parent, ID_COMBOBOX, "", wxDefaultPosition,
                              wxSize(180,-1), enumList, 0);
         }
         else
         {
            // Do not show the value if value is not in the list
            cbControl =
               new wxComboBox(parent, ID_COMBOBOX, "", wxDefaultPosition,
                              wxSize(180,-1), enumList, wxCB_READONLY);
         }
         
         control = cbControl;
      }
      break;
   case Gmat::REAL_TYPE:
   case Gmat::INTEGER_TYPE:
      {
         control = new wxTextCtrl(parent, ID_TEXTCTRL,
                                  wxT(""), wxDefaultPosition, wxSize(180,-1), 0,
                                  wxTextValidator(wxGMAT_FILTER_NUMERIC));
      }
      break;
   case Gmat::FILENAME_TYPE:
   case Gmat::STRING_TYPE:
   default:
      control = new wxTextCtrl(parent, ID_TEXTCTRL,
                               wxT(""), wxDefaultPosition, wxSize(180,-1));
      break;
   }
   
   control->SetToolTip(config->Read(_T("Hint")));
   return control;
}


//------------------------------------------------------------------------------
// void LoadControl(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Sets the data for a control
 *
 * @param label The control's label
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::LoadControl(GmatBase *theObject, const std::string &label)
{
   #ifdef DEBUG_BASEPANEL_LOAD
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::LoadControl() label='%s'\n", label.c_str());
   #endif

   Integer index = theObject->GetParameterID(label);
   Gmat::ParameterType type = theObject->GetParameterType(index);
   
   wxControl *theControl = controlMap[index];
   wxString valueString;
   
   switch (type)
   {
      case Gmat::ON_OFF_TYPE:
         ((wxComboBox*)(theControl))->
            SetValue(wxT(theObject->GetOnOffParameter
                         (theObject->GetParameterID(label)).c_str()));
         break;
      case Gmat::BOOLEAN_TYPE:
          ((wxCheckBox*) theControl)->SetValue(theObject->
                GetBooleanParameter(theObject->GetParameterID(label)));
         break;
      case Gmat::REAL_TYPE:
         {
            Real val = theObject->GetRealParameter(
                  theObject->GetParameterID(label));
            valueString << val;
            ((wxTextCtrl*)theControl)->ChangeValue(valueString);
         }
         break;

      case Gmat::INTEGER_TYPE:
         {
            Integer val = theObject->GetIntegerParameter(
                  theObject->GetParameterID(label));
            valueString << val;
            ((wxTextCtrl*)theControl)->ChangeValue(valueString);
         }
         break;

      case Gmat::FILENAME_TYPE:
      case Gmat::STRING_TYPE:
         valueString = wxT(theObject->GetStringParameter(label).c_str());
         ((wxTextCtrl*)theControl)->ChangeValue(valueString);
         break;
         
      case Gmat::OBJECT_TYPE:
         valueString = (theObject->GetStringParameter(theObject->GetParameterID(label))).c_str();
         ((wxComboBox*)theControl)->SetStringSelection(valueString);
         //((wxComboBox*)theControl)->Append(valueString); // removed for Bug 1621 wcs 2009.11.10
         // why no break added? added break (loj: 2011.06.01)
         break;
      case Gmat::OBJECTARRAY_TYPE:
         valueString = (theObject->GetStringParameter(theObject->GetParameterID(label))).c_str();
         ((wxTextCtrl*)theControl)->ChangeValue(valueString);
         break;
      case Gmat::ENUMERATION_TYPE:
         {
            valueString = (theObject->GetStringParameter(theObject->GetParameterID(label))).c_str();
            wxComboBox *cb = (wxComboBox*)theControl;
            cb->SetValue(valueString);
            
            // if ComboBox is not read only, add value to list
            if (cb->GetWindowStyleFlag() != wxCB_READONLY)
               cb->Append(valueString);
            break;
         }
      default:
         break;
   }
}


//------------------------------------------------------------------------------
// void SaveControl(GmatBase *theObject, const std::string &label, ...)
//------------------------------------------------------------------------------
/**
 * Passes a control's data to the object.  Returns true if parameter was saved to
 * object
 *
 * @param label The string associated with the control.
 */
//------------------------------------------------------------------------------
bool GmatBaseSetupPanel::SaveControl(GmatBase *theObject, const std::string &label,
                                     bool showErrorMsgs)
{
   #ifdef DEBUG_SAVE_CONTROL
   MessageInterface::ShowMessage
      ("SaveControl() entered, theObject=<%p>, label='%s', "
       "showErrorMsgs=%d\n", theObject, label.c_str(), showErrorMsgs);
   #endif
   
   Integer index = theObject->GetParameterID(label);
   Gmat::ParameterType type = theObject->GetParameterType(index);
   
   wxControl *theControl = controlMap[index];
   std::string valueString;
   
   switch (type)
   {
   case Gmat::ON_OFF_TYPE:
      {
         valueString = ((wxComboBox*)theControl)->GetValue();
         theObject->SetOnOffParameter(index, valueString);
      }
      break;
   case Gmat::BOOLEAN_TYPE:
      {
         theObject->SetBooleanParameter(index, ((wxCheckBox*)theControl)->GetValue());
      }
      break;
   case Gmat::REAL_TYPE:
      {
         Real val;
         valueString = ((wxTextCtrl*)theControl)->GetValue();
         if (showErrorMsgs)
         {
            CheckReal(val, valueString, label, "Real Number", false);
            if (!canClose)
               return false;
         }
         else if (!GmatStringUtil::ToReal(valueString, &val))
            return false;
         theObject->SetRealParameter(index, val);
      }
      break;
      
   case Gmat::INTEGER_TYPE:
      {
         Integer val;
         valueString = ((wxTextCtrl*)theControl)->GetValue();
         if (showErrorMsgs)
         {
            CheckInteger(val, valueString, label, "Integer", false);
            if (!canClose)
               return false;
            
         }
         else if (!GmatStringUtil::ToInteger(valueString, &val))
            return false;
         theObject->SetIntegerParameter(index, val);
      }
      break;
      
   case Gmat::FILENAME_TYPE:
   case Gmat::STRING_TYPE:
      valueString = ((wxTextCtrl*)theControl)->GetValue();
      theObject->SetStringParameter(index, valueString.c_str());
      break;
      
   case Gmat::OBJECT_TYPE:
   case Gmat::ENUMERATION_TYPE:
      valueString = ((wxComboBox*)theControl)->GetValue();
      theObject->SetStringParameter(index, valueString);
      break;
      
   case Gmat::OBJECTARRAY_TYPE:
      // For now use TextCtrl, use ListBoxCtrl later
      valueString = ((wxTextCtrl*)theControl)->GetValue();
      theObject->SetStringParameter(index, valueString.c_str());
      break;
      
   default:
      break;
   }
   
   #ifdef DEBUG_SAVE_CONTROL
   MessageInterface::ShowMessage("SaveControl() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// std::string AssignAcceleratorKey(std::string text)
//------------------------------------------------------------------------------
std::string GmatBaseSetupPanel::AssignAcceleratorKey(std::string text)
{
   // find best candidate
   // 1) Beginning of word
   // accelKeys contains list of already used accelerator keys (lower case)
   
   unsigned int accelIndex = 0;
   bool haveCandidate = false;
   bool findWord = true;  // true when flying over spaces in string
   unsigned int iText;
   std::string titleText = "";
   
   // first, check to see if accelerator has already been assigned (can happen if label
   // is loaded from INI file
   size_t found = text.find(GUI_ACCEL_KEY);
   if ( found != std::string::npos)
   {
      // add to accel keys
      accelKeys.push_back(tolower(text[found+1]));
      return text;
   }
   
   for (iText=0;iText<text.length();iText++)
   {
      // if character has not already been used
      if (find (accelKeys.begin(), accelKeys.end(), tolower(text[iText])) == accelKeys.end())
      {
         // if it is the beginning of a word alphanumeric
         if ((findWord) && (isalnum(text[iText])))
         {
            accelIndex = iText;
            accelKeys.push_back(tolower(text[iText]));
            if (accelIndex > 0)
               titleText.append(text, 0, accelIndex);
            titleText += GUI_ACCEL_KEY;
            titleText.append(text, accelIndex, text.length());
            return titleText;
         }
         // if in the middle of a word and we don't already have a candidate
         if ((!haveCandidate) && (isalnum(text[iText])))
         {
            accelIndex = iText;
            haveCandidate = true;
         }
      }
      if ((!findWord) && (!isalnum(text[iText])))
         findWord = true;
      if ((findWord) && (isalnum(text[iText])))
         findWord = false;
   }
   if (haveCandidate)
   {
      accelKeys.push_back(tolower(text[accelIndex]));
      titleText.append(text, 0, accelIndex);
      titleText += GUI_ACCEL_KEY;
      titleText.append(text, accelIndex, text.length());
   }
   return titleText;
}


//------------------------------------------------------------------------------
// bool GetLayoutConfig(wxConfigBase *config)
//------------------------------------------------------------------------------
/**
 * Creates the configuration object that provides layout data
 *
 */
//------------------------------------------------------------------------------
bool GmatBaseSetupPanel::GetLayoutConfig(GmatBase *theObject, wxFileConfig **config)
{
    std::string configPath;
    // get the GUI_CONFIG_PATH
    FileManager *fm = FileManager::Instance();
    try
    {
       configPath = fm->GetAbsPathname(FileManager::GUI_CONFIG_PATH);
    }
    catch (UtilityException &e)
    {
       MessageInterface::ShowMessage
          ("GmatBaseSetupPanel:Create() error occurred!\n%s\n",
           e.GetFullMessage().c_str());
       configPath = "";
    }
    
    static bool loadMessageWritten = false;
    std::string filename = configPath+theObject->GetTypeName()+".ini";
    bool configFileExists = GmatFileUtil::DoesFileExist(filename);
    
    if (!loadMessageWritten)
    {
       if (configFileExists)
          MessageInterface::ShowMessage
             ("GmatBaseSetupPanel:Attempting to load layout from file: %s\n",
              filename.c_str());
       else
          MessageInterface::ShowMessage
             ("GmatBaseSetupPanel:Unable to find layout file: %s\n",
              filename.c_str());
       loadMessageWritten = true;
    }
    *config = new wxFileConfig(wxEmptyString, wxEmptyString,
                               (filename).c_str(),
                               wxEmptyString, wxCONFIG_USE_LOCAL_FILE |
                               wxCONFIG_USE_RELATIVE_PATH );
    return configFileExists;
    
}


//------------------------------------------------------------------------------
// wxString GetParameterLabel(Integer i, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Creates a label for a parameter text.  It uses INI (if available) or
 * just a TitleCase string representation of the parameter text
 *
 * @param text input text
 */
//------------------------------------------------------------------------------
std::string GmatBaseSetupPanel::GetParameterLabel(GmatBase *theObject, Integer i,
                                                  wxFileConfig *config) const
{
   std::string text = theObject->GetParameterText(i);
   std::string titleText;
   wxString str;
   
   // first, see if the parameter is in the object's INI file
   // set the path to the section that contains the parameter's items
   config->SetPath(wxT(("/"+text).c_str()));
   if (config->Read(wxT("Label"), &str))
   {
      titleText = str.c_str();
      return titleText;
   }
   
   bool findword = false;
   if (text.length() > 0)
      titleText = text[0];
   for (unsigned int i=1;i<text.length();i++)
   {
      
      if (!findword)
      {
         // if not finding a word and is not uppercase, find the next word
         // e.g., NASATextFile -> NASAT (find word (next upper case char))
         //       Text File -> T (Find word (next upper case char))
         if (!((isupper(text[i])) || (isdigit(text[i]))))
            findword = true;
      }
      else
      {
         if ((isupper(text[i])) || (isdigit(text[i])))
         {
            findword = false;
            titleText += " ";
         }
      }
      titleText += text[i];
   }
   return titleText;
}


//------------------------------------------------------------------------------
// wxString GetParameterUnit(Integer i, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Creates a label for a parameter text.  It uses INI (if available) or
 * just a TitleCase string representation of the parameter text
 *
 * @param text input text
 */
//------------------------------------------------------------------------------
std::string GmatBaseSetupPanel::GetParameterUnit(GmatBase *theObject, Integer i,
                                                 wxFileConfig *config) const
{
   std::string text = theObject->GetParameterText(i);
   wxString str;
   
   // first, see if the parameter is in the object's INI file
   // set the path to the section that contains the parameter's items
   config->SetPath(wxT(("/"+text).c_str()));
   if (config->Read(wxT("Unit"), &str))
      return str.c_str();
   else
      return theObject->GetParameterUnit(i);
}


//------------------------------------------------------------------------------
// void OnBrowseButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for browse button click
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::OnBrowseButton(wxCommandEvent& event)
{
   wxBitmapButton *bb = (wxBitmapButton *) event.GetEventObject();
   wxTextCtrl *control = (wxTextCtrl *) bb->GetPrevSibling();
   wxString oldname = control->GetValue();
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;
      
      filename = dialog.GetPath().c_str();
      
      if (!filename.IsSameAs(oldname))
      {
         control->SetValue(filename);
         EnableUpdate(true);
      }
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for comboboxes
 *
 * Activates the Apply button when selection is changed.
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   // We don't want save every text change here so removed (LOJ: 2011.06.02)
   // The change will be save when an user hits apply button
   #if 0
   std::string label = localObject->GetParameterText(inverseControlMap[(wxControl *) event.GetEventObject()]);
   try
   {
      if (SaveControl(localObject, label))
         RefreshProperties(localObject);
   }
   catch (BaseException &)
   {
   }
   #endif
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnComboBoxTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for ComboBox text change
 *
 * Activates the Apply button when selection is changed.
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::OnComboBoxTextChange(wxCommandEvent& event)
{
   // We don't want save every text change here so removed (LOJ: 2011.06.02)
   // The change will be save when an user hits apply button
   #if 0
   std::string label = localObject->GetParameterText(inverseControlMap[(wxControl *) event.GetEventObject()]);
   try
   {
      if (SaveControl(localObject, label))
         RefreshProperties(localObject, label);
   }
   catch (BaseException &)
   {
   }
   #endif
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for text boxes
 *
 * Activates the Apply button when text is changed.
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::OnTextChange(wxCommandEvent& event)
{
   // We don't want save every text change here so removed (LOJ: 2011.06.02)
   // The change will be save when an user hits apply button
   #if 0
   std::string label = localObject->GetParameterText(inverseControlMap[(wxControl *) event.GetEventObject()]);
   try
   {
      if (SaveControl(localObject, label))
         RefreshProperties(localObject, label);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage
         ("GmatBaseSetupPanel::OnTextChange() %s\n", be.GetFullMessage().c_str());
   }
   #endif
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void SortProperties(StringArray *propertyNames, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Sort properties based on INI PositionBefore statements
 *
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::SortProperties(StringArray *propertyNames, wxFileConfig *config)
{
   // first, see if the INI file wants properties alphabetically sorted
   wxString alpha_sort;
   if (config->Read("/Main/Sort Properties", &alpha_sort))
   {
      if (alpha_sort.Lower() == "true")
         std::sort(propertyNames->begin(), propertyNames->end());
   }

   // Now, go through all the properties and order them according to "Position before"
   wxString position;
   StringArray::iterator item;
   StringArray::iterator beforeItem;
   StringArray origOrder = *propertyNames;

   for (unsigned int i = 0; i < origOrder.size(); i++)
   {
      // see if position defined
      if (config->Read(("/"+origOrder[i]+"/Position Before").c_str(), &position))
      {
         //MessageInterface::ShowMessage
         //   ("Sorting %s before %s\n", origOrder[i].c_str(), position.c_str());
         // find the property to move before
         if (position != "")
         {
            beforeItem = find(propertyNames->begin(), propertyNames->end(), position.c_str());
            if (beforeItem == propertyNames->end()) continue;
         }
         // find the property to move
         item = find(propertyNames->begin(), propertyNames->end(), origOrder[i].c_str());
         propertyNames->erase(item);
         // if no position before specified, move to the end
         if (position == "")
            propertyNames->push_back(origOrder[i]);
         else
         {
            // beforeItem is now wrong because of the erase...find it again
             beforeItem = find(propertyNames->begin(), propertyNames->end(), position.c_str());
             propertyNames->insert(beforeItem, origOrder[i]);
         }
      }
   }
}


//------------------------------------------------------------------------------
// void SortGroups(StringArray *groupNames, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Sort groups based on INI PositionBefore statements
 *
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::SortGroups(StringArray *groupNames, wxFileConfig *config)
{
   // go through all the groups and order them according to "Position before"
   wxString position;
   StringArray::iterator item;
   StringArray::iterator beforeItem;
   StringArray origOrder = *groupNames;

   for (unsigned int i = 0; i < origOrder.size(); i++)
   {
      // see if position defined
      if (config->Read(("/"+origOrder[i]+"/Position Before").c_str(), &position))
      {
         // find the group to move before
         if (position != "")
         {
            beforeItem = find(groupNames->begin(), groupNames->end(), position.Lower().c_str());
            if (beforeItem == groupNames->end()) continue;
         }
         // find the group to move
         item = find(groupNames->begin(), groupNames->end(), origOrder[i].c_str());
         groupNames->erase(item);
         // if no position before specified, move to the end
         if (position == "")
            groupNames->push_back(origOrder[i]);
         else
         {
            // beforeItem is now wrong because of the erase...find it again
             beforeItem = find(groupNames->begin(), groupNames->end(), position.Lower().c_str());
             groupNames->insert(beforeItem, origOrder[i]);
         }
      }
   }
}


//------------------------------------------------------------------------------
// void NormalizeLabels( StringArray propertyNames, std::vector<wxString> propertyGroups,
//                       std::vector<wxStaticText*> propertyDescriptors,
//                       std::vector<wxControl*> propertyControls,
//                       std::vector<wxStaticText*> propertyUnits )
//------------------------------------------------------------------------------
/**
 * Make all labels in each group the same size
 *
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::NormalizeLabels( StringArray propertyNames,
                      std::vector<wxString> propertyGroups,
                      std::vector<wxStaticText*> propertyDescriptors,
                      std::vector<wxControl*> propertyControls,
                      std::vector<wxControl*> propertyUnits )
{
   // initialize all groups with a max size
   SizerSizeType labelSizes, unitSizes;
   SizerSizeType::iterator sizeItem;
   std::vector<wxStaticText*>::iterator item;
   unsigned int j;

   // initialize the sizes first
   for (j = 0; j < propertyGroups.size(); j++)
   {
      if (labelSizes.find(propertyGroups[j].Lower()) == labelSizes.end())
      {
         labelSizes.insert( std::make_pair(propertyGroups[j].Lower(), 0) );
         unitSizes.insert( std::make_pair(propertyGroups[j].Lower(), 0) );
      }
   }

   // now find the minimums for the descriptions and units
   for(item = propertyDescriptors.begin(), j = 0;
       item != propertyDescriptors.end(); ++item, ++j)
   {
      // get the label size so far for the group
      sizeItem = labelSizes.find(propertyGroups[j].Lower());
      if (((int) sizeItem->second) < (*item)->GetBestSize().GetWidth())
         sizeItem->second = (*item)->GetBestSize().GetWidth();
      // get the unit size so far for the group
      sizeItem = unitSizes.find(propertyGroups[j].Lower());
      if (((int) sizeItem->second) < propertyUnits[j]->GetBestSize().GetWidth())
         sizeItem->second = propertyUnits[j]->GetBestSize().GetWidth();
   }

   // adjust grouplabels and groupunits
   for(item = propertyDescriptors.begin(), j = 0;
       item != propertyDescriptors.end(); ++item, ++j)
   {
      // set the label min size
      sizeItem = labelSizes.find(propertyGroups[j].Lower());
      (*item)->SetMinSize(wxSize(((int) sizeItem->second), (*item)->GetMinHeight()));
      // set the unit min size
      sizeItem = unitSizes.find(propertyGroups[j].Lower());
      propertyUnits[j]->SetMinSize(wxSize(((int) sizeItem->second), propertyUnits[j]->GetMinHeight()));
   }
}

//------------------------------------------------------------------------------
// void FixTabOrder( wxWindow *lastControl, wxSizer *sizer )
//------------------------------------------------------------------------------
/**
 * Fixes the tab order so that tab button follows the visual order of the controls
 *
 *
 * @param lastControl the control that is BEFORE in the tab order for every control
 *                   in the sizer.  lastControl is NULL if no control is before any
 *                   of these controls
 * @param sizer the sizer that contains the controls to fix tab order for
 */
//------------------------------------------------------------------------------
wxWindow *GmatBaseSetupPanel::FixTabOrder( wxWindow *lastControl, wxSizer *sizer )
{
   wxSizerItemList list = sizer->GetChildren();
   wxSizerItemList::iterator iter;
   for (iter = list.begin(); iter != list.end(); ++iter)
   {
      if ((*iter)->IsSizer())
         lastControl = FixTabOrder( lastControl, (*iter)->GetSizer() );
      else
      {
         if (lastControl != NULL)
            (*iter)->GetWindow()->MoveAfterInTabOrder(lastControl);
         lastControl = (*iter)->GetWindow();
      }
   }
   return lastControl;

}


//------------------------------------------------------------------------------
// RefreshProperties(GmatBase *theObject, std::string ignoreControl)
//------------------------------------------------------------------------------
/**
 * Refreshes the Parameters/Properties controls (label, control, and unit) by
 * re-inspecting the object
 *
 *
 * @param theObject the object to re-inspect
 * @param ignoreControl if not "", then ignore the controls for that parameter (user
 *                      is probably currently typing in them
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::RefreshProperties(GmatBase *theObject, std::string ignoreControl)
{
   #ifdef DEBUG_BASEPANEL_CREATE
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::RefreshProperties() entered. Ignore=%s\n", ignoreControl.c_str());
   #endif

   wxFileConfig *pConfig;
   GetLayoutConfig( theObject, &pConfig );
   // iterate through the map
   for (std::map<Integer, wxControl *>::iterator i = controlMap.begin();
        i != controlMap.end(); ++i)
   {
      if ((ignoreControl == "") || (theObject->GetParameterText(i->first) != ignoreControl))
         RefreshProperty(theObject, i->first, i->second, pConfig);
   }
}


//------------------------------------------------------------------------------
// RefreshProperty(GmatBase *theObject, Integer index, wxControl *control, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Refreshes the Parameter/Property controls (label, control, and unit) for one parameter
 *
 *
 * @param theObject the object to re-inspect
 * @param index the index of the parameter in the object
 * @param control the edit control to refresh
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::RefreshProperty(GmatBase *theObject, Integer index, wxControl *control,
                                         wxFileConfig *config)
{
    wxControl *aSibling;

    // refresh the label, the value, and the unit
    std::string labelText = theObject->GetParameterText(index);

    // set the path to the section that contains the parameter's items
    config->SetPath(wxT(("/"+labelText).c_str()));

    // refresh value
    LoadControl(theObject, labelText);
    control->Enable(theObject->IsParameterEnabled(index));

    // refresh label
    if (theObject->GetParameterType(index) != Gmat::BOOLEAN_TYPE)
        aSibling = (wxControl *) control->GetPrevSibling();
    else
        aSibling = control;

    labelText = GetParameterLabel(theObject, index, config);
    if ((aSibling->GetLabelText()).c_str() != labelText)
    {
        labelText = AssignAcceleratorKey(labelText);
        aSibling->SetLabel(labelText.c_str());
    }

    // refresh units
    control->GetNextSibling()->SetLabel(wxT(GetParameterUnit(theObject, index, config).c_str()));
}

