//$Header$
//------------------------------------------------------------------------------
//                                  AxisSystem
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2004/12/20
//
/**
 * Definition of the AxisSystem class.  This is the base class for the 
 * InertialAxes and DynamicAxes classes.
 *
 */
//------------------------------------------------------------------------------

#ifndef AxisSystem_hpp
#define AxisSystem_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateBase.hpp"
#include "Rmatrix33.hpp"
#include "A1Mjd.hpp"

class GMAT_API AxisSystem : public CoordinateBase
{
public:

   // default constructor
   AxisSystem(const std::string &itsType,
              const std::string &itsName = "");
   // copy constructor
   AxisSystem(const AxisSystem &axisSys);
   // operator = for assignment
   const AxisSystem& operator=(const AxisSystem &axisSys);
   // destructor
   virtual ~AxisSystem();
   
   
   // initializes the AxisSystem
   virtual void Initialize();
   
   //---------------------------------------------------------------------------
   //  bool RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
   //                        Rvector &outState, SpacePoint *j2000Body)
   //---------------------------------------------------------------------------
   /**
    * This method will rotate the input inState into the MJ2000Eq frame.
    *
    * @param epoch     the epoch at which to perform the rotation.
    * @param inState   the input state (in this AxisSystem) to be rotated.
    * @param iutState  the output state, in the MJ2000Eq AxisSystem, the result 
    *                  of rotating the input inState.
    * @param j2000Body the origin of the output MJ2000EqAxes frame.
    *
    * @return success or failure of the operation.
    */
   //---------------------------------------------------------------------------
   virtual bool RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
                                 Rvector &outState, SpacePoint *j2000Body) = 0;

   //---------------------------------------------------------------------------
   //  bool RotateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
   //                          Rvector &outState, SpacePoint *j2000Body)
   //---------------------------------------------------------------------------
   /**
    * This method will rotate the input inState from the MJ2000Eq frame into
    * this AxisSystem.
    *
    * @param epoch     the epoch at which to perform the rotation.
    * @param inState   the input state (in MJ2000Eq AxisSystem) to be rotated.
    * @param iutState  the output state, in this AxisSystem, the result 
    *                  of rotating the input inState.
    * @param j2000Body the origin of the input MJ2000EqAxes frame.
    *
    * @return success or failure of the operation.
    */
   //---------------------------------------------------------------------------
   virtual bool RotateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
                                   Rvector &outState, SpacePoint *j2000Body) = 0;
   
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   //virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase - may need these later??
   //virtual std::string     GetParameterText(const Integer id) const;     
   //virtual Integer         GetParameterID(const std::string &str) const; 
   //virtual Gmat::ParameterType
   //                        GetParameterType(const Integer id) const;
   //virtual std::string     GetParameterTypeString(const Integer id) const;
   
   // currently, no access to RotMatrix and RotDotMatrix allowed
   
protected:

   enum
   {
      AxisSystemParamCount = CoordinateBaseParamCount,
      //ROTATION_MATRIX = CoordinateBaseParamCount,
      //ROTATION_DOT_MATRIX,
      //AxisSystemParamCount
   };
   
   //static const std::string PARAMETER_TEXT[AxisSystemParamCount - CoordinateBaseParamCount];
   
   //static const Gmat::ParameterType PARAMETER_TYPE[AxisSystemParamCount - CoordinateBaseParamCount];
   
   //---------------------------------------------------------------------------
   //  void CalculateRotationMatrix(SpacePoint *j2000Body)
   //---------------------------------------------------------------------------
   /**
    * This method will compute the rotMatrix and rotDotMatrix used for rotations
    * from/to this AxisSystem to/from the MJ2000EqAxes system.
    *
    * @param j2000Body the origin of the MJ2000EqAxes frame.
    */
   //---------------------------------------------------------------------------
   virtual void CalculateRotationMatrix() = 0;
   
   /// rotation matrix - 
   /// default constructor creates a 3x3 zero-matrix
   Rmatrix33   rotMatrix;
   /// derivative of rotation matrix - 
   /// default constructor creates a 3x3 zero-matrix
   Rmatrix33   rotDotMatrix;
   
};
#endif // AxisSystem_hpp
