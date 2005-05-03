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
#include "EopFile.hpp"
#include "ItrfCoefficientsFile.hpp"

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

   virtual GmatCoordinate::ParameterUsage UsesEopFile() const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesEpoch() const;
   virtual GmatCoordinate::ParameterUsage UsesPrimary() const;
   virtual GmatCoordinate::ParameterUsage UsesSecondary() const;
   virtual GmatCoordinate::ParameterUsage UsesXAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesYAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesZAxis() const;
   // methods to set parameters for the AxisSystems
   virtual void                  SetPrimaryObject(SpacePoint *prim);
   virtual void                  SetSecondaryObject(SpacePoint *second);
   virtual void                  SetEpoch(const A1Mjd &toEpoch);
   virtual void                  SetXAxis(const std::string &toValue);
   virtual void                  SetYAxis(const std::string &toValue);
   virtual void                  SetZAxis(const std::string &toValue);
   // methods to set the files to use - for those AxisSystems that 
   // need all or part of the FK5 reduction
   virtual void                  SetEopFile(EopFile *eopF);
   virtual void                  SetCoefficientsFile(ItrfCoefficientsFile *itrfF);
   virtual void                  SetEpochFormat(const std::string &fmt);  // for GUI
   virtual SpacePoint*           GetPrimaryObject() const;
   virtual SpacePoint*           GetSecondaryObject() const;
   virtual A1Mjd                 GetEpoch() const;
   virtual std::string           GetXAxis() const;
   virtual std::string           GetYAxis() const;
   virtual std::string           GetZAxis() const;
   virtual EopFile*              GetEopFile() const;
   virtual ItrfCoefficientsFile* GetItrfCoefficientsFile();
   virtual std::string           GetEpochFormat() const; // for GUI
   
   // initializes the AxisSystem
   virtual bool Initialize();
   
   // methods to convert to/from the MJ2000 Equatorial axis system
   virtual bool RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
                                 Rvector &outState); 
   virtual bool RotateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
                                   Rvector &outState); 
   
   //---------------------------------------------------------------------------
   //  bool RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
   //                        Rvector &outState//, SpacePoint *j2000Body)
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
   //virtual bool RotateToMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
   //                              Rvector &outState) = 0; //, SpacePoint *j2000Body) = 0;

   //---------------------------------------------------------------------------
   //  bool RotateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
   //                          Rvector &outState//, SpacePoint *j2000Body)
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
   //virtual bool RotateFromMJ2000Eq(const A1Mjd &epoch, const Rvector &inState,
   //                                Rvector &outState) = 0; //, SpacePoint *j2000Body) = 0;
   
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   //virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase - may need these later??
   virtual std::string     GetParameterText(const Integer id) const;     
   virtual Integer         GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;
   virtual Real            GetRealParameter(const Integer id) const;
   virtual Real            SetRealParameter(const Integer id,
                                            const Real value);
   virtual Real            GetRealParameter(const std::string &label) const;
   virtual Real            SetRealParameter(const std::string &label,
                                            const Real value);
   
   // currently, no access to RotMatrix and RotDotMatrix allowed
   
protected:

   enum
   {
      EPOCH = CoordinateBaseParamCount,
      AxisSystemParamCount
   };
   
   static const std::string PARAMETER_TEXT[AxisSystemParamCount - CoordinateBaseParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[AxisSystemParamCount - CoordinateBaseParamCount];
   
   //---------------------------------------------------------------------------
   //  void CalculateRotationMatrix(const A1Mjd &atEpoch)
   //---------------------------------------------------------------------------
   /**
    * This method will compute the rotMatrix and rotDotMatrix used for rotations
    * from/to this AxisSystem to/from the MJ2000EqAxes system.
    *
    * @param atEpoch  epoch at which to compute the roration matrix
    */
   //---------------------------------------------------------------------------
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch) = 0;
   
   /// rotation matrix - 
   /// default constructor creates a 3x3 zero-matrix
   Rmatrix33   rotMatrix;
   /// derivative of rotation matrix - 
   /// default constructor creates a 3x3 zero-matrix
   Rmatrix33   rotDotMatrix;
   /// epoch
   A1Mjd epoch;

   // data and methods for those AxisSystems that need all or part of the FK5 
   // reduction
   static const Real  JD_OF_JANUARY_1_1997  = 2450449.5;  // correct????
   static const Real  DETERMINANT_TOLERANCE = 1.0e-15;

   EopFile                   *eop;
   ItrfCoefficientsFile      *itrf;
   
   std::string               epochFormat;
   
   std::vector<IntegerArray> a, ap;
   Rvector                   A, B, C, D, E, F, Ap, Bp, Cp, Dp;
   
   // internediate quantitied needed by more than one method
   
   virtual void      InitializeFK5();

   virtual Rmatrix33 ComputePrecessionMatrix(const Real tTDB);
   virtual Rmatrix33 ComputeNutationMatrix(const Real tTDB, Real &dPsi,
                                           Real &longAscNodeLunar,
                                           Real &cosEpsbar);
   virtual Rmatrix33 ComputeSiderealTimeRotation(const Real jdTT,
                                                 const Real tUT1,
                                                 Real dPsi,
                                                 Real longAscNodeLunar,
                                                 Real cosEpsbar,
                                                 Real &cosAst,
                                                 Real &sinAst);
   virtual Rmatrix33 ComputeSiderealTimeDotRotation(const Real mjdUTC, 
                                                    Real cosAst, Real sinAst,
                                                    Real &x, Real &y);
   virtual Rmatrix33 ComputePolarMotionRotation(Real x, Real y);
   
};
#endif // AxisSystem_hpp
