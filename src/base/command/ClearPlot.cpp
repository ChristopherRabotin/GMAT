#include "ClearPlot.hpp"

#include "MessageInterface.hpp"

ClearPlot::ClearPlot() :
   GmatCommand    ("ClearPlot"),
   plotName       (""),
   thePlot        (NULL)
{
}


ClearPlot::~ClearPlot()
{
}


ClearPlot::ClearPlot(const ClearPlot &c) :
   GmatCommand    (c),
   plotName       (c.plotName),
   thePlot        (NULL)
{
}

ClearPlot& ClearPlot::operator=(const ClearPlot &c)
{
   if (&c != this)
   {
      plotName = c.plotName;
      thePlot = NULL;
   }
   
   return *this;
}


GmatBase* ClearPlot::Clone() const
{
   return new ClearPlot(*this);
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
bool ClearPlot::InterpretAction()
{
   Integer loc = generatingString.find("ClearPlot", 0) + 9, end;
   const char *str = generatingString.c_str();
   while (str[loc] == ' ')
      ++loc;
    
   // Find the Subscriber list
   end = generatingString.find(" ", loc);
   plotName = generatingString.substr(loc, end-loc);
   
   #ifdef DEBUG_CLEARPLOT
      MessageInterface::ShowMessage("Plot to be cleared: %s\n", plotName.c_str());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool ClearPlot::Initialize()
{
   #ifdef DEBUG_CLEARPLOT
      MessageInterface::ShowMessage("ClearPlot::Initialize() entered\n");
   #endif
      
   GmatCommand::Initialize();
   
   GmatBase *xy;
   
   if ((*objectMap).find(plotName) != objectMap->end()) 
   {
      xy = (GmatBase *)(*objectMap)[plotName];
      if (xy->GetTypeName() == "XYPlot") 
         thePlot = (TsPlot*)xy;
      else
         throw CommandException(
            "Object named \"" + plotName + "\" should be an XYPlot to use the "
            "ClearPlot command for this object, but it is a " + 
            xy->GetTypeName());      
   }
   else 
   {
      MessageInterface::ShowMessage
         ("ClearPlot command cannot find XY Plot \"%s\"; command has no effect."
         "\n", plotName.c_str());
      return false;
   }

   return true;
}


//---------------------------------------------------------------------------
//  bool GmatCommand::Execute()
//---------------------------------------------------------------------------
/**
 * The method that is fired to perform the GmatCommand.
 *
 * Derived classes implement this method to perform their actions on
 * GMAT objects.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//---------------------------------------------------------------------------
bool ClearPlot::Execute()
{
   if (thePlot)
      thePlot->TakeAction("ClearData");
   return true;
}
