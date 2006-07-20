#ifndef ENDOPTIMIZE_HPP_
#define ENDOPTIMIZE_HPP_

#include "GmatCommand.hpp"


class GMAT_API EndOptimize : public GmatCommand
{
public:
   EndOptimize();
   virtual ~EndOptimize();
    
   EndOptimize(const EndOptimize& et);
   EndOptimize&              operator=(const EndOptimize& et);
    
   virtual bool            Initialize();
   virtual bool            Execute();
    
   virtual bool            Insert(GmatCommand *cmd, GmatCommand *prev);

   // inherited from GmatBase
   virtual GmatBase*       Clone() const;
   virtual const std::string&
                           GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName);
};


#endif /*ENDOPTIMIZE_HPP_*/
