#ifndef CLEARPLOT_HPP_
#define CLEARPLOT_HPP_

#include "GmatCommand.hpp"
#include "TsPlot.hpp"


/**
 * Command used to remove data from an XY plot during a run
 */
class ClearPlot : public GmatCommand
{
public:
	ClearPlot();
	virtual          ~ClearPlot();
   ClearPlot(const ClearPlot &c);
   ClearPlot&        operator=(const ClearPlot &c);
   
   virtual GmatBase* Clone() const;
   
   bool              InterpretAction();
   bool              Initialize();
   bool              Execute();

protected:
   std::string plotName;
   TsPlot      *thePlot;
};

#endif /*CLEARPLOT_HPP_*/
