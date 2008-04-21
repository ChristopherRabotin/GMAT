//$Id$
//------------------------------------------------------------------------------
//                              SetPathDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2006/03/29
//
/**
 * Shows dialog for setting path for various files used in the system.
 */
//------------------------------------------------------------------------------

#include "SetPathDialog.hpp"
#include "MultiPathSetupPanel.hpp"
#include "SinglePathSetupPanel.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

#include <wx/dir.h>           // for wxDir::Exists()

//#define DEBUG_SETPATH_DIALOG

//------------------------------------------------------------------------------
// SetPathDialog(wxWindow *parent)
//------------------------------------------------------------------------------
SetPathDialog::SetPathDialog(wxWindow *parent)
   : GmatDialog(parent, -1, "SetPathDialog")
{
   Create();
   ShowData();
}


//------------------------------------------------------------------------------
// ~SetPathDialog()
//------------------------------------------------------------------------------
SetPathDialog::~SetPathDialog()
{
}


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void SetPathDialog::Create()
{
   #ifdef DEBUG_SETPATH_DIALOG
   MessageInterface::ShowMessage("SetPathDialog::Create() entered.\n");
   #endif
   
   FileManager *fm = FileManager::Instance();
   
   // create Notebook
   mPathNotebook =
      new wxNotebook(this, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxGROW);
   
   StringArray gmatPaths = fm->GetAllGmatFunctionPaths();
   
   // add panels to notebook   
   mGmatFunPathPanel = new MultiPathSetupPanel(mPathNotebook, gmatPaths);
   mPathNotebook->AddPage(mGmatFunPathPanel, "GMAT Function");
   
   StringArray matlabPaths = fm->GetAllMatlabFunctionPaths();
   mMatlabPathPanel = new MultiPathSetupPanel(mPathNotebook, matlabPaths);
   mPathNotebook->AddPage(mMatlabPathPanel, "MATLAB Function");
   
   std::string outputPath = fm->GetFullPathname("OUTPUT_PATH");
   
   #ifdef DEBUG_SETPATH_DIALOG
   MessageInterface::ShowMessage("   outputPath='%s'\n", outputPath.c_str());
   #endif
   
   mOutputPathPanel = new SinglePathSetupPanel(mPathNotebook, outputPath.c_str());
   mPathNotebook->AddPage(mOutputPathPanel, "Output");
   
   theMiddleSizer->Add(mPathNotebook, 1, wxALIGN_CENTER|wxGROW|wxALL, 5);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void SetPathDialog::LoadData()
{
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void SetPathDialog::SaveData()
{
   #ifdef DEBUG_SETPATH_DIALOG
   MessageInterface::ShowMessage("SetPathDialog::SaveData() entered.\n");
   #endif

   canClose = true;
   
   FileManager *fm = FileManager::Instance();
   wxArrayString pathNames;
   
   // Gmat Function paths
   if (mGmatFunPathPanel->HasDataChanged())
   {
      #ifdef DEBUG_SETPATH_DIALOG
      MessageInterface::ShowMessage("   Saving GmatFunction paths...\n");
      #endif
      
      pathNames = mGmatFunPathPanel->GetPathNames();
      
      #ifdef DEBUG_SETPATH_DIALOG
      MessageInterface::ShowMessage("   ...Adding %d paths\n", pathNames.GetCount());
      #endif
      
      fm->ClearGmatFunctionPath();
      for (UnsignedInt i=0; i<pathNames.GetCount(); i++)
      {
         #ifdef DEBUG_SETPATH_DIALOG
         MessageInterface::ShowMessage("   ...Adding '%s'\n", pathNames[i].c_str());
         #endif
         
         fm->AddGmatFunctionPath(pathNames[i].c_str());
      }
   }
   
   // Matlab Function paths
   if (mMatlabPathPanel->HasDataChanged())
   {
      #ifdef DEBUG_SETPATH_DIALOG
      MessageInterface::ShowMessage("   Saving MatlabFunction paths...\n");
      #endif
      
      pathNames = mMatlabPathPanel->GetPathNames();
      fm->ClearMatlabFunctionPath();
      for (UnsignedInt i=0; i<pathNames.GetCount(); i++)
         fm->AddMatlabFunctionPath(pathNames[i].c_str());
   }
   
   // Log file path
   if (mOutputPathPanel->HasDataChanged())
   {
      std::string pathName = mOutputPathPanel->GetFullPathName().c_str();
      if (wxDir::Exists(pathName))
      {
         #ifdef DEBUG_SETPATH_DIALOG
         MessageInterface::ShowMessage("   Saving Log path to '%s'\n", pathName.c_str());
         #endif
         
         fm->SetAbsPathname("OUTPUT_PATH", pathName);
         MessageInterface::SetLogPath(pathName);
      }
      else
      {
         wxMessageBox(wxT("The directory \"" + pathName + "\" does not exist.\n"),
                      wxT("Directory Error"));
         
         // find page
         int numPage = mPathNotebook->GetPageCount();
         for (int i=0; i<numPage; i++)
         {
            if (mPathNotebook->GetPageText(i) == "Output")
            {
               mPathNotebook->SetSelection(i);
               break;
            }
         }
         canClose = false;
      }
   }
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void SetPathDialog::ResetData()
{
   canClose = true;
}


