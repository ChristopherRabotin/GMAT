//$Header$
//------------------------------------------------------------------------------
//                              ResourceTree
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2003/08/29
//
/**
 * This class provides the resource tree and event handlers.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "bitmaps/folder.xpm"
#include "bitmaps/openfolder.xpm"
#include "bitmaps/file.xpm"
#include "bitmaps/spacecraft.xpm"
#include "bitmaps/earth.xpm"

#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "ResourceTree.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ResourceTree, wxTreeCtrl)
   EVT_TREE_ITEM_RIGHT_CLICK(-1, ResourceTree::OnItemRightClick)

   // had to change back to double click because of a segmentation
   // fault
   EVT_TREE_ITEM_ACTIVATED(-1, ResourceTree::OnItemActivated)
   EVT_TREE_BEGIN_LABEL_EDIT(-1, ResourceTree::OnBeginLabelEdit)
   EVT_TREE_END_LABEL_EDIT(-1, ResourceTree::OnEndLabelEdit)
   
   EVT_MENU(POPUP_ADD_SC, ResourceTree::OnAddSc)
   EVT_MENU(POPUP_ADD_FORMATION, ResourceTree::OnAddFormation)
   EVT_MENU(POPUP_ADD_CONSTELLATION, ResourceTree::OnAddConstellation)
   EVT_MENU(POPUP_ADD_PROPAGATOR, ResourceTree::OnAddPropagator)
   EVT_MENU(POPUP_ADD_BODY, ResourceTree::OnAddBody)
   EVT_MENU(POPUP_OPEN, ResourceTree::OnOpen)
   EVT_MENU(POPUP_CLOSE, ResourceTree::OnClose)
   EVT_MENU(POPUP_RENAME, ResourceTree::OnRename)
   EVT_MENU(POPUP_DELETE, ResourceTree::OnDelete)
   
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ResourceTree(wxFrame *parent)
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
ResourceTree::ResourceTree(wxWindow *parent, const wxWindowID id,
              const wxPoint &pos, const wxSize &size, long style)
              :wxTreeCtrl(parent, id, pos, size, style)
{
    this->parent = parent;
    theGuiInterpreter = GmatAppData::GetGuiInterpreter();
    AddIcons();
    AddDefaultResources();
}

//------------------------------------------------------------------------------
// void UpdateResources()
//------------------------------------------------------------------------------
void ResourceTree::UpdateResources()
{
    MessageInterface::ShowMessage(" In ResourceTree::UpdateResources()\n");

    // update spacecraft
    StringArray scNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
    int size = scNames.size();

    this->DeleteChildren(mSpacecraftItem);
    
    for (int i = 0; i<size; i++)
    {
        wxString scname = wxString(scNames[i].c_str());
        MessageInterface::ShowMessage(" SC name: " + scNames[i] + "\n");
        this->AppendItem(mSpacecraftItem, wxT(scname), ICON_SPACECRAFT, -1,
                         new GmatTreeItemData(wxT(scname), DEFAULT_SPACECRAFT));
    };

    // update other tree items
}

//------------------------------------------------------------------------------
//  void AddDefaultResources()
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultResources()
{
    wxTreeItemId resource = this->AddRoot(wxT("Resources"), -1, -1,
                                  new GmatTreeItemData(wxT("Resources"),
                                  RESOURCES_FOLDER));

//      wxTreeItemId spacecraftItem = this->AppendItem(resource, wxT("Spacecraft"),
//                                    -1, -1, new GmatTreeItemData(wxT("Spacecraft"),
//                                    SPACECRAFT_FOLDER));
//      SetItemImage(spacecraftItem, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
    
    mSpacecraftItem =
        this->AppendItem(resource, wxT("Spacecraft"),
                         -1, -1, new GmatTreeItemData(wxT("Spacecraft"),
                                                      SPACECRAFT_FOLDER));
    
    SetItemImage(mSpacecraftItem, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
    
    wxTreeItemId formationItem = this->AppendItem(resource, wxT("Formations"),
                                 -1, -1, new GmatTreeItemData(wxT("Formations"),
                                 FORMATIONS_FOLDER));
    SetItemImage(formationItem, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

    wxTreeItemId constellationItem = this->AppendItem(resource,
                                     wxT("Constellations"), -1, -1,
                                     new GmatTreeItemData(wxT("Constellations"),
                                     CONSTELLATIONS_FOLDER));
    SetItemImage(constellationItem, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

    wxTreeItemId propagatorItem = this->AppendItem(resource,
                     wxT("Propagators"), -1, -1,
                     new GmatTreeItemData(wxT("Propagators"),
                     PROPAGATORS_FOLDER));
    SetItemImage(propagatorItem, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);
    
    wxTreeItemId universeItem = this->AppendItem(resource, wxT("Universe"), -1, -1,
                     new GmatTreeItemData(wxT("Universe"), UNIVERSE_FOLDER));
    SetItemImage(universeItem, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

    wxTreeItemId solverItem = this->AppendItem(resource, wxT("Solvers"),
                     -1, -1, new GmatTreeItemData(wxT("Solvers"), SOLVERS_FOLDER));
    SetItemImage(solverItem, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

    wxTreeItemId plotItem = this->AppendItem(resource, wxT("Plots"),
                     -1, -1, new GmatTreeItemData(wxT("Plots"), PLOTS_FOLDER));
    SetItemImage(plotItem, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

    wxTreeItemId interfaceItem = this->AppendItem(resource, wxT("Interfaces"), -1, -1,
                     new GmatTreeItemData(wxT("Interfaces"), INTERFACES_FOLDER));
    SetItemImage(interfaceItem, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

    this->AppendItem(resource, wxT("Reports"), -1, -1,
                     new GmatTreeItemData(wxT("Reports"), REPORTS_FOLDER));
    this->AppendItem(resource, wxT("Subscripts"), -1, -1,
                     new GmatTreeItemData(wxT("Subscripts"), SUBSCRIPTS_FOLDER));
    this->AppendItem(resource, wxT("Variables"), -1, -1,
                     new GmatTreeItemData(wxT("Variables"), VARIABLES_FOLDER));
    this->AppendItem(resource, wxT("Ground Stations"), -1, -1,
                     new GmatTreeItemData(wxT("Ground Stations"), GROUNDSTATIONS_FOLDER));

    AddDefaultBodies(universeItem);
//      AddDefaultSpacecraft(spacecraftItem);
    AddDefaultSpacecraft(mSpacecraftItem);
    AddDefaultFormations(formationItem);
    AddDefaultConstellations(constellationItem);
    AddDefaultPropagators(propagatorItem);
    AddDefaultSolvers(solverItem);
    AddDefaultPlots(plotItem);
    AddDefaultInterfaces(interfaceItem);
}

//------------------------------------------------------------------------------
// void AddDefaultBodies(wxTreeItemId universe)
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultBodies(wxTreeItemId universe)
{
    this->AppendItem(universe, wxT("Sun"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Sun"), DEFAULT_BODY));
    this->AppendItem(universe, wxT("Mercury"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Mercury"), DEFAULT_BODY));
    this->AppendItem(universe, wxT("Venus"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Venus"), DEFAULT_BODY));

    wxTreeItemId earth = this->AppendItem(universe, wxT("Earth"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Earth"), DEFAULT_BODY));
    this->AppendItem(earth, wxT("Moon"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Moon"), DEFAULT_BODY));

    this->AppendItem(universe, wxT("Mars"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Mars"), DEFAULT_BODY));
    this->AppendItem(universe, wxT("Jupiter"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Jupiter"), DEFAULT_BODY));
    this->AppendItem(universe, wxT("Saturn"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Saturn"), DEFAULT_BODY));
    this->AppendItem(universe, wxT("Uranus"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Uranus"), DEFAULT_BODY));
    this->AppendItem(universe, wxT("Neptune"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Neptune"), DEFAULT_BODY));
    this->AppendItem(universe, wxT("Pluto"), ICON_EARTH, -1,
                     new GmatTreeItemData(wxT("Pluto"), DEFAULT_BODY));
}

//------------------------------------------------------------------------------
// void AddDefaultSpacecraft(wxTreeItemId spacecraft)
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSpacecraft(wxTreeItemId spacecraft)
{
    MessageInterface::ShowMessage(" In ResourceTree::AddDefaultSpacecraft()\n");
    StringArray scNames = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
    int size = scNames.size();
    for (int i = 0; i<size; i++)
    {
        wxString scname = wxString(scNames[i].c_str());
        MessageInterface::ShowMessage(" SC name: " + scNames[i] + "\n");
        this->AppendItem(spacecraft, wxT(scname), ICON_SPACECRAFT, -1,
                         new GmatTreeItemData(wxT(scname), DEFAULT_SPACECRAFT));
    };

    this->AppendItem(spacecraft, wxT("TempSat1"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("TempSat1"), DEFAULT_SPACECRAFT));
    this->AppendItem(spacecraft, wxT("TempSat2"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("TempSat2"), DEFAULT_SPACECRAFT));
}


//------------------------------------------------------------------------------
// void AddDefaultFormations(wxTreeItemId formation)
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultFormations(wxTreeItemId formation)
{
    wxTreeItemId mms = this->AppendItem(formation, wxT("MMS"), -1, -1,
                             new GmatTreeItemData(wxT("MMS"), DEFAULT_FORMATION_FOLDER));
    SetItemImage(mms, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

    this->AppendItem(mms, wxT("MMS1"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("MMS1"), DEFAULT_FORMATION_SPACECRAFT));
    this->AppendItem(mms, wxT("MMS2"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("MMS2"), DEFAULT_FORMATION_SPACECRAFT));
    this->AppendItem(mms, wxT("MMS3"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("MMS3"), DEFAULT_FORMATION_SPACECRAFT));
    this->AppendItem(mms, wxT("MMS4"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("MMS4"), DEFAULT_FORMATION_SPACECRAFT));
}

//------------------------------------------------------------------------------
// void AddDefaultConstellations(wxTreeItemId constellation)
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultConstellations(wxTreeItemId constellation)
{
    wxTreeItemId gps = this->AppendItem(constellation, wxT("GPS"), -1, -1,
                             new GmatTreeItemData(wxT("GPS"), DEFAULT_CONSTELLATION_FOLDER));
    SetItemImage(gps, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

    this->AppendItem(gps, wxT("GPS1"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("GPS1"), DEFAULT_CONSTELLATION_SATELLITE));
    this->AppendItem(gps, wxT("GPS2"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("GPS2"), DEFAULT_CONSTELLATION_SATELLITE));
    this->AppendItem(gps, wxT("GPS3"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("GPS3"), DEFAULT_CONSTELLATION_SATELLITE));
    this->AppendItem(gps, wxT("GPS4"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("GPS4"), DEFAULT_CONSTELLATION_SATELLITE));
}

//------------------------------------------------------------------------------
// void AddDefaultPropagators(wxTreeItemId propagator)
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultPropagators(wxTreeItemId propagator)
{
    this->AppendItem(propagator, wxT("RKV89"), -1, -1,
          new GmatTreeItemData(wxT("RKV89"), DEFAULT_PROPAGATOR));
    this->AppendItem(propagator, wxT("RKV45"), -1, -1,
          new GmatTreeItemData(wxT("RKV45"), DEFAULT_PROPAGATOR));
    this->AppendItem(propagator, wxT("Cowell"), -1, -1,
          new GmatTreeItemData(wxT("Cowell"), DEFAULT_PROPAGATOR));
}

//------------------------------------------------------------------------------
// void AddDefaultSolvers(wxTreeItemId solver)
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSolvers(wxTreeItemId solver)
{
    this->AppendItem(solver, wxT("DC"), -1, -1,
          new GmatTreeItemData(wxT("DC"), DEFAULT_SOLVER));
    this->AppendItem(solver, wxT("Optimizers"), -1, -1,
          new GmatTreeItemData(wxT("Optimizers"), DEFAULT_SOLVER));
    this->AppendItem(solver, wxT("Monte Carlo"), -1, -1,
          new GmatTreeItemData(wxT("Monte Carlo"), DEFAULT_SOLVER));
}

//------------------------------------------------------------------------------
// void AddDefaultPlots(wxTreeItemId plot)
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultPlots(wxTreeItemId plot)
{
    this->AppendItem(plot, wxT("Polar"), -1, -1,
          new GmatTreeItemData(wxT("Polar"), DEFAULT_PLOT));
    this->AppendItem(plot, wxT("3D"), -1, -1,
          new GmatTreeItemData(wxT("3D"), DEFAULT_PLOT));
    this->AppendItem(plot, wxT("Ground Track"), -1, -1,
          new GmatTreeItemData(wxT("Ground Track"), DEFAULT_PLOT));
    this->AppendItem(plot, wxT("XY"), -1, -1,
          new GmatTreeItemData(wxT("XY"), DEFAULT_PLOT));
}

//------------------------------------------------------------------------------
// void AddDefaultInterfaces(wxTreeItemId interfaceTree)
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultInterfaces(wxTreeItemId interfaceTree)
{
    this->AppendItem(interfaceTree, wxT("TCP/IP"), -1, -1,
          new GmatTreeItemData(wxT("TCP/IP"), DEFAULT_INTERFACE));
    this->AppendItem(interfaceTree, wxT("Mex"), -1, -1,
          new GmatTreeItemData(wxT("Mex"), DEFAULT_INTERFACE));
}

//------------------------------------------------------------------------------
// void OnItemRightClick(wxTreeEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnItemRightClick(wxTreeEvent& event)
{
    ShowMenu(event.GetItem(), event.GetPoint());
}

//------------------------------------------------------------------------------
// void ShowMenu(wxTreeItemId id, const wxPoint& pt)
//------------------------------------------------------------------------------
void ResourceTree::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
    GmatTreeItemData *treeItem = (GmatTreeItemData *)GetItemData(id);
    wxString title = treeItem->GetDesc();
    int dataType = treeItem->GetDataType();
    
#if wxUSE_MENUS
    wxMenu menu;
    
    if (strcmp(title, wxT("Spacecraft")) == 0)
      menu.Append(POPUP_ADD_SC, wxT("Add Spacecraft..."));
    else if (strcmp(title, wxT("Formations")) == 0)
      menu.Append(POPUP_ADD_FORMATION, wxT("Add Formation..."));
    else if (strcmp(title, wxT("Constellations")) == 0)
      menu.Append(POPUP_ADD_CONSTELLATION, wxT("Add Constellation..."));
    else if (strcmp(title, wxT("Propagators")) == 0)
      menu.Append(POPUP_ADD_PROPAGATOR, wxT("Add Propagator..."));
    else if (strcmp(title, wxT("Universe")) == 0)
      menu.Append(POPUP_ADD_BODY, wxT("Add Body..."));
    else if ((dataType == DEFAULT_FORMATION_FOLDER)     ||
             (dataType == CREATED_FORMATION_FOLDER)     ||
             (dataType == DEFAULT_CONSTELLATION_FOLDER) ||
             (dataType == CREATED_CONSTELLATION_FOLDER))
    {
      menu.Append(POPUP_ADD_SC, wxT("Add Spacecraft..."));
      menu.AppendSeparator();
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      menu.Append(POPUP_RENAME, wxT("Rename"));
      menu.Append(POPUP_DELETE, wxT("Delete"));
    }
    else if (dataType == DEFAULT_PROPAGATOR)
    {
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
    }
    else
    {
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      menu.Append(POPUP_RENAME, wxT("Rename"));
      menu.Append(POPUP_DELETE, wxT("Delete"));
    }

    PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}

//------------------------------------------------------------------------------
// void OnItemActivated(wxTreeEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnItemActivated(wxTreeEvent &event)
{
    // get some info about this item
    wxTreeItemId itemId = event.GetItem();
    GmatTreeItemData *item = (GmatTreeItemData *)GetItemData(itemId);

    mainNotebook->CreatePage(item);
}

//------------------------------------------------------------------------------
// void SetMainNotebook (GmatMainNotebook *mainNotebook)
//------------------------------------------------------------------------------
void ResourceTree::SetMainNotebook (GmatMainNotebook *mainNotebook)
{
  this->mainNotebook = mainNotebook;
}

//------------------------------------------------------------------------------
// GmatMainNotebook *GetMainNotebook()
//------------------------------------------------------------------------------
GmatMainNotebook *ResourceTree::GetMainNotebook()
{
   return this->mainNotebook;
}

//------------------------------------------------------------------------------
// void OnOpen(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnOpen(wxCommandEvent &event)
{
 // Get info from selected item
 GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
 mainNotebook->CreatePage(item);
}

//------------------------------------------------------------------------------
// void OnClose(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnClose(wxCommandEvent &event)
{
 // make item most current, then close it
 GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
 mainNotebook->CreatePage(item);
 mainNotebook->ClosePage();
}

//------------------------------------------------------------------------------
// void OnAddSc(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddSc(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  this->AppendItem(item, wxT("New Spacecraft"), ICON_SPACECRAFT, -1,
              new GmatTreeItemData(wxT("New Spacecraft"), CREATED_SPACECRAFT));
}


//------------------------------------------------------------------------------
// void OnAddFormation(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddFormation(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  wxTreeItemId formation = this->AppendItem(item, wxT("New Formation"),
                           -1, -1, new GmatTreeItemData(wxT
                           ("New Formation"), CREATED_FORMATION_FOLDER));

  SetItemImage(formation, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

  this->AppendItem(formation, wxT("MMS1"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("MMS1"), CREATED_FORMATION_SPACECRAFT));
  this->AppendItem(formation, wxT("MMS2"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("MMS2"), CREATED_FORMATION_SPACECRAFT));
}

//------------------------------------------------------------------------------
// void OnAddConstellation(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddConstellation(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  wxTreeItemId constellation = this->AppendItem(item, wxT("New Constellation"),
                               -1, -1, new GmatTreeItemData(wxT
                              ("New Constellation"), CREATED_CONSTELLATION_FOLDER));

  SetItemImage(constellation, ICON_OPENFOLDER, wxTreeItemIcon_Expanded);

  this->AppendItem(constellation, wxT("GPS1"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("GPS1"), CREATED_CONSTELLATION_SATELLITE));
  this->AppendItem(constellation, wxT("GPS2"), ICON_SPACECRAFT, -1,
                     new GmatTreeItemData(wxT("GPS2"), CREATED_CONSTELLATION_SATELLITE));
}

//------------------------------------------------------------------------------
// void OnAddPropagator(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddPropagator(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  this->AppendItem(item, wxT("New Propagator"), -1, -1,
        new GmatTreeItemData(wxT("New Propagator"), CREATED_PROPAGATOR));

}

//------------------------------------------------------------------------------
// void OnAddBody(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddBody(wxCommandEvent &event)
{
  wxTreeItemId item = GetSelection();
  this->AppendItem(item, wxT("New Body"), ICON_EARTH, -1,
        new GmatTreeItemData(wxT("New Body"), CREATED_BODY));
}

//------------------------------------------------------------------------------
// void OnRename(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnRename(wxCommandEvent &event)
{
//  static wxString sText;
//  sText = wxGetTextFromUser(wxT("New name: "), wxT("Input Text"),
//                            sText, this);
//  if ( !sText.IsEmpty())
//  {
//    SetItemText(item, sText);
//    GmatTreeItemData *selItem = (GmatTreeItemData *) GetItemData(item);
//    selItem->SetDesc(sText);
//  }
  (void) this->EditLabel(GetSelection());
}

//------------------------------------------------------------------------------
// void OnDelete(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnDelete(wxCommandEvent &event)
{
  event.Skip();
}

//------------------------------------------------------------------------------
// void OnBeginLabelEdit(wxTreeEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnBeginLabelEdit(wxTreeEvent &event)
{
  
  GmatTreeItemData *selItem = (GmatTreeItemData *)
                               GetItemData(event.GetItem());
                               
  int dataType = selItem->GetDataType();
  bool isDefaultFolder = ((dataType == RESOURCES_FOLDER)     ||
                         (dataType == SPACECRAFT_FOLDER)     ||
                         (dataType == FORMATIONS_FOLDER)     ||
                         (dataType == CONSTELLATIONS_FOLDER) ||
                         (dataType == PROPAGATORS_FOLDER)    ||
                         (dataType == UNIVERSE_FOLDER)       ||
                         (dataType == SOLVERS_FOLDER)        ||
                         (dataType == PLOTS_FOLDER)          ||
                         (dataType == INTERFACES_FOLDER)     ||
                         (dataType == REPORTS_FOLDER)        ||
                         (dataType == SUBSCRIPTS_FOLDER)     ||
                         (dataType == VARIABLES_FOLDER)      ||
                         (dataType == GROUNDSTATIONS_FOLDER));
                         
  bool isDefaultItem = ((dataType == DEFAULT_PROPAGATOR) ||
                        (dataType == DEFAULT_BODY)       ||
                        (dataType == DEFAULT_SOLVER)     ||
                        (dataType == DEFAULT_PLOT)       ||
                        (dataType == DEFAULT_INTERFACE)  ||
                        (dataType == DEFAULT_REPORT));

  //kind of redundant because OpenPage returns false for some
  //of the default folders
  if ( (!mainNotebook->OpenPage(selItem)) ||
       (isDefaultFolder)                  ||
       (isDefaultItem))
  {
     event.Veto();
  }
}

//------------------------------------------------------------------------------
// void OnEndLabelEdit(wxTreeEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnEndLabelEdit(wxTreeEvent &event)
{
  wxString label = event.GetLabel();
  
  // check to see if label is a single word
  if (label.IsWord())
  {
     GmatTreeItemData *selItem = (GmatTreeItemData *)
                                 GetItemData(GetSelection());
     selItem->SetDesc(label);
  }
  else
  {
    event.Veto();
  }
}

//------------------------------------------------------------------------------
// void AddIcons()
//------------------------------------------------------------------------------
void ResourceTree::AddIcons()
{
  int size = 16;
  
  wxImageList *images = new wxImageList ( size, size, true );
  
  wxBusyCursor wait;
  wxIcon icons[5];

  icons[0] = wxIcon ( folder_xpm );
  icons[1] = wxIcon ( file_xpm );
  icons[2] = wxIcon ( openfolder_xpm );
  icons[3] = wxIcon ( spacecrapt_xpm );
  icons[4] = wxIcon ( earth_xpm );
  
  int sizeOrig = icons[0].GetWidth();
    for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
    {
        if ( size == sizeOrig )
        {
            images->Add(icons[i]);
        }
        else
        {
            images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(size, size)));
        }
    }

    AssignImageList(images);

}

