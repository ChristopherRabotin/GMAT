//$Header$
//------------------------------------------------------------------------------
//                              GmatAppData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/10/29
//
/**
 * defines application data.
 */
//------------------------------------------------------------------------------
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"

GuiInterpreter* GmatAppData::theGuiInterpreter = NULL;

#if !defined __CONSOLE_APP__
ViewTextFrame* GmatAppData::theMessageWindow = NULL;
ViewTextFrame* GmatAppData::theCompareWindow = NULL;
wxTextCtrl* GmatAppData::theMessageTextCtrl = NULL;
ResourceTree* GmatAppData::theResourceTree = NULL;
MissionTree* GmatAppData::theMissionTree = NULL;
OutputTree* GmatAppData::theOutputTree = NULL;
//GmatMainNotebook *GmatAppData::theMainNotebook = NULL;
GmatMainFrame *GmatAppData::theMainFrame = NULL;
wxFont GmatAppData::theFont = wxFont(10, wxMODERN, wxNORMAL, wxNORMAL);

#endif
