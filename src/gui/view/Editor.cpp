//$Id$
//------------------------------------------------------------------------------
//                                     Editor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2009/01/05
//
/**
 * Implements Editor class.
 */
//------------------------------------------------------------------------------

#include "Editor.hpp"
#include "GmatSavePanel.hpp"
#include "FunctionSetupPanel.hpp"
#include "EditorPreferences.hpp"
#include "FindReplaceDialog.hpp"
#include "MessageInterface.hpp"
#include <wx/filename.h>           // for wxFileName
#include <wx/numdlg.h>             // for wxGetNumberFromUser

//#define DEBUG_EDITOR
//#define DEBUG_EDITOR_FIND
//#define DEBUG_EDITOR_PREF


BEGIN_EVENT_TABLE (Editor, wxStyledTextCtrl)
   // common
   EVT_SIZE (                         Editor::OnSize)
   // edit
   EVT_MENU (wxID_CLEAR,              Editor::OnClear)
   EVT_MENU (wxID_CUT,                Editor::OnCut)
   EVT_MENU (wxID_COPY,               Editor::OnCopy)
   EVT_MENU (wxID_PASTE,              Editor::OnPaste)
   EVT_MENU (STC_ID_INDENTMORE,       Editor::OnIndentMore)
   EVT_MENU (STC_ID_INDENTLESS,       Editor::OnIndentLess)
   EVT_MENU (wxID_SELECTALL,          Editor::OnSelectAll)
   EVT_MENU (STC_ID_SELECTLINE,       Editor::OnSelectLine)
   EVT_MENU (wxID_REDO,               Editor::OnRedo)
   EVT_MENU (wxID_UNDO,               Editor::OnUndo)
   // find, replace, goto
   EVT_MENU (wxID_FIND,               Editor::OnFind)
   EVT_MENU (STC_ID_FINDNEXT,         Editor::OnFindNext)
   EVT_MENU (STC_ID_REPLACENEXT,      Editor::OnReplaceNext)
   EVT_MENU (STC_ID_REPLACEALL,       Editor::OnReplaceAll)
   EVT_MENU (STC_ID_BRACEMATCH,       Editor::OnBraceMatch)
   EVT_MENU (STC_ID_GOTO,             Editor::OnGoToLine)
   // view
   EVT_MENU_RANGE (STC_ID_HILIGHTFIRST, STC_ID_HILIGHTLAST,
                                      Editor::OnHilightLang)
   EVT_MENU (STC_ID_DISPLAYEOL,       Editor::OnDisplayEOL)
   EVT_MENU (STC_ID_INDENTGUIDE,      Editor::OnIndentGuide)
   EVT_MENU (STC_ID_LINENUMBER,       Editor::OnLineNumber)
   EVT_MENU (STC_ID_LONGLINEON,       Editor::OnLongLineOn)
   EVT_MENU (STC_ID_WHITESPACE,       Editor::OnWhiteSpace)
   EVT_MENU (STC_ID_FOLDTOGGLE,       Editor::OnFoldToggle)
   EVT_MENU (STC_ID_OVERTYPE,         Editor::OnSetOverType)
   EVT_MENU (STC_ID_READONLY,         Editor::OnSetReadOnly)
   EVT_MENU (STC_ID_WRAPMODEON,       Editor::OnWrapmodeOn)
   EVT_MENU (STC_ID_CHARSETANSI,      Editor::OnUseCharset)
   EVT_MENU (STC_ID_CHARSETMAC,       Editor::OnUseCharset)
   // extra
   EVT_MENU (STC_ID_CHANGELOWER,      Editor::OnChangeCase)
   EVT_MENU (STC_ID_CHANGEUPPER,      Editor::OnChangeCase)
   EVT_MENU (STC_ID_CONVERTCR,        Editor::OnConvertEOL)
   EVT_MENU (STC_ID_CONVERTCRLF,      Editor::OnConvertEOL)
   EVT_MENU (STC_ID_CONVERTLF,        Editor::OnConvertEOL)
   // stc
   EVT_STC_MARGINCLICK (wxID_ANY,     Editor::OnMarginClick)
   EVT_STC_CHANGE      (wxID_ANY,     Editor::OnTextChange)
   EVT_STC_CHARADDED   (wxID_ANY,     Editor::OnCharAdded)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// Editor(wxWindow *parent, bool notifyChange, wxWindowID id,
//        const wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param  parent  Parent window pointer
 * @param  notifyChange  Set this to true if parent class to be notified of change
 *
 */
//------------------------------------------------------------------------------
Editor::Editor(wxWindow *parent, bool notifyChange, wxWindowID id,
               const wxPoint &pos, const wxSize &size, long style)
   : wxStyledTextCtrl(parent, id, pos, size, style)
{
   #ifdef DEBUG_EDITOR
   MessageInterface::ShowMessage
      ("Editor::Editor() entered, parent = <%p>\n", parent);
   #endif
   
   mParent = (GmatPanel*)parent;
   mNotifyChange = notifyChange;
   mFileName = wxEmptyString;
   mFindReplaceDialog = NULL;
   
   mLineNumberID = 0;
   mDividerID = 1;
   mFoldingID = 2;
   mPrevLineNumber = 1;
   mLastSelectPos = -1;
   mLastFindPos = -1;
   
   // initialize language
   mLanguage = NULL;
   
   // Set EOL mode to wxSTC_EOL_LF. Why wxSTC_EOL_CRLF keep adding blank line?
   // wxSTC_EOL_CRLF (Windows), wxSTC_EOL_CR (Macintosh) and wxSTC_EOL_LF (Linux)
   SetEOLMode(wxSTC_EOL_LF);
   
   // default font for all styles
   SetViewEOL(GmatEditor::globalCommonPrefs.displayEOLEnable);
   SetIndentationGuides(GmatEditor::globalCommonPrefs.indentGuideEnable);
   SetEdgeMode(GmatEditor::globalCommonPrefs.longLineOnEnable?
               wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
   SetViewWhiteSpace(GmatEditor::globalCommonPrefs.whiteSpaceEnable?
                     wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
   SetOvertype(GmatEditor::globalCommonPrefs.overTypeInitial);
   SetReadOnly(GmatEditor::globalCommonPrefs.readOnlyInitial);
   SetWrapMode(GmatEditor::globalCommonPrefs.wrapModeInitial?
               wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);
   wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
   StyleSetFont(wxSTC_STYLE_DEFAULT, font);
   StyleSetForeground(wxSTC_STYLE_DEFAULT, *wxBLACK);
   StyleSetBackground(wxSTC_STYLE_DEFAULT, *wxWHITE);
   StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(_T("DARK GREY")));
   StyleSetBackground(wxSTC_STYLE_LINENUMBER, *wxWHITE);
   StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour(_T("DARK GREY")));
      
   // set visibility
   SetVisiblePolicy(wxSTC_VISIBLE_STRICT|wxSTC_VISIBLE_SLOP, 1);
   SetXCaretPolicy(wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
   SetYCaretPolicy(wxSTC_CARET_EVEN|wxSTC_VISIBLE_STRICT|wxSTC_CARET_SLOP, 1);
   
   // markers
   MarkerDefine(wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_DOTDOTDOT, _T("BLACK"), _T("BLACK"));
   MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_ARROWDOWN, _T("BLACK"), _T("BLACK"));
   MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_EMPTY,     _T("BLACK"), _T("BLACK"));
   MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_DOTDOTDOT, _T("BLACK"), _T("WHITE"));
   MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, _T("BLACK"), _T("WHITE"));
   MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY,     _T("BLACK"), _T("BLACK"));
   MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_EMPTY,     _T("BLACK"), _T("BLACK"));
   
   // miscelaneous
   mLineNumberMargin = TextWidth(wxSTC_STYLE_LINENUMBER, _T("_999999"));
   mFoldingMargin = 16;
   CmdKeyClear(wxSTC_KEY_TAB, 0); // this is done by the menu accelerator key
   SetLayoutCache(wxSTC_CACHE_PAGE);
   
   // Get object type namses to be used as keywords
   GuiInterpreter *guiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   ObjectTypeArray excList;
   excList.push_back(Gmat::PARAMETER);
   excList.push_back(Gmat::MATH_NODE);
   
   std::string creatables = guiInterpreter->GetStringOfAllFactoryItemsExcept(excList);
   mGmatKeywords = creatables.c_str();

   #ifdef DEBUG_GMAT_KEYWORDS
   MessageInterface::ShowMessage
      ("Editor::Editor():: Here is the list of keywords:\n%s\n",
       mGmatKeywords.c_str());
   #endif
   
   // set GMAT language keywords
   bool found = false;
   GmatEditor::LanguageInfoType *curInfo;
   
   #ifdef DEBUG_GMAT_KEYWORDS
   MessageInterface::ShowMessage
      ("There are %d languages defined\n", GmatEditor::globalLanguagePrefsSize);
   #endif
   
   for (int index = 0; index < GmatEditor::globalLanguagePrefsSize; index++)
   {
      curInfo = (GmatEditor::LanguageInfoType*)&GmatEditor::globalLanguagePrefs[index];
      wxString name = curInfo->name;
      
      if (name == "GMAT")
      {
         #ifdef DEBUG_GMAT_KEYWORDS
         MessageInterface::ShowMessage
            ("   ==> Found GMAT language, so setting new keywords\n");
         #endif
         curInfo->styles[4].words = mGmatKeywords.c_str();
         found = true;
         break;
      }
   }
   
   #ifdef DEBUG_GMAT_KEYWORDS
   if (!found)
      MessageInterface::ShowMessage("==> GMAT language not found!!\n");
   #endif
   
   // Let's set it to GMAT preference
   InitializePrefs("GMAT");
   
   #ifdef DEBUG_EDITOR
   MessageInterface::ShowMessage("Editor::Editor() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// ~Editor()
//------------------------------------------------------------------------------
Editor::~Editor()
{
   if (mFindReplaceDialog)
      delete mFindReplaceDialog;
   
   mFindReplaceDialog = NULL;
}


//------------------------------------------------------------------------------
// void SetFindText(const wxString &text)
//------------------------------------------------------------------------------
void Editor::SetFindText(const wxString &text)
{
   mFindText = text;
}


//------------------------------------------------------------------------------
// void SetReplaceText(const wxString &text)
//------------------------------------------------------------------------------
void Editor::SetReplaceText(const wxString &text)
{
   mReplaceText = text;
}


//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void Editor::OnSize(wxSizeEvent& event)
{
   int x = GetClientSize().x +
     (GmatEditor::globalCommonPrefs.lineNumberEnable? mLineNumberMargin: 0) +
     (GmatEditor::globalCommonPrefs.foldEnable? mFoldingMargin: 0);
   
   if (x > 0)
      SetScrollWidth(x);
   
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnFont(wxCommandEvent& event)
//------------------------------------------------------------------------------
void Editor::OnFont(wxCommandEvent& event)
{
   //@todo - need implementation
}


// edit event handlers
//------------------------------------------------------------------------------
// void OnRedo(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnRedo(wxCommandEvent &WXUNUSED(event))
{
   if (!CanRedo())
      return;
   
   Redo();
}


//------------------------------------------------------------------------------
// void OnUndo(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnUndo(wxCommandEvent &WXUNUSED(event))
{
   if (!CanUndo())
      return;
   Undo();
}

//------------------------------------------------------------------------------
// void OnClear(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnClear(wxCommandEvent &WXUNUSED(event))
{
   if (GetReadOnly())
      return;
   
   Clear();
}


//------------------------------------------------------------------------------
// void OnCut(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnCut(wxCommandEvent &WXUNUSED(event))
{
   if (GetReadOnly() ||(GetSelectionEnd()-GetSelectionStart() <= 0))
      return;
   
   Cut();
}


//------------------------------------------------------------------------------
// void OnCopy(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnCopy(wxCommandEvent &WXUNUSED(event))
{
   if (GetSelectionEnd()-GetSelectionStart() <= 0)
      return;
   
   Copy();
}


//------------------------------------------------------------------------------
// void OnPaste(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnPaste(wxCommandEvent &WXUNUSED(event))
{
   if (!CanPaste())
      return;
   
   Paste();
}


//------------------------------------------------------------------------------
// void OnFind(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnFind(wxCommandEvent &WXUNUSED(event))
{
   if (mFindReplaceDialog == NULL)
   {
      mFindReplaceDialog = new FindReplaceDialog(this, -1, "Find & Replace");
      
      #ifdef DEBUG_EDITOR_FIND
      MessageInterface::ShowMessage
         ("Editor::OnFind() FindReplaceDialog <%p> created\n", mFindReplaceDialog);
      #endif
      
      // show modeless dialog
      mFindReplaceDialog->Show(true);
      
      // set this as Editor
      mFindReplaceDialog->SetEditor(this);
      
      // set the current caret position to the position of the search anchor. 
      SearchAnchor();
   }
   else
   {
      // show modeless dialog
      mFindReplaceDialog->Show(true);
   }
}


//------------------------------------------------------------------------------
// void OnFindNext(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnFindNext(wxCommandEvent &WXUNUSED(event))
{
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage
      ("Editor::OnFindNext() entered, mFindReplaceDialog=<%p>, mFindText='%s'\n",
       mFindReplaceDialog, mFindText.c_str());
   #endif
   
   // According to document only SearchAnchor() is needed, but it doesn't find
   // next text, so added GotoPos(). But GotoPos() removes previous selection.
   GotoPos(mLastSelectPos);
   SearchAnchor();
      
   #ifdef DEBUG_EDITOR_FIND
   int anchorPos = GetAnchor();
   MessageInterface::ShowMessage("   anchorPos=%d\n", anchorPos);
   #endif
   
   // Find some text starting at the search anchor.
   // This does not ensure the selection is visible.
   mLastFindPos = SearchNext(wxSTC_FIND_WHOLEWORD, mFindText);
   int line = GetCurrentLine();
   
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage
      ("   mLastFindPos=%d, line=%d\n", mLastFindPos, line);
   #endif
   
   // Scroll to line to make line with selection visible
   int scrollOffset = LinesOnScreen();
   ScrollToLine(line - (int)(scrollOffset/2));
   
   // Save selection end for the next search
   mLastSelectPos = GetSelectionEnd();
   
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage("mLastSelectPos=%d\n", mLastSelectPos);
   #endif
}


//------------------------------------------------------------------------------
// void OnFindPrev(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnFindPrev(wxCommandEvent &WXUNUSED(event))
{
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage
      ("Editor::OnFindPrev() entered, mFindReplaceDialog=<%p>, mFindText='%s'\n",
       mFindReplaceDialog, mFindText.c_str());
   #endif
   
   // According to document only SearchAnchor() is needed, but it doesn't find
   // next text, so added GotoPos(). But GotoPos() removes previous selection.
   GotoPos(mLastSelectPos);
   SearchAnchor();
   
   #ifdef DEBUG_EDITOR_FIND
   int anchorPos = GetAnchor();
   MessageInterface::ShowMessage("   anchorPos=%d\n", anchorPos);
   #endif
   
   // Find some text starting at the search anchor and moving backwards.
   // This does not ensure the selection is visible.
   mLastFindPos = SearchPrev(wxSTC_FIND_WHOLEWORD, mFindText);
   int line = GetCurrentLine();
   
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage
      ("    mLastFindPos=%d, line=%d\n", mLastFindPos, line);
   #endif
   
   // Scroll to line to make line with selection visible
   int scrollOffset = LinesOnScreen();
   ScrollToLine(line - (int)(scrollOffset/2));
   
   // Save selection start for the next search
   mLastSelectPos = GetSelectionStart();
   
   #ifdef DEBUG_EDITOR_FIND
   MessageInterface::ShowMessage("mLastSelectPos=%d\n", mLastSelectPos);
   #endif
}


//------------------------------------------------------------------------------
// void OnReplaceNext(wxCommandEvent &event)
//------------------------------------------------------------------------------
void Editor::OnReplaceNext(wxCommandEvent &event)
{
   if (mLastFindPos == -1)
      OnFindNext(event);

   // if text to replace not found, return
   if (mLastFindPos == -1)
      return;
   
   // Replace the selected text with the argument text.
   ReplaceSelection(mReplaceText);
   OnFindNext(event);
}


//------------------------------------------------------------------------------
// void OnReplaceAll(wxCommandEvent &event)
//------------------------------------------------------------------------------
void Editor::OnReplaceAll(wxCommandEvent &event)
{
   OnFindNext(event);
   while (mLastFindPos != -1)
   {
      ReplaceSelection(mReplaceText);
      OnFindNext(event);
   }
}


//------------------------------------------------------------------------------
// void OnBraceMatch(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnBraceMatch(wxCommandEvent &WXUNUSED(event))
{
   int min = GetCurrentPos();
   int max = BraceMatch(min);
   if (max >(min+1))
   {
      BraceHighlight(min+1, max);
      SetSelection(min+1, max);
   }
   else
   {
      BraceBadLight(min);
   }
}


//------------------------------------------------------------------------------
// void OnGoToLine(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnGoToLine(wxCommandEvent &WXUNUSED(event))
{
   long lineNumber = wxGetNumberFromUser
      ("", "Line Number", "Go To", mPrevLineNumber, 1, 100000, this);
   
   if (lineNumber != -1)
   {
      mPrevLineNumber = lineNumber;      
      GotoLine(lineNumber-1);
   }
}


//------------------------------------------------------------------------------
// void OnIndentMore(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnIndentMore(wxCommandEvent &WXUNUSED(event))
{
   CmdKeyExecute(wxSTC_CMD_TAB);
}


//------------------------------------------------------------------------------
// void OnIndentLess(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnIndentLess(wxCommandEvent &WXUNUSED(event))
{
   CmdKeyExecute(wxSTC_CMD_BACKTAB);
}


//------------------------------------------------------------------------------
// void OnSelectAll(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnSelectAll(wxCommandEvent &WXUNUSED(event))
{
   SetSelection(0, GetTextLength());
}


//------------------------------------------------------------------------------
// void OnSelectLine(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnSelectLine(wxCommandEvent &WXUNUSED(event))
{
   int lineStart = PositionFromLine(GetCurrentLine());
   int lineEnd = PositionFromLine(GetCurrentLine() + 1);
   SetSelection(lineStart, lineEnd);
}


//------------------------------------------------------------------------------
// void OnComment(wxCommandEvent &event)
//------------------------------------------------------------------------------
void Editor::OnComment(wxCommandEvent &event)
{
   #ifdef DEBUG_EDITOR_COMMENT
   MessageInterface::ShowMessage("Editor::OnComment() entered\n");
   #endif
   
   // Retrieve the selected text.
   wxString selString = GetSelectedText();
   selString.Replace("\n", "\n%");
   selString = "%" + selString;
   
   if (selString.Last() == '%')
      selString = selString.Mid(0, selString.Length()-1);
   
   // Replace the selected text with the argument text.
   ReplaceSelection(selString);
   
   #ifdef DEBUG_EDITOR_COMMENT
   MessageInterface::ShowMessage("Editor::OnComment() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnUncomment(wxCommandEvent &event)
//------------------------------------------------------------------------------
void Editor::OnUncomment(wxCommandEvent &event)
{
   #ifdef DEBUG_EDITOR_COMMENT
   MessageInterface::ShowMessage("Editor::OnUncomment() entered\n");
   #endif
   
   // Retrieve the selected text.
   wxString selString = GetSelectedText();
   
   if (selString.StartsWith("%"))  // gets rid of first %
      selString = selString.Mid(1, selString.Length()-1);
   
   selString.Replace("\n%", "\n");
   
   // Replace the selected text with the argument text.
   ReplaceSelection(selString);
   
   #ifdef DEBUG_EDITOR_COMMENT
   MessageInterface::ShowMessage("Editor::OnUncomment() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnHilightLang(wxCommandEvent &event)
//------------------------------------------------------------------------------
void Editor::OnHilightLang(wxCommandEvent &event)
{
   InitializePrefs(GmatEditor::globalLanguagePrefs[event.GetId() -
                                                   STC_ID_HILIGHTFIRST].name);
}


//--------------------------------------------------------------------------------
// void OnDisplayEOL(wxCommandEvent &WXUNUSED(event))
//--------------------------------------------------------------------------------
void Editor::OnDisplayEOL(wxCommandEvent &WXUNUSED(event))
{
   SetViewEOL(!GetViewEOL());
}


//------------------------------------------------------------------------------
// void OnIndentGuide(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnIndentGuide(wxCommandEvent &WXUNUSED(event))
{
   SetIndentationGuides(!GetIndentationGuides());
}


//------------------------------------------------------------------------------
// void OnLineNumber(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnLineNumber(wxCommandEvent &WXUNUSED(event))
{
   SetMarginWidth(mLineNumberID,
                  GetMarginWidth(mLineNumberID) == 0 ? mLineNumberMargin : 0);
}


//------------------------------------------------------------------------------
// void OnLongLineOn(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnLongLineOn(wxCommandEvent &WXUNUSED(event))
{
   SetEdgeMode(GetEdgeMode() == 0? wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
}


//------------------------------------------------------------------------------
// void OnWhiteSpace(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnWhiteSpace(wxCommandEvent &WXUNUSED(event))
{
   SetViewWhiteSpace(GetViewWhiteSpace() == 0?
                      wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
}


//------------------------------------------------------------------------------
// void OnFoldToggle(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnFoldToggle(wxCommandEvent &WXUNUSED(event))
{
   ToggleFold(GetFoldParent(GetCurrentLine()));
}


//------------------------------------------------------------------------------
// void OnSetOverType(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnSetOverType(wxCommandEvent &WXUNUSED(event))
{
   SetOvertype(!GetOvertype());
}


//------------------------------------------------------------------------------
// void OnSetReadOnly(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnSetReadOnly(wxCommandEvent &WXUNUSED(event))
{
   SetReadOnly(!GetReadOnly());
}


//------------------------------------------------------------------------------
// void OnWrapmodeOn(wxCommandEvent &WXUNUSED(event))
//------------------------------------------------------------------------------
void Editor::OnWrapmodeOn(wxCommandEvent &WXUNUSED(event))
{
   SetWrapMode(GetWrapMode() == 0? wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);
}


//------------------------------------------------------------------------------
// void OnUseCharset(wxCommandEvent &event)
//------------------------------------------------------------------------------
void Editor::OnUseCharset(wxCommandEvent &event)
{
   int Nr;
   int charset = GetCodePage();
   
//    switch(event.GetId())
//    {
//    case STC_ID_CHARSETANSI:
//       charset = wxSTC_CHARSET_ANSI;
//       break;
//    case STC_ID_CHARSETMAC:
//       charset = wxSTC_CHARSET_ANSI;
//       break;
//    }
   
   for (Nr = 0; Nr < wxSTC_STYLE_LASTPREDEFINED; Nr++)
      StyleSetCharacterSet(Nr, charset);
   
   SetCodePage(charset);
}


//------------------------------------------------------------------------------
// void OnChangeCase(wxCommandEvent &event)
//------------------------------------------------------------------------------
void Editor::OnChangeCase(wxCommandEvent &event)
{
//    switch(event.GetId())
//    {
//    case STC_ID_CHANGELOWER:
//       CmdKeyExecute(wxSTC_CMD_LOWERCASE);
//       break;
//    case STC_ID_CHANGEUPPER:
//       CmdKeyExecute(wxSTC_CMD_UPPERCASE);
//       break;
//    }
}


//------------------------------------------------------------------------------
// void OnConvertEOL(wxCommandEvent &event)
//------------------------------------------------------------------------------
void Editor::OnConvertEOL(wxCommandEvent &event)
{
   int eolMode = GetEOLMode();
//    switch(event.GetId())
//    {
//    case STC_ID_CONVERTCR:
//       eolMode = wxSTC_EOL_CR;
//       break;
//    case STC_ID_CONVERTCRLF:
//       eolMode = wxSTC_EOL_CRLF;
//       break;
//    case STC_ID_CONVERTLF:
//       eolMode = wxSTC_EOL_LF;
//       break;
//    }
   ConvertEOLs(eolMode);
   SetEOLMode(eolMode);
}


//------------------------------------------------------------------------------
// void Editor::OnMarginClick (wxStyledTextEvent &event)
//------------------------------------------------------------------------------
void Editor::OnMarginClick (wxStyledTextEvent &event)
{
   if (event.GetMargin() == 2)
   {
      int lineClick = LineFromPosition (event.GetPosition());
      int levelClick = GetFoldLevel (lineClick);
      if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
         ToggleFold (lineClick);
   }
}


//------------------------------------------------------------------------------
// void OnTextChange (wxStyledTextEvent &event)
//------------------------------------------------------------------------------
void Editor::OnTextChange (wxStyledTextEvent &event)
{
   if (mNotifyChange)
   {
      if (IsModified())
      {
         mParent->SetEditorModified(true);
         GmatAppData::Instance()->GetMainFrame()->SetActiveChildDirty(true);
      }
   }
}


//------------------------------------------------------------------------------
// void OnCharAdded (wxStyledTextEvent &event)
//------------------------------------------------------------------------------
void Editor::OnCharAdded (wxStyledTextEvent &event)
{
   char chr = (char)event.GetKey();
   int currentLine = GetCurrentLine();
   // Change this if support for mac files with \r is needed
   if (chr == '\n')
   {
      int lineInd = 0;
      if (currentLine > 0)
         lineInd = GetLineIndentation(currentLine - 1);
      if (lineInd == 0)
         return;
      SetLineIndentation (currentLine, lineInd);
      GotoPos(PositionFromLine (currentLine) + lineInd);
   }
}

//----------------------------
// private methods
//----------------------------

//------------------------------------------------------------------------------
// wxString DeterminePrefs(const wxString &filename)
//------------------------------------------------------------------------------
wxString Editor::DeterminePrefs(const wxString &filename)
{
   #ifdef DEBUG_EDITOR_PREF
   MessageInterface::ShowMessage
     ("Editor::DeterminePrefs() entered, filename='%s'\n", filename.c_str());
   #endif
   
   GmatEditor::LanguageInfoType const* curInfo;
   
   // determine language from filepatterns
   int index;
   for (index = 0; index < GmatEditor::globalLanguagePrefsSize; index++)
   {
      curInfo = &GmatEditor::globalLanguagePrefs [index];
      wxString filepattern = curInfo->filepattern;
      filepattern.Lower();
      while(!filepattern.empty()) {
         wxString cur = filepattern.BeforeFirst(';');
         if ((cur == filename) ||
            (cur ==(filename.BeforeLast('.') + _T(".*"))) ||
            (cur ==(_T("*.") + filename.AfterLast('.'))))
         {
            #ifdef DEBUG_EDITOR_PREF
            MessageInterface::ShowMessage
              ("Editor::DeterminePrefs() exiting with '%s'\n", curInfo->name);
            #endif
            
            return curInfo->name;
         }
         filepattern = filepattern.AfterFirst(';');
      }
   }
   
   #ifdef DEBUG_EDITOR_PREF
   MessageInterface::ShowMessage("Editor::DeterminePrefs() exiting with empty string\n");
   #endif
   
   return wxEmptyString;
}


//------------------------------------------------------------------------------
// bool InitializePrefs(const wxString &name)
//------------------------------------------------------------------------------
bool Editor::InitializePrefs(const wxString &name)
{
   #ifdef DEBUG_EDITOR_PREF
   MessageInterface::ShowMessage
     ("Editor::InitializePrefs() entered, name='%s'\n", name.c_str());
   #endif
   
   // initialize styles
   StyleClearAll();
   GmatEditor::LanguageInfoType const* curInfo = NULL;
   
   // determine language
   bool found = false;
   for (int index = 0; index < GmatEditor::globalLanguagePrefsSize; index++)
   {
      curInfo = &GmatEditor::globalLanguagePrefs[index];
      if (curInfo->name == name)
      {
         found = true;
         break;
      }
   }
   
   if (!found)
   {
      #ifdef DEBUG_EDITOR_PREF
      MessageInterface::ShowMessage("Editor::InitializePrefs() returning false\n");
      #endif
      return false;
   }
   
   // set lexer and language
   SetLexer(curInfo->lexer);
   mLanguage = curInfo;
   
   // set margin for line numbers
   SetMarginType(mLineNumberID, wxSTC_MARGIN_NUMBER);
   StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColour(_T("DARK GREY")));
   StyleSetBackground(wxSTC_STYLE_LINENUMBER, *wxWHITE);
   
   //SetMarginWidth(mLineNumberID, 0); // start out not visible
   SetMarginWidth(mLineNumberID, mLineNumberMargin); // show line numbers
   
   // default fonts for all styles!
   int index;
   for (index = 0; index < wxSTC_STYLE_LASTPREDEFINED; index++)
   {
      wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
      StyleSetFont(index, font);
   }
   
   // set common styles
   StyleSetForeground(wxSTC_STYLE_DEFAULT, wxColour(_T("DARK GREY")));
   StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour(_T("DARK GREY")));
   
   // initialize settings
   if (GmatEditor::globalCommonPrefs.syntaxEnable)
   {
      int keywordIndex = 0;
      for (index = 0; index < STYLE_TYPES_COUNT; index++)
      {
         int styleType = curInfo->styles[index].type;
         if (styleType == -1)
            continue;
         
         const GmatEditor::StyleInfoType &curType =
            GmatEditor::globalStylePrefs[styleType];
         wxFont font(curType.fontsize, wxMODERN, wxNORMAL, wxNORMAL, false,
                     curType.fontname);
         StyleSetFont(index, font);
         
         if (curType.foreground)
            StyleSetForeground(index, wxColour(curType.foreground));
         
         if (curType.background)
            StyleSetBackground(index, wxColour(curType.background));
         
         StyleSetBold(index,(curType.fontstyle & GMAT_STC_STYLE_BOLD) > 0);
         StyleSetItalic(index,(curType.fontstyle & GMAT_STC_STYLE_ITALIC) > 0);
         StyleSetUnderline(index,(curType.fontstyle & GMAT_STC_STYLE_UNDERL) > 0);
         StyleSetVisible(index,(curType.fontstyle & GMAT_STC_STYLE_HIDDEN) == 0);
         StyleSetCase(index, curType.lettercase);
         
         const wxChar *svalue = curInfo->styles[index].words;
         if (svalue)
         {
            SetKeyWords(keywordIndex, svalue);
            keywordIndex += 1;
         }
      }
   }
   
   // set margin as unused
   SetMarginType(mDividerID, wxSTC_MARGIN_SYMBOL);
   SetMarginWidth(mDividerID, 0);
   SetMarginSensitive(mDividerID, false);
   
   // folding
   SetMarginType(mFoldingID, wxSTC_MARGIN_SYMBOL);
   SetMarginMask(mFoldingID, wxSTC_MASK_FOLDERS);
   StyleSetBackground(mFoldingID, *wxWHITE);
   SetMarginWidth(mFoldingID, 0);
   SetMarginSensitive(mFoldingID, false);
   if (GmatEditor::globalCommonPrefs.foldEnable)
   {
      SetMarginWidth(mFoldingID, curInfo->folds != 0? mFoldingMargin: 0);
      SetMarginSensitive(mFoldingID, curInfo->folds != 0);
      SetProperty(_T("fold"), curInfo->folds != 0? _T("1"): _T("0"));
      SetProperty(_T("fold.comment"),
                  (curInfo->folds & GMAT_STC_FOLD_COMMENT) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.compact"),
                  (curInfo->folds & GMAT_STC_FOLD_COMPACT) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.preprocessor"),
                  (curInfo->folds & GMAT_STC_FOLD_PREPROC) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.html"),
                  (curInfo->folds & GMAT_STC_FOLD_HTML) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.html.preprocessor"),
                  (curInfo->folds & GMAT_STC_FOLD_HTMLPREP) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.comment.python"),
                  (curInfo->folds & GMAT_STC_FOLD_COMMENTPY) > 0? _T("1"): _T("0"));
      SetProperty(_T("fold.quotes.python"),
                  (curInfo->folds & GMAT_STC_FOLD_QUOTESPY) > 0? _T("1"): _T("0"));
   }
   SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
   
   // set spaces and indention
   SetTabWidth(3);
   SetUseTabs(false);
   SetTabIndents(true);
   SetBackSpaceUnIndents(true);
   SetIndent(GmatEditor::globalCommonPrefs.indentEnable ? 3 : 0);
   
   // others
   SetViewEOL(GmatEditor::globalCommonPrefs.displayEOLEnable);
   SetIndentationGuides(GmatEditor::globalCommonPrefs.indentGuideEnable);
   SetEdgeColumn(80);
   SetEdgeMode(GmatEditor::globalCommonPrefs.longLineOnEnable? wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
   SetViewWhiteSpace(GmatEditor::globalCommonPrefs.whiteSpaceEnable?
                     wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
   SetOvertype(GmatEditor::globalCommonPrefs.overTypeInitial);
   SetReadOnly(GmatEditor::globalCommonPrefs.readOnlyInitial);
   SetWrapMode(GmatEditor::globalCommonPrefs.wrapModeInitial?
               wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);
   
   #ifdef DEBUG_EDITOR_PREF
   MessageInterface::ShowMessage("Editor::InitializePrefs() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// wxString GetLine(int lineNumber)
//------------------------------------------------------------------------------
wxString Editor::GetLine(int lineNumber)
{
   return wxStyledTextCtrl::GetLine(lineNumber);
}


//------------------------------------------------------------------------------
// wxString GetText()
//------------------------------------------------------------------------------
wxString Editor::GetText()
{
   return wxStyledTextCtrl::GetText();
}


//------------------------------------------------------------------------------
// bool LoadFile()
//------------------------------------------------------------------------------
bool Editor::LoadFile()
{
#if wxUSE_FILEDLG
   // get filname
   if (!mFileName) {
      wxFileDialog dlg(this, _T("Open file"), wxEmptyString, wxEmptyString,
                        _T("Any file(*)|*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
      if (dlg.ShowModal() != wxID_OK) return false;
      mFileName = dlg.GetPath();
   }
   
   // load file
   return LoadFile(mFileName);
#else
   return false;
#endif // wxUSE_FILEDLG
}


//------------------------------------------------------------------------------
// bool LoadFile(const wxString &filename)
//------------------------------------------------------------------------------
bool Editor::LoadFile(const wxString &filename)
{
   #ifdef DEBUG_EDITOR_FILE
   MessageInterface::ShowMessage
     ("Editor::LoadFile() entered, this=<%p>, filename='%s'\n", this, filename.c_str());
   #endif
   
   // load file in edit and clear undo
   if (!filename.empty())
      mFileName = filename;
   
   // It's ok to have non-existing file
   //wxFile file(mFileName);
   //if (!file.IsOpened())
   //   return false;
   
   ClearAll();

   // No file reading
   //long lng = file.Length();
   //if (lng > 0)
   //{
   //   wxString buf;
   //   wxChar *buff = buf.GetWriteBuf(lng);
   //   file.Read(buff, lng);
   //   buf.UngetWriteBuf();
   //   InsertText(0, buf);
   //}
   //file.Close();
   
   wxStyledTextCtrl::LoadFile(mFileName);
   EmptyUndoBuffer();
   
   // determine lexer language
   wxFileName fname(mFileName);
   InitializePrefs(DeterminePrefs(fname.GetFullName()));
   
   #ifdef DEBUG_EDITOR_FILE
   MessageInterface::ShowMessage("Editor::LoadFile() exiting with true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool SaveFile()
//------------------------------------------------------------------------------
bool Editor::SaveFile()
{
#if wxUSE_FILEDLG
   // return if no change
   if (!IsModified()) return true;

   // get filname
   if (!mFileName)
   {
      wxFileDialog dlg(this, _T("Save file"), wxEmptyString, wxEmptyString,
                        _T("Any file(*)|*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
      if (dlg.ShowModal() != wxID_OK) return false;
      mFileName = dlg.GetPath();
   }
   
   // save file
   return SaveFile(mFileName);
#else
   return false;
#endif // wxUSE_FILEDLG
}


//------------------------------------------------------------------------------
// bool SaveFile(const wxString &filename)
//------------------------------------------------------------------------------
bool Editor::SaveFile(const wxString &filename)
{
   // Always save to file (loj: 2009.01.29)
   // return if no change
   //if (!IsModified()) return true;
   
   //     // save edit in file and clear undo
   //     if (!filename.empty()) mFileName = filename;
   //     wxFile file(mFileName, wxFile::write);
   //     if (!file.IsOpened()) return false;
   //     wxString buf = GetText();
   //     bool okay = file.Write(buf);
   //     file.Close();
   //     if (!okay) return false;
   //     EmptyUndoBuffer();
   //     SetSavePoint();
   
   //     return true;
   
   return wxStyledTextCtrl::SaveFile(filename);

}


//------------------------------------------------------------------------------
// bool IsModified()
//------------------------------------------------------------------------------
bool Editor::IsModified()
{
   bool state = (GetModify() && !GetReadOnly());
   
   #ifdef DEBUG_EDITOR_MODIFIED
   MessageInterface::ShowMessage("Editor::IsModified() returning %d\n", state);
   #endif
   
   return state;
}

