//$Header$
//------------------------------------------------------------------------------
//                             CoordUtil
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Modified:
//   2004/04/19 Linda Jun - Modified Swnigby code to reuse for GMAT project
//
/**
 * Impements conversion functions between Cartesian and Keplerian elements.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "CoordUtil.hpp"
#include "RealUtilities.hpp"     // for Sqrt(), IsZero()
#include "UtilityException.hpp"
#include "MessageInterface.hpp"
#include <sstream>               // for stringstream
#include <math.h>

using namespace GmatMathUtil;


const Real CoordUtil::ORBIT_TOL = 1.0E-10;
const Real CoordUtil::ORBIT_TOL_SQ = 1.0E-20;

//---------------------------------
// public static functions
//---------------------------------

//------------------------------------------------------------------------------
// static bool IsRvValid(Real r[3], Real v[3])
//------------------------------------------------------------------------------
bool CoordUtil::IsRvValid(Real r[3], Real v[3])
{
   if ( ((r[0]*r[0] + r[1]*r[1] + r[2]*r[2]) > CoordUtil::ORBIT_TOL_SQ) &&
        ((v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) > CoordUtil::ORBIT_TOL_SQ) )
      return true;
   else
      return false;
   
}

//------------------------------------------------------------------------------
// static Real MeanToTrueAnomaly(Real ma, Real ecc, Real tol = 1.0e-08)
//------------------------------------------------------------------------------
Real CoordUtil::MeanToTrueAnomaly(Real ma, Real ecc, Real tol)
{
   Real ta;
   Integer iter;
   Integer ret;
   
   ret = CoordUtil::ComputeMeanToTrueAnomaly(ma, ecc, tol, &ta, &iter);

   if (ret > 0 )
      throw UtilityException("CoordUtil::MeanToTrueAnomaly() Error converting "
                             " Mean Anomaly to True Anomaly\n");
   else
      return ta;
}

//------------------------------------------------------------------------------
// static Integer ComputeMeanToTrueAnomaly(Real ma, Real ecc, Real tol,
//                                         Real *ta, Integer *iter)
//------------------------------------------------------------------------------
/*
 * @param <ecc>  input eccentricity
 * @param <ma>   input elliptical or hyperbolic mean anomaly
 * @param <tol>  input tolerance for accuracy
 * @param <ta>   output true anomaly (deg)
 * @param <iter> output number of iterations
 */
//------------------------------------------------------------------------------
Integer CoordUtil::ComputeMeanToTrueAnomaly(Real ma, Real ecc, Real tol,
                                            Real *ta, Integer *iter)
{
   Real temp, temp2;
   Real rm,   e,     e1,  e2,  c,  f,  f1,   f2,    g;
   Real ztol = 1.0e-30;
   int done;

   rm = DegToRad(Mod(ma, 360.0));
   *iter = 0;

   if (ecc <= 1.0)
   {
      //---------------------------------------------------------
      // elliptical orbit
      //---------------------------------------------------------
      
      e2 = rm + ecc * Sin(rm);
      done = 0;

      while (!done)
      {
         *iter = *iter + 1;
         temp = 1.0 - ecc * Cos(e2);

         if (Abs(temp) < ztol)
            return (3);

         e1 = e2 - (e2 - ecc * Sin(e2) - rm)/temp;

         if (Abs(e2-e1) < tol)
         {
            done = 1;
            e2 = e1;
         }

         if (!done)
         {
            *iter = *iter + 1;
            temp = 1.0 - ecc * Cos(e1);

            if (Abs(temp) < ztol)
               return (4);

            e2 = e1 - (e1 - ecc * Sin(e1) - rm)/temp;

            if( Abs(e1-e2) < tol)
               done = 1;
         }
      }

      e = e2;

      if (e < 0.0)
         e = e + TWO_PI;

      c = Abs(RadToDeg(e) - 180.0);

      if (c >= 1.0e-08) 
      {
         temp  = 1.0 - ecc;

         if (Abs(temp)< ztol)
            return (5);

         temp2 = (1.0 + ecc)/temp;

         if (temp2 <0.0)
            return (6);

         f = Sqrt(temp2);
         g = Tan(e/2.0);
         *ta = 2.0 * RadToDeg(ATan(f*g));

      }
      else
      {
         *ta = RadToDeg(e);
      }

      if( *ta < 0.0)
         *ta = *ta + 360.0;

   }
   else
   {
      //---------------------------------------------------------
      // hyperbolic orbit
      //---------------------------------------------------------

      if (rm > PI)
         rm = rm - TWO_PI;

      f2 = ecc * Sinh(rm) - rm;
      done = 0;

      while (!done)
      {
         *iter = *iter + 1;
         temp = ecc * Cosh(f2) - 1.0;

         if (Abs(temp) < ztol)
            return (7);
 
         f1 = f2 - (ecc * Sinh(f2) - f2 - rm) / temp;

         if (Abs(f2-f1) < tol)
         {
            done = 1;
            f2 = f1;
         } 

         if (!done)
         {
            *iter = *iter + 1; 
            temp = ecc * Cosh(f1) - 1.0;

            if (Abs(temp) < ztol)
               return (8);

            f2 = f1 - (ecc * Sinh(f1) - f1 - rm) / temp;

            if ( Abs(f1-f2) < tol)
               done = 1;
         }
         
         if (*iter > 1000)
         {
            throw UtilityException
               ("CoordUtil::ComputeMeanToTrueAnomaly() "
                "Caught in infinite loop numerical argument "
                "out of domain for sinh() and cosh()\n");
         }
      }

      f = f2;
      temp = ecc - 1.0;

      if (Abs(temp) < ztol)
         return (9);

      temp2 = (ecc + 1.0) / temp;

      if (temp2 <0.0)
         return (10);

      e = Sqrt(temp2);
      g = Tanh(f/2.0);
      *ta = 2.0 * RadToDeg(ATan(e*g));

      if (*ta < 0.0)
         *ta = *ta + 360.0;
   }

   return (0);
   
} // end ComputeMeanToTrueAnomaly()

//------------------------------------------------------------------------------
// static Integer ComputeCartToKepl(Real grav, Real r[3], Real v[3], Real *tfp,
//                                  Real elem[6], Real *ma)
//------------------------------------------------------------------------------
/*
 * @param <grav> input graviational constant
 * @param <r>    input position vector in cartesian coordinate
 * @param <v>    input velocity vector in cartesian coordinate
 * @param <tfp>  output time from periapsis
 * @param <elem> output six keplerian elements
 *               (1) = semimajor axis
 *               (2) = eccentricity
 *               (3) = inclination (deg)
 *               (4) = longitude of ascending node (deg)
 *               (5) = argument of periapsis (deg)
 *               (6) = true anomaly (deg)
 * @param <ma>   output mean anomaly (deg)
 */
//------------------------------------------------------------------------------
Integer CoordUtil::ComputeCartToKepl(Real grav, Real r[3], Real v[3], Real *tfp,
                                     Real elem[6], Real *ma)
{
   if (Abs(grav) < 1E-30)
      return(2);
   
   Rvector3 pos(r[0], r[1], r[2]);
   Rvector3 vel(v[0], v[1], v[2]);
   
   // eqn 4.1
   Rvector3 angMomentum = Cross(pos, vel);
   
   // eqn 4.2
   Real h = angMomentum.GetMagnitude();
   
   if (h < 1E-30) {
      return 1;
   }
   
   // eqn 4.3
   Rvector3 nodeVec = Cross(Rvector3(0,0,1), angMomentum);
   
   // eqn 4.4
   Real n = nodeVec.GetMagnitude();
   
   // eqn 4.5 - 4.6
   Real posMag = pos.GetMagnitude();
   Real velMag = vel.GetMagnitude();
   
   // eqn 4.7 - 4.8
   Rvector3 eccVec = (1/grav)*((velMag*velMag - grav/posMag)*pos - (pos * vel) * vel);
   Real e = eccVec.GetMagnitude();
   
   // eqn 4.9
   Real zeta = 0.5*velMag*velMag - grav/posMag;
   
   if (Abs(1 - e) < 1E-30)
   {
      throw UtilityException
         ("CoordUtil::CartesianToKeplerian() "
          "Radius is near infinite in keplarian to cartesian conversion\n");
   }
   
   // eqn 4.10
   Real sma = -grav/(2*zeta);
   
   // eqn 4.11
   Real i = ACos( angMomentum.Get(2)/h );
   
   Real raan, argPeriapsis, trueAnom;
   raan=argPeriapsis=trueAnom=0;
   if ( e >= 1E-11 && i >= 1E-11 )  // CASE 1: Non-circular, Inclined Orbit
   {
      raan = ACos( nodeVec.Get(0)/n );
      if (nodeVec.Get(1) < 0)
         raan = TWO_PI - raan;
           
      argPeriapsis = ACos( (nodeVec*eccVec)/(n*e) );
      if (eccVec.Get(2) < 0)
         argPeriapsis = TWO_PI - argPeriapsis;
        
      trueAnom = ACos( (eccVec*pos)/(e*posMag) );
      if (pos*vel < 0)
         trueAnom = TWO_PI - trueAnom;
   }
   if ( e >= 1E-11 && i < 1E-11 )  // CASE 2: Non-circular, Equatorial Orbit
   {
      raan = 0;
      argPeriapsis = ACos(eccVec.Get(0)/e);
      if (eccVec.Get(1) < 0)
         argPeriapsis = TWO_PI - argPeriapsis;
           
      trueAnom = ACos( (eccVec*pos)/(e*posMag) );
      if (pos*vel < 0)
         trueAnom = TWO_PI - trueAnom;
   }
   if ( e < 1E-11 && i >= 1E-11 )  // CASE 3: Circular, Inclined Orbit
   {
      raan = ACos( nodeVec.Get(0)/n );
      if (nodeVec.Get(1) < 0)
         raan = TWO_PI - raan;
           
      argPeriapsis = 0;
        
      trueAnom = ACos( (nodeVec*pos)/(n*posMag) );
      if (pos.Get(2) < 0)
         trueAnom = TWO_PI - trueAnom;
   }
   if ( e < 1E-11 && i < 1E-11 )  // CASE 4: Circular, Equatorial Orbit
   {
      raan = 0;
      argPeriapsis = 0;
      trueAnom = ACos(pos.Get(0)/posMag);
      if (pos.Get(1) < 0)
         trueAnom = TWO_PI - trueAnom;
   }
   
   elem[0] = sma;
   elem[1] = e;
   elem[2] = i*DEG_PER_RAD;
   elem[3] = raan*DEG_PER_RAD;
   elem[4] = argPeriapsis*DEG_PER_RAD;
   elem[5] = trueAnom*DEG_PER_RAD;
 
   return 0;  
}


//------------------------------------------------------------------------------
// static Integer ComputeKeplToCart(Real grav, Real elem[6], Real r[3],
//                           Real v[3], AnomalyType anomalyType)
//------------------------------------------------------------------------------
Integer CoordUtil::ComputeKeplToCart(Real grav, Real elem[6], Real r[3],
                                     Real v[3], CoordUtil::AnomalyType anomalyType)
{
   Real sma = elem[0],
        ecc = elem[1],
        inc = elem[2]*RAD_PER_DEG,
       raan = elem[3]*RAD_PER_DEG,
        per = elem[4]*RAD_PER_DEG,
       anom = elem[5]*RAD_PER_DEG;
   
   // **************
   // taken from old code above; necessary to avoid crash, but not in spec
   // if input keplerian anomaly is MA, convert to TA
   if (anomalyType == CoordUtil::MA)
   {
      Real ma = elem[5]; 
      
      Real ta;
      Integer iter;
      Integer ret = CoordUtil::ComputeMeanToTrueAnomaly(ma, ecc, 1E-8, &ta, &iter);
      
      if (ret > 0)
         return ret;
      else
         anom = ta * RAD_PER_DEG;
   }
   // ***************
   
   // radius near infinite
   //if (1-ecc*Cos(anom) < 1E-30) {
   if (1+ecc*Cos(anom) < 1E-30) 
   {
      MessageInterface::PopupMessage(Gmat::WARNING_,
             "Warning::Radius is near infinite in keplerian to cartesian conversion.\n");
      return 1;
   }
   
   // eqn 4.24; semilatus rectum
   Real p = sma*(1 - ecc*ecc);
   
   // orbit parabolic
   if (Abs(p) < 1E-30) 
   {
        return 2;
   }
   
   // eqn 4.25; radius
   Real rad = p/(1 + ecc*Cos(anom));
   
   // eqn 4.26 - 4.28
   r[0] = rad*(Cos(per + anom) * Cos(raan) - Cos(inc) * Sin(per + anom) * Sin(raan)),
   r[1] = rad*(Cos(per + anom) * Sin(raan) + Cos(inc) * Sin(per + anom) * Cos(raan)),
   r[2] = rad*Sin(per + anom)*Sin(inc);
                
   v[0] = Sqrt(grav/p)*(Cos(anom)+ecc)*(-Sin(per)*Cos(raan)-Cos(inc)*Sin(raan)*Cos(per))
                        - Sqrt(grav/p)*Sin(anom)*(Cos(per)*Cos(raan)-Cos(inc)*Sin(raan)*Sin(per));
                        
   v[1] = Sqrt(grav/p)*(Cos(anom)+ecc)*(-Sin(per)*Sin(raan)+Cos(inc)*Cos(raan)*Cos(per))
                        - Sqrt(grav/p)*Sin(anom)*(Cos(per)*Sin(raan)+Cos(inc)*Cos(raan)*Sin(per));
                        
   v[2] = Sqrt(grav/p)*((Cos(anom)+ecc)*Sin(inc)*Cos(per) - Sin(anom)*Sin(inc)*Sin(per));
   
   return 0;
}

//------------------------------------------------------------------------------
// static Rvector6 CartesianToKeplerian(const Rvector6 &cartVec, 
//                                      const Real grav, Anomaly &anomaly)
//------------------------------------------------------------------------------
Rvector6 CoordUtil::CartesianToKeplerian(const Rvector6 &cartVec, const Real grav, 
                                         Anomaly &anomaly)
{
    Real ma;
    Rvector6 newKepl = CartesianToKeplerian(cartVec, grav, &ma); 
    
    anomaly.SetSMA(newKepl[0]);
    anomaly.SetECC(newKepl[1]);
    anomaly.SetValue(newKepl[5]);
   
    if (anomaly.GetTypeString() == "MA")
    {
       newKepl[5] = ma;
       anomaly.SetValue(ma); 
    }
    else if (anomaly.GetTypeString() == "EA")
    {
       Anomaly tempAnomaly(newKepl[0], newKepl[1], newKepl[5], Anomaly::TA);
       newKepl[5] = tempAnomaly.GetEccentricAnomaly();
       anomaly.SetValue(newKepl[5]); 
    }
    
    return newKepl;
}

//------------------------------------------------------------------------------
// static Rvector6 CartesianToKeplerian(const Rvector6 &cartVec, Real grav,
//                                      Real *ma);
//------------------------------------------------------------------------------
Rvector6 CoordUtil::CartesianToKeplerian(const Rvector6 &cartVec, Real grav, Real *ma)
{
   Real kepl[6];
   Real r[3];
   Real v[3];
   Real tfp;
   Integer  ret;
   Integer  errorCode;

   for (int i=0; i<6; i++)
      kepl[i] = 0.0;
   
   if(grav < 1.0)
   {
      throw UtilityException("CoordUtil::CartesianToKeplerian() gravity constant "
                             "too small for conversion to Keplerian elements\n");
   }
   else
   {
      cartVec.GetR(r);
      cartVec.GetV(v);

      //MessageInterface::ShowMessage("CoordUtil::CartesianToKeplerian() r=%f, %f, %f "
      //                              "v=%f, %f, %f\n", r[0], r[1], r[2], v[0], v[1], v[2]);
      
      if (CoordUtil::IsRvValid(r,v))
      {
         errorCode = CoordUtil::ComputeCartToKepl(grav, r, v, &tfp, kepl, ma);

         switch (errorCode)
         {
         case 0: // no error
            ret = 1;
            break;
         case 2:
            throw UtilityException
               ("CoordUtil::CartesianToKeplerian() "
                "Gravity constant too small for conversion to Keplerian elements\n");
         default:
            throw UtilityException
               ("CoordUtil::CartesianToKeplerian() "
                "Unable to convert Cartesian elements to Keplerian\n");
         }
      }
      else
      {
         std::stringstream ss;
         ss << cartVec;
         throw UtilityException
            ("CoordUtil::CartesianToKeplerian() Invalid Cartesian elements:\n" +
             ss.str());
      }
   }

   Rvector6 keplVec = Rvector6(kepl[0], kepl[1], kepl[2], kepl[3], kepl[4], kepl[5]);
   return keplVec;

}


//------------------------------------------------------------------------------
// static Rvector6 KeplerianToCartesian(const Rvector6 &keplVec, 
//                                      const Real grav, Anomaly anomaly)
//------------------------------------------------------------------------------
Rvector6 CoordUtil::KeplerianToCartesian(const Rvector6 &keplVec, 
                                         const Real grav, Anomaly anomaly)
{
   if (anomaly.GetTypeString() == "EA" || anomaly.GetTypeString() == "HA")
   {
      Rvector6 temp = keplVec;
      temp[5] = anomaly.GetTrueAnomaly();
      return KeplerianToCartesian(temp, grav, CoordUtil::TA);
   }
   else if (anomaly.GetTypeString() == "TA")
      return KeplerianToCartesian(keplVec, grav, CoordUtil::TA);
   
   else   //  mean anomaly
      return KeplerianToCartesian(keplVec, grav, CoordUtil::MA);
   
}

//------------------------------------------------------------------------------
// static Rvector6 KeplerianToCartesian(const Rvector6 &keplVec, Real grav,
//                                      AnomalyType anomalyType)
//------------------------------------------------------------------------------
Rvector6 CoordUtil::KeplerianToCartesian(const Rvector6 &keplVec, Real grav,
                                         CoordUtil::AnomalyType anomalyType)
{
   Integer   ret = 1;
   Real  temp_r[3];
   Real  temp_v[3];
   Real  kepl[6];
   Rvector6  cartVec;
   
   for (int i=0; i<6; i++)
      kepl[i] = keplVec[i];
   
   if(Abs(kepl[0]) <= CoordUtil::ORBIT_TOL)
   {
      // Degenerate conic... 
      ret = 0;
   }
   else if(kepl[1] < 0.0)
   {
      MessageInterface::ShowMessage
         ("CoordUtil::KeplerianToCartesian() "
          "Can't have an eccentricity less than 0.0 (%g)."
          " Will change the sign of the eccentricity.\n", kepl[1]);

      kepl[1] *= -1.0;
   }
   else if((kepl[0] > 0.0) && (kepl[1] > 1.0))
   {
      MessageInterface::ShowMessage
         ("CoordUtil::KeplerianToCartesian() "
          "Can't have a positive semimajor axis (%g)"
          " with an eccentricity greater than 1.0 (%g)."
          " Will change the sign of the semimajor axis.\n",
          kepl[0], kepl[1]);
      
      kepl[0] *= -1.0;
   }
   else if((kepl[0] < 0.0) && (kepl[1] < 1.0))
   {
      MessageInterface::ShowMessage
         ("CoordUtil::KeplerianToCartesian() "
          "Can't have a negative semimajor axis (%g)"
          " with an eccentricity less than 1.0 (%g)."
          " Will change the sign of the semimajor axis.\n",
          kepl[0], kepl[1]);

      kepl[0] *= -1.0;
      ret = 1;
   }
   else
   {
      ret = 1;
   }

   if(ret)
   {
      if(grav < 1.0)
      {
         throw UtilityException
            ("CoordUtil::KeplerianToCartesian() "
             "Gravity constant too small for conversion to Keplerian elements\n");
      }
      else if (kepl[1] == 1.0)
      {
         throw UtilityException
            ("CoordUtil::KeplerianToCartesian() "
             "Conversion of parabolic state (ecc = 1) from "
             "Keplerian to Cartesian not currently supported\n");
      }
      else
      {
         if(kepl[1] > 50.0)
         {
            MessageInterface::ShowMessage
               ("CoordUtil::KeplerianToCartesian() "
                "Probable loss of precision in conversion "
                "of hyperbolic Keplerian elements to Cartesian.\n");
         }

         // if the return code from a call to compute_kepl_to_cart is greater than zero
         if (CoordUtil::ComputeKeplToCart(grav, kepl, temp_r, temp_v, anomalyType) > 0)
         {
            throw UtilityException
               ("CoordUtil::KeplerianToCartesian() "
                "Unable to convert Keplerian elements to Cartesian\n");
         }
         else
         {
            // build the state vector
            cartVec = Rvector6(temp_r[0], temp_r[1], temp_r[2],
                               temp_v[0], temp_v[1], temp_v[2]);
         }
      }
   }

   return cartVec;
}

