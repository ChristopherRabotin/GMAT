
#include "EndOptimize.hpp"
#include "BranchCommand.hpp"
#include "MessageInterface.hpp"

EndOptimize::EndOptimize() :
   GmatCommand         ("EndOptimize")
{
   objectTypeNames.push_back("BranchEnd");
   depthChange = -1;
}

EndOptimize::~EndOptimize()
{
}
    
EndOptimize::EndOptimize(const EndOptimize& et) :
   GmatCommand         (et)
{
}

EndOptimize& EndOptimize::operator=(const EndOptimize& et)
{
   if (this == &et)
      return *this;
    
   return *this;
}
    
bool EndOptimize::Initialize()
{
   GmatCommand::Initialize();
   
   // Validate that next points to the owning Optimize command
   if (!next)
      throw CommandException("EndOptimize Command not properly reconnected");
    
   if (next->GetTypeName() != "Optimize")
      throw CommandException("EndOptimize Command not connected to Optimize Command");
    
   return true;
}

bool EndOptimize::Execute()
{
   #ifdef DEBUG_TARGET_COMMANDS
      if (next)
         MessageInterface::ShowMessage(
            "End Optimize points to a %s command\n", next->GetTypeName().c_str());
      else
         MessageInterface::ShowMessage(
            "EndOptimize does not reconnect to Optimize comamnd\n");
   #endif
   
   BuildCommandSummary(true);
   return true;
}

bool EndOptimize::Insert(GmatCommand *cmd, GmatCommand *prev)
{
   // if inserting after End statement for branch command, we want to 
   // insert right after the entire If command
   if (this == prev)
      return ((BranchCommand*)next)->InsertRightAfter(cmd);
   return false;
}

GmatBase* EndOptimize::Clone() const
{
   return (new EndOptimize(*this));
}

const std::string& EndOptimize::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   generatingString = prefix + "EndOptimize;";
   if ((next) && (next->GetTypeName() == "Optimize"))
   {
      generatingString += "  % For optimizer ";
      generatingString += next->GetRefObjectName(Gmat::SOLVER);
   }
   return generatingString;
}
