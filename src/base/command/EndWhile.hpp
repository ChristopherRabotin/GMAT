//$Header$
//------------------------------------------------------------------------------
//                             EndWHile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Wendy Shoan/GSFC/MAB
// Created: 2004/08/11
//
/**
* Definition for the closing line of the While Statement
 */
//------------------------------------------------------------------------------


#ifndef EndWhile_hpp
#define EndWhile_hpp


#include "Command.hpp"


class EndWhile : public GmatCommand
{
public:
   EndWhile(void);
   virtual ~EndWhile(void);

   EndWhile(const EndWhile& ew);
   EndWhile&              operator=(const EndWhile& ew);

   virtual bool            Initialize(void);
   virtual bool            Execute(void);

   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

};


#endif // EndWhile_hpp
