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
   Real ww[3], xl[3], xnu[2], xnup[3], rm, vm;
   Real denom, c,     xnum,   p,       rd;
   
   const Real TOL = 1.0e-30;
   
   Real cta, sta,   xnupm, f,  dnul, xlm,  cw,    sw;
   Real div, cosea, sinea, ea, ava,  tang, sinhf, auxf;
   Real q,   z;
   
   long i;

   Real pow3;
   
   ww[0] = r[1] * v[2] - r[2] * v[1];
   ww[1] = r[2] * v[0] - r[0] * v[2];
   ww[2] = r[0] * v[1] - r[1] * v[0];
   
   c = Sqrt(ww[0]*ww[0] + ww[1]*ww[1] + ww[2]*ww[2]);

   if (Abs(c) < TOL)
      return(1); 

   ww[0] = ww[0] / c;
   ww[1] = ww[1] / c;
   ww[2] = ww[2] / c;

   xnu[0] = -ww[1];
   xnu[1] =  ww[0];

   xnum =Sqrt(ww[1]*ww[1] + ww[0]*ww[0]);

   if (Abs(grav) < TOL)
      return(2);

   p = c * c /(grav);
   rm = Sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
   vm = Sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
   rd = (r[0]*v[0] + r[1]*v[1] + r[2]*v[2]);

   // solve for elem[0]

   denom = 2.0 - rm * vm * vm / (grav);

   if (Abs(denom) < TOL)
      return(3);

   elem[0] = rm / denom;

   // solve for elem[1]

   if (Abs(elem[0]) < TOL)
      return (4);

   elem[1]= Sqrt(Abs(1.0 - p/elem[0]));

   // solve for elem[2]
   
   elem[2] = RadToDeg(ACos(ww[2]));
   
   // solve for elem[5]

   if (elem[1] >= 1.0e-06) 
   {
      denom = elem[1] * rm;

      if (Abs(denom) < TOL)
         return(5);

      cta = (p-rm)/denom;
      denom = elem[1] * (grav) * rm;

      if (Abs(denom) < TOL)
         return(6); 

      sta = rd * c / denom;

   }
   else
   {
      elem[1] = 0.0;

      if( Abs(elem[2]) >=  1.0e-06) 
      {
         xnup[0] = -ww[2] * ww[0];
         xnup[1] = -ww[2] * ww[1];
         xnup[2] =  ww[0]*ww[0] + ww[1]*ww[1];

         xnupm = Sqrt(xnup[0]*xnup[0] + xnup[1]*xnup[1] + xnup[2]*xnup[2]);
         denom = rm * xnum;

         if (Abs(denom) < TOL)
            return(7);

         cta = (r[0] * xnu[0] + r[1] * xnu[1])/denom;
         denom = rm * xnupm;

         if (Abs(denom) < TOL)
            return(8);

         sta = (r[0]*xnup[0] + r[1]*xnup[1] + r[2]*xnup[2]) / denom;
      }
      else
      {
         if (Abs(rm) < TOL)
            return(9);

         cta = r[0] / rm;
         sta = r[1] / rm;
      }
   }

   elem[5] = RadToDeg(ATan(sta,cta));

   if( elem[5] < 0.0)
      elem[5] = elem[5] + 360.0;

   // solve for elem[3],   if elem[2]=0, elem[3]=0

   elem[3] = 0.0;

   if(Abs(elem[2]) >= 1.0e-06)
   {
      elem[3] = RadToDeg(ATan(ww[0],-ww[1]));

      if (elem[3] < 0.0)
         elem[3] = elem[3] + 360.0;
   }
   
   // solve for elem(5), if elem(2)=0, elem(5)=0

   elem[4] = 0.0;
      
   if (elem[1] >= 1.0e-06) 
   {
      if (Abs(rm) < TOL)
         return(10); 

      f = vm * vm - (grav) / rm;

      for (i=0; i<3; i++)
         xl[i] = f * r[i] - rd * v[i];

      dnul = xnu[0] * xl[0] + xnu[1] * xl[1];
      xlm = Sqrt(xl[0]*xl[0] + xl[1]*xl[1] + xl[2]*xl[2]);

      if(Abs(elem[2]) >= 1.0e-06) 
      {
         denom = xnum * xlm;

         if (Abs(denom) < TOL)
            return (11);

         elem[4] = dnul / denom;

         if( Abs(elem[4]) >=  1.0 ) 
         {
            if( elem[4] > 0.0) 
               elem[4] = 0.0;
            else
               elem[4] = 180.0;
         }    
         else
         {
            elem[4] = RadToDeg(ACos(elem[4]));
         }


         elem[4] = (xl[2] >= 0.0) ? Abs(elem[4]) : -Abs(elem[4]);

      }
      else
      {
         if (Abs(xlm) < TOL)
            return (12);

         cw = xl[0] / xlm;
         sw = xl[1] / xlm;

         elem[4] = RadToDeg(ATan(sw,cw));
      }
   }

   if (elem[4] < 0.0)
      elem[4] = elem[4] + 360.0;

   q = DegToRad(elem[5]);
   
   pow3 = Pow(elem[0],3);
   
   if (elem[0] >= 0.0)
   {
      // solve for tfp on an elliptic orbit

      div = 1.0  + elem[1] * Cos(q);

      if (Abs(div) < TOL)
         return (13);

      cosea = (elem[1] + Cos(q)) / div;
      sinea = Sqrt(1.0 - elem[1]*elem[1]) * Sin(q)/div;
      ea = ATan(sinea,cosea);
      ava = ea - elem[1] * Sin(ea);

      if (ava < 0.0) ava = ava + TWO_PI;

      *ma = RadToDeg(ava);

      //dos:if (Abs(Pow(elem[0],3)) < TOL) 
      if (Abs(pow3) < TOL) 
         return (13);
      
      denom = Sqrt((grav)/pow3);

      if (Abs(denom) < TOL)
         return (14);

      *tfp = ava / denom;

   }
   else
   {
      // solve for tfp on a hyperbolic orbit
      denom = Cos(q/2.0);

      if (Abs(denom) < TOL)
         return (15);
                    
      tang = Sqrt((elem[1] - 1.0)/(elem[1] + 1.0)) * Sin(q/2.0)/denom;
      denom = 1.0 - tang * tang;

      if (Abs(denom) < TOL)
         return (16);

      sinhf = (2.0 * tang) / denom;
      auxf = Log(sinhf + Sqrt(sinhf * sinhf + 1.0));
      z = elem[1] * sinhf - auxf;
      *ma = RadToDeg(z);

      if (Abs(grav) < TOL)
         return (2);
      
      *tfp = Sqrt(-pow3 / (grav)) * z;
   }
   
   return (0);
}

//------------------------------------------------------------------------------
// static Integer ComputeKeplToCart(Real grav, Real elem[6], Real r[3],
//                           Real v[3], AnomalyType anomalyType)
//------------------------------------------------------------------------------
Integer CoordUtil::ComputeKeplToCart(Real grav, Real elem[6], Real r[3],
                                     Real v[3], CoordUtil::AnomalyType anomalyType)
{
   Integer ret, iter;

   Real temp, a,  b,  c,  q,  ecc, ma, ta;
   Real ci,   si, cn, sn, cw, sw,  ct, st;
   Real p,    x,  ck;
   Real rmag;
   
   Real ztol = 1.0e-30;
   Real tol = 1.0e-08;
   
   ret = 0;
   
   if (anomalyType == CoordUtil::MA) //loj: 6/23/04 changed from TA
   {
      ma = elem[5]; 
      ecc = elem[1];

      ret = CoordUtil::ComputeMeanToTrueAnomaly(ma, ecc, tol, &ta, &iter);
      
      if (ret > 0)
      {
         return ret;
      }
   }

   if (elem[2] < 0.0)
   {
      elem[2] = elem[2] + 180.0;
   }
   
   a = DegToRad(elem[2]);
   b = DegToRad(elem[3]);
   c = DegToRad(elem[4]);
   q = DegToRad(elem[5]);
   
   if (anomalyType == CoordUtil::MA) //loj: 6/23/04 changed from TA
   {
      q = DegToRad(ta);
   }
   
   if (Abs(elem[2]) < 1.e-06)
   {
      b = 0.0;
   }

   if(elem[1] < 1.e-06)
   {
      c=0.0;
   }

   ci = Cos(a);
   si = Sin(a);
   
   cn = Cos(b);
   sn = Sin(b);

   cw = Cos(c+q);
   sw = Sin(c+q);
     
   p = elem[0] * (1.0 - elem[1] * elem[1]);
   
   //    compute the radius vector and its magnitude

   temp = 1.0 + elem[1] * Cos(q);

   if (Abs(temp) < ztol)
   {
      return (1);
   }
   
   rmag = p / temp;
   
   r[0] = (rmag) * (cw * cn - ci * sw * sn);
   r[1] = (rmag) * (cw * sn + ci * sw * cn);
   r[2] = (rmag) * sw * si;
   
   cw = Cos(c);
   sw = Sin(c);

   ct = Cos(q);
   st = Sin(q);
   
   if (Abs(p) < ztol)
   {
      return (2); 
   }
   
   ck = Sqrt((grav)/p);
   x = ct + elem[1];
   
   //    compute the velocity vector and its magnitude
   Real swxcn = sw*cn;
   Real swxsn = sw*sn;
   Real cwxcn = cw*cn;
   Real cwxsn = cw*sn;
   
   Real cixsnxcw = ci*sn*cw;
   Real cixsnxsw = ci*sn*sw;
   Real cixcnxcw = ci*cn*cw;
   Real cixcnxsw = ci*cn*sw;

   Real temp0 = ck * (x * (-swxcn-cixsnxcw) - st * (cwxcn-cixsnxsw));
   Real temp1 = ck * (x * (-swxsn+cixcnxcw) - st * (cwxsn+cixcnxsw));
   Real temp2 = ck * (x * si * cw - st * si * sw);

   
//     v[0] = ck * (x * (-sw*cn-ci*sn*cw) - st * (cw*cn-ci*sn*sw));
//     v[1] = ck * (x * (-sw*sn+ci*cn*cw) - st * (cw*sn+ci*cn*sw));
//     v[2] = ck * (x * si * cw - st * si * sw);

   v[0] = temp0;
   v[1] = temp1;
   v[2] = temp2;
   
   return ret;
}

//------------------------------------------------------------------------------
// friend Rvector6 CartesianToKeplerian(const Rvector6 &cartVec, 
//                                      const Real grav, Anomaly &anomaly)
//------------------------------------------------------------------------------
Rvector6 CartesianToKeplerian(const Rvector6 &cartVec, const Real grav, 
                              Anomaly &anomaly)
{
    Real ma;
    Rvector6 newKepl = CartesianToKeplerian(cartVec, grav, &ma); 

    anomaly.SetSMA(newKepl[0]);
    anomaly.SetECC(newKepl[1]);
    anomaly.SetValue(newKepl[5]);
   
    if (anomaly.GetType() == "MA")
    {
       newKepl[5] = ma;
       anomaly.SetValue(ma); 
    }
    else if (anomaly.GetType() == "EA")
    {
        Anomaly tempAnomaly(newKepl[0],newKepl[1],newKepl[5]);
        newKepl[5] = tempAnomaly.GetEccentricAnomaly();
        anomaly.SetValue(newKepl[5]); 
    }

    return newKepl;
}

//------------------------------------------------------------------------------
// friend Rvector6 CartesianToKeplerian(const Rvector6 &cartVec, Real grav,
//                                      Real *ma);
//------------------------------------------------------------------------------
Rvector6 CartesianToKeplerian(const Rvector6 &cartVec, Real grav, Real *ma)
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
      throw UtilityException("CoordUtil::CartesianToKeplerian() ravity constant "
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

//---------------------------------
// friend functions
//---------------------------------

//------------------------------------------------------------------------------
// friend Rvector6 KeplerianToCartesian(const Rvector6 &keplVec, 
//                                      const Real grav, Anomaly anomaly)
//------------------------------------------------------------------------------
Rvector6 KeplerianToCartesian(const Rvector6 &keplVec, 
                              const Real grav, Anomaly anomaly)
{
   if (anomaly.GetType() == "EA" || anomaly.GetType() == "HA")
   {
      Rvector6 temp = keplVec;
      temp[5] = anomaly.GetTrueAnomaly();
      return KeplerianToCartesian(temp, grav, CoordUtil::TA);
   }
   else if (anomaly.GetType() == "TA")
      return KeplerianToCartesian(keplVec, grav, CoordUtil::TA);

   else   //  mean anomaly
      return KeplerianToCartesian(keplVec, grav, CoordUtil::MA);
         
}

//------------------------------------------------------------------------------
// friend Rvector6 KeplerianToCartesian(const Rvector6 &keplVec, Real grav,
//                                      AnomalyType anomalyType)
//------------------------------------------------------------------------------
Rvector6 KeplerianToCartesian(const Rvector6 &keplVec, Real grav,
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

