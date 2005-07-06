#include "Report.hpp"

Report::Report() :
   GmatCommand ("Report"),
   rfName      (""),
   reporter    (NULL)
{
}

Report::~Report()
{
}


Report::Report(const Report &rep) :
   GmatCommand    (rep),
   rfName         (rep.rfName),
   reporter       (NULL)
{
}


Report& Report::operator=(const Report &rep)
{
   if (this != &rep)
   {
      rfName = rep.rfName;
      reporter = NULL;
   }
   
   return *this;
}


GmatBase* Report::Clone() const
{
   return new Report(*this);
}


bool Report::InterpretAction()
{
   return true;
}


bool Report::Execute()
{
   return true;
}
