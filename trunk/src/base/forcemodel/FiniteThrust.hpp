//$Header$
//------------------------------------------------------------------------------
//                              FiniteThrust
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/12/20
//
/**
 * Defines the FiniteThrust class used to model the acceleration during a finite 
 * burn. 
 */
//------------------------------------------------------------------------------


#ifndef FiniteThrust_hpp
#define FiniteThrust_hpp

#include "PhysicalModel.hpp"
#include "FiniteBurn.hpp"


/**
 * The force used for finite burns.
 */
class GMAT_API FiniteThrust : public PhysicalModel
{
public:
   FiniteThrust(const std::string &name = "");
   virtual ~FiniteThrust();
   FiniteThrust(const FiniteThrust& ft);
   FiniteThrust&           operator=(const FiniteThrust& ft);

   virtual GmatBase*       Clone() const;

   virtual void            Clear(const Gmat::ObjectType 
                                 type = Gmat::UNKNOWN_OBJECT);
   virtual bool            SetRefObjectName(const Gmat::ObjectType type,
                                            const std::string &name);
   virtual const StringArray&
                           GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool            SetRefObject(GmatBase *obj, 
                              const Gmat::ObjectType type, 
                              const std::string &name = "");
//   virtual bool            SetRefObject(GmatBase *obj, 
//                              const Gmat::ObjectType type,
//                              const std::string &name, const Integer index);
   virtual bool            IsTransient();
   virtual void            SetPropList(std::vector<SpaceObject*> *soList);
   virtual bool            Initialize();
   virtual bool            GetDerivatives(Real * state, Real dt, Integer order);
protected:
   // Pieces needed for bookkeeping
   /// Not sure if this is needed yet
   std::vector <FiniteBurn *>    burns;
   /// Names for the FiniteBurn objects used for this force 
   StringArray                   burnNames;
   /// Names of the spacecraft accessed by this force
   StringArray                   mySpacecraft;
   /// Spacecraft, in order, used in the ForceModel
   std::vector<SpaceObject *>    *spacecraft;
   /// Indexes (in the spacecraft vector) for the Spacecraft used by this force
   std::vector<Integer>          scIndices;
};

#endif // FiniteThrust_hpp
