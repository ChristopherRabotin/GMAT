//$Header$
//------------------------------------------------------------------------------
//                             File: OrbitalParameters.hpp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/12
// Modified: Wendy Shoan 2004/04/01 added OrbitPeriod
//
/**
 * Declares other Keplerian related parameter classes.
 *   VelApoapsis, VelPeriapsis, Apoapsis, Periapsis, OrbitPeriod
 */
//------------------------------------------------------------------------------
#ifndef OrbitalParameters_hpp
#define OrbitalParameters_hpp

#include "OrbitReal.hpp"

//==============================================================================
//                              VelApoapsis
//==============================================================================
/**
 * Declares Velocity at Apoapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API VelApoapsis : public OrbitReal
{
public:

   VelApoapsis(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "",
               const std::string &unit = "Km/Sec");
   VelApoapsis(const VelApoapsis &copy);
   const VelApoapsis& operator=(const VelApoapsis &right);
   virtual ~VelApoapsis();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              VelPeriapsis
//==============================================================================
/**
 * Declares Velocity at Periapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API VelPeriapsis : public OrbitReal
{
public:

   VelPeriapsis(const std::string &name = "",
                GmatBase *obj = NULL,
                const std::string &desc = "",
                const std::string &unit = "Km/Sec");
   VelPeriapsis(const VelPeriapsis &copy);
   const VelPeriapsis& operator=(const VelPeriapsis &right);
   virtual ~VelPeriapsis();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              Apoapsis
//==============================================================================
/**
 * Declares Apoapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API Apoapsis : public OrbitReal
{
public:

   Apoapsis(const std::string &name = "",
            GmatBase *obj = NULL,
            const std::string &desc = "",
            const std::string &unit = "Km/Sec");
   Apoapsis(const Apoapsis &copy);
   const Apoapsis& operator=(const Apoapsis &right);
   virtual ~Apoapsis();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              Periapsis
//==============================================================================
/**
 * Declares Periapsis class.
*/
//------------------------------------------------------------------------------

class GMAT_API Periapsis : public OrbitReal
{
public:

   Periapsis(const std::string &name = "",
             GmatBase *obj = NULL,
             const std::string &desc = "",
             const std::string &unit = "");
   Periapsis(const Periapsis &copy);
   const Periapsis& operator=(const Periapsis &right);
   virtual ~Periapsis();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              OrbitPeriod
//==============================================================================
/**
 * Declares OrbitPeriod class.
 */
//------------------------------------------------------------------------------

class GMAT_API OrbitPeriod : public OrbitReal
{
public:

   OrbitPeriod(const std::string &name = "",
               GmatBase *obj = NULL,
               const std::string &desc = "",
               const std::string &unit = "");
   OrbitPeriod(const OrbitPeriod &copy);
   const OrbitPeriod& operator=(const OrbitPeriod &right);
   virtual ~OrbitPeriod();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};
#endif // OrbitalParameters_hpp
