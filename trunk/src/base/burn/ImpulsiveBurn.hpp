//$Header$
//------------------------------------------------------------------------------
//                              ImpulsiveBurn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the ImpulsiveBurn class used for maneuvers. 
 */
//------------------------------------------------------------------------------


#ifndef ImpulsiveBurn_hpp
#define ImpulsiveBurn_hpp


#include "Burn.hpp"


class GMAT_API ImpulsiveBurn : public Burn
{
public:
   ImpulsiveBurn(const std::string &nomme = "");
   virtual ~ImpulsiveBurn();
   ImpulsiveBurn(const ImpulsiveBurn &copy);
   ImpulsiveBurn&       operator=(const ImpulsiveBurn &orig);
   
   virtual bool         Fire(Real *burnData = NULL, Real epoch = 21545.0);
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
protected:
   void                 TransformDeltaVToJ2kFrame(Real *deltaV, Real epoch);
};

#endif // ImpulsiveBurn_hpp
