//$Id: Ionosphere.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         Ionosphere Model
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Tuan Dang Nguyen
// Created: 2010/06/21
//
/**
 * IRI 2007 ionosphere media correction model.
 */
//------------------------------------------------------------------------------

#include "Ionosphere.hpp"
#include "GmatConstants.hpp"
//#include "math.h"
//#include "f2c.h"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"

#define DEBUG_IONOSPHERE_ELECT_DENSITY
#define DEBUG_IONOSPHERE_TEC

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Real Ionosphere::NUM_OF_INTERVALS = 20;

//------------------------------------------------------------------------------
// Ionosphere(const std::string& nomme)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
//------------------------------------------------------------------------------
Ionosphere::Ionosphere(const std::string &nomme):
   MediaCorrection("Ionosphere", nomme)
{
   objectTypeNames.push_back("Ionosphere");
   model = 2;                 // 2 for IRI2007 ionosphere model
   
   waveLength = 0.0;          // wave length of the signal
   epoch = 0.0;               // time
   yyyy = 0;                  // year
   mmdd = 0;                  // month and day
   hours = 0.0;               // hours
}


//------------------------------------------------------------------------------
// ~Ionosphere()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Ionosphere::~Ionosphere()
{
}


//------------------------------------------------------------------------------
// Ionosphere(const Ionosphere& ions)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
Ionosphere::Ionosphere(const Ionosphere& ions):
   MediaCorrection(ions),
   waveLength (ions.waveLength),
   epoch      (ions.epoch),
   yyyy       (ions.yyyy),
   mmdd       (ions.mmdd),
   hours      (ions.hours)
{
   stationLoc    = ions.stationLoc;
   spacecraftLoc = ions.spacecraftLoc;
}


//-----------------------------------------------------------------------------
// Ionosphere& operator=(const Ionosphere& ions)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param tps The Troposphere that is provides parameters for this one
 *
 * @return This Troposphere, configured to match tps
 */
//-----------------------------------------------------------------------------
Ionosphere& Ionosphere::operator=(const Ionosphere& ions)
{
   if (this != &ions)
   {
      MediaCorrection::operator=(ions);
      
      waveLength      = ions.waveLength;
      epoch           = ions.epoch;
      yyyy            = ions.yyyy;
      mmdd            = ions.mmdd;
      hours           = ions.hours;
      stationLoc      = ions.stationLoc;
      spacecraftLoc   = ions.spacecraftLoc;
   }
   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone()
//------------------------------------------------------------------------------
/**
 * Clone a Ionosphere object
 */
//------------------------------------------------------------------------------
GmatBase* Ionosphere::Clone() const
{
   return new Ionosphere(*this);
}


//------------------------------------------------------------------------------
// bool SetWaveLength(Real lambda)
//------------------------------------------------------------------------------
/**
 * Set wave length
 * @param lambda  The wave length
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetWaveLength(Real lambda)
{
   waveLength = lambda;
   return true;
}


//------------------------------------------------------------------------------
// bool SetTime(GmatEpoch ep)
//------------------------------------------------------------------------------
/**
 * Set time
 * @param ep  The time
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetTime(GmatEpoch ep)
{
   epoch = ep;
   Real temp;
   std::string time;
   TimeConverterUtil::Convert("A1ModJulian", epoch, "", "UTCGregorian", temp, time, 2);
   yyyy = atoi(time.substr(0,4).c_str());
   mmdd = atoi(time.substr(5,2).c_str())*100 + atoi(time.substr(8,2).c_str());
   hours = atof(time.substr(11,2).c_str()) + atof(time.substr(14,2).c_str())/60 +
      atof(time.substr(17,2).c_str())/3600 + atof(time.substr(20,3).c_str())/3600000.0;
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetStationPosition(Rvector3 p)
//------------------------------------------------------------------------------
/**
 * Set station position
 * @param p  Position of station. (unit: km)
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetStationPosition(Rvector3 p)
{
   stationLoc = p;
   return true;
}


//------------------------------------------------------------------------------
// bool SetSpacecraftPosition(Rvector3 p)
//------------------------------------------------------------------------------
/**
 * Set spacecraft position
 * @param p  Position of spacecraft. (unit: km)
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetSpacecraftPosition(Rvector3 p)
{
   spacecraftLoc = p;
   return true;
}


//------------------------------------------------------------------------------
// bool SetEarthRadius(Real r)
//------------------------------------------------------------------------------
/**
 * Set earth radius
 * @param r  radius of earth. (unit: km)
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetEarthRadius(Real r)
{
   earthRadius= r;
   return true;
}


//---------------------------------------------------------------------------
// float ElectronDensity(Rvector3 pos2, Rvector3 pos1)
//---------------------------------------------------------------------------
/**
 * This function is used to calculate average electron density along
 * 2 positions.
 *
 * @ param pos1 the first position in Earth fixed coordinate system (unit: km)
 * @ param pos2 the second position in Earth fixed coordinate system (unit: km)
 *
 * return value is electron density (unit: number electrons per m3)
 *
 */
//---------------------------------------------------------------------------
float Ionosphere::ElectronDensity(Rvector3 pos2, Rvector3 pos1)
{
   // the fisrt position's latitude and longitude (unit: degree):
   //real latitude = (real)(GmatMathConstants::PI_OVER_TWO_DEG - acos(pos1.Get(2)/pos1.GetMagnitude())*GmatMathConstants::DEG_PER_RAD);	// unit: degree
   real latitude = (real)(asin(pos1.Get(2)/pos1.GetMagnitude())*GmatMathConstants::DEG_PER_RAD);	// unit: degree
   real longitude = (real)(atan2(pos1.Get(1),pos1.Get(0))*GmatMathConstants::DEG_PER_RAD);			// unit: degree
   
   // mmag  = 0 geographic   =1 geomagnetic coordinates
   integer jmag = 0;	// 1;
   
   // jf(1:30)     =.true./.false. flags; explained in IRISUB.FOR
   logical jf[31];
   for (int i=1; i <= 30; ++i)
      jf[i] = TRUE_;
   
   jf[5] = FALSE_;
   jf[6] = FALSE_;
   jf[23] = FALSE_;
   jf[29] = FALSE_;
   jf[30] = FALSE_;
   
//   jf[21] = FALSE_;
//   jf[28] = FALSE_;
   
   // iy,md        date as yyyy and mmdd (or -ddd)
   // hour         decimal hours LT (or UT+25)
   integer iy = (integer)yyyy;
   integer md = (integer)mmdd;
   real hour = (real)hours;
   
   // Upper and lower integration limits
   real hbeg = (real)(pos1.GetMagnitude() - earthRadius); // 0
   real hend = hbeg;
   real hstp = 1.0;
   
   integer error = 0;

   real outf[20*501+1];
   real oarr[51];


//   iri_sub(&jf[1], &jmag, &latitude, &longitude, &iy, &md, &hour,
//           &hbeg, &hend, &hstp, &outf[21], &oarr[1], &error);

   integer ivar = 1;		// get attitude result
   integer iut = 1;			// 1 for universal time; 0 for local time

# ifdef DEBUG_IONOSPHERE_ELECT_DENSITY
   MessageInterface::ShowMessage("           .At time = %lf A1Mjd:",epoch);
   MessageInterface::ShowMessage("         year = %d   md = %d   hour = %lf h,   time type = %s,\n", iy, md, hour, (iut?"Universal":"Local"));
   MessageInterface::ShowMessage("              At position (x,y,z) = (%lf,  %lf,  %lf)km in Earth fixed coordinate system: ", pos1[0], pos1[1], pos1[2]);
   MessageInterface::ShowMessage("(latitude = %lf degree,  longitude = %lf degree,  attitude = %lf km,  ", latitude, longitude, hbeg);
   MessageInterface::ShowMessage("coordinate system type = %s)\n",(jmag?"Geomagetic":"Geographic"));
#endif
   iri_web(&jmag, &jf[1], &latitude, &longitude, &iy, &md, &iut, &hour, &hbeg, &hbeg, 
	   &ivar, &hbeg, &hend, &hstp, &outf[21], &oarr[1], &error);

   if (error != 0)
   {
      MessageInterface::ShowMessage("Ionosphere data files not found\n");
      throw new MeasurementException("Ionosphere data files not found\n");
   }

   real density = outf[20+1];
   if (density < 0)
      density = 0;

#ifdef DEBUG_IONOSPHERE_ELECT_DENSITY
   MessageInterface::ShowMessage("              Electron density at that time and location = %le electrons per m3.\n", density);
#endif

   return density;         //*(pos2-pos1).GetMagnitude();
}

/*
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
float Ionosphere::ElectronDensity1(Rvector3 pos2, Rvector3 pos1)
{
   real latitude = (real)(90.0 - acos(pos1.Get(2)/pos1.GetMagnitude())*180.0/acos(-1.0));
   real longitude = (real)(atan2(pos1.Get(1),pos1.Get(0))*180.0/acos(-1.0));
   
   // mmag  = 0 geographic   =1 geomagnetic coordinates
   integer jmag = 1;
   
   // jf(1:30)     =.true./.false. flags; explained in IRISUB.FOR
   logical jf[31];
   for (int i=1; i <= 30; ++i)
      jf[i] = TRUE_;
   
   jf[5] = FALSE_;
   jf[6] = FALSE_;
   jf[23] = FALSE_;
   jf[29] = FALSE_;
   jf[30] = FALSE_;
   
   // iy,md        date as yyyy and mmdd (or -ddd)
   // hour         decimal hours LT (or UT+25)
   integer iy = (integer)yyyy;
   integer md = (integer)mmdd;
   real hour = (real)hours;
   
   // Upper and lower integration limits
   real hbeg = (real)(pos1.GetMagnitude() - earthRadius);          // 0
   real hend = (real)(pos2.GetMagnitude() - earthRadius);          // 1500;
   
   real tec = 0;
   real tecb = 0;
   real tect = 0;
   integer error = 0;

   irit13(&latitude, &longitude, &jmag, &jf[1], &iy, &md, &hour,
          &hbeg, &hend, &tec, &tecb, &tect, &error);
   
   if (error != 0)
   {
      MessageInterface::ShowMessage("Ionosphere data files not found\n");
      throw new GmatBaseException("Ionosphere data files not found\n");
   }
   
   MessageInterface::ShowMessage("density = %.12lf\n", tec/((hend-hbeg)*1000));
   return (tec /((hend-hbeg)*1000));

}
*/


//---------------------------------------------------------------------------
// Real Ionosphere::TEC()
// This function is used to calculate number of electron inside a 1 meter 
// square cross sectioncylinder with its bases on spacecraft and on ground 
// station.
//
//  return value: tec  (unit: number of electorns per 1 meter square)
//---------------------------------------------------------------------------
Real Ionosphere::TEC()
{
#ifdef DEBUG_IONOSPHERE_TEC
	MessageInterface::ShowMessage("         It performs calculation electron density along the path\n");
	MessageInterface::ShowMessage("            from ground station location: (%lf,  %lf,  %lf)km\n", stationLoc[0], stationLoc[1], stationLoc[2]);
	MessageInterface::ShowMessage("            to spacecraft location:       (%lf,  %lf,  %lf)km\n", spacecraftLoc[0], spacecraftLoc[1], spacecraftLoc[2]);
#endif
   Rvector3 dR = (spacecraftLoc - stationLoc) / NUM_OF_INTERVALS;
   Rvector3 p1 = stationLoc;
   Rvector3 p2;
   Real electdensity, ds;
   Real tec = 0.0;
   for(int i = 0; i < NUM_OF_INTERVALS; ++i)
   {
      p2 = p1 + dR;
	  electdensity = ElectronDensity(p2, p1);					// unit: electron per m-3
	  ds = (p2-p1).GetMagnitude()*GmatMathConstants::KM_TO_M;	// unit: m
      tec += electdensity*ds;
      p1 = p2;
   }
   
   return tec;
}


//---------------------------------------------------------------------------
// Real Ionosphere::BendingAngle()
//---------------------------------------------------------------------------
Real Ionosphere::BendingAngle()
{
   Rvector3 rangeVec = spacecraftLoc - stationLoc;
   Rvector3 dR = rangeVec / NUM_OF_INTERVALS;
   Rvector3 p1 = stationLoc;
   Rvector3 p2, delta;
   Real n1, n2, dn_drho, de1, de2, integrant;
   Real gammar = 0.0;
   
   //Real beta0 = GmatConstants::PI/2 - acos(rangeVec.GetUnitVector()*p1.GetUnitVector());
   Real beta0 = GmatMathConstants::PI_OVER_TWO - acos(rangeVec.GetUnitVector()*p1.GetUnitVector());
   //MessageInterface::ShowMessage("Elevation angle = %f\n", beta0*180/GmatConstants::PI);
   MessageInterface::ShowMessage("Elevation angle = %f\n", beta0*GmatMathConstants::DEG_PER_RAD);
   Real beta = beta0;
   Real freq = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / waveLength;
   for(int i = 0; i < NUM_OF_INTERVALS; ++i)
   {
      p2 = p1 + dR;
      
      delta = dR/100;
      de1 = ElectronDensity(p1+delta, p1);
      de2 = ElectronDensity(p1+2*delta,p1+delta);
      n1 = 1 - 40.3*de1/(freq*freq);
      n2 = 1 - 40.3*de2/(freq*freq);
      dn_drho = -40.3*(de2 - de1)/ (freq*freq) / (((p1+delta).GetMagnitude() - p1.GetMagnitude())*GmatMathConstants::KM_TO_M);
      integrant = dn_drho/(n1*tan(beta));
      gammar += integrant*(p2.GetMagnitude() - p1.GetMagnitude())*GmatMathConstants::KM_TO_M;
      //MessageInterface::ShowMessage("de1 = %.12lf,  de2 = %.12lf, rho1 = %f,   "
      //"rho2 = %f, integrant = %.12lf, gammar =%.12lf\n",de1, de2, p1.GetMagnitude(),
      //p2.GetMagnitude(), integrant, gammar*180/GmatConstants::PI);
      
      p1 = p2;
      beta = beta0 + gammar;
   }
   
   return gammar;
}


//---------------------------------------------------------------------------
// RealArray Ionosphere::Correction()
// This function is used to calculate Ionosphere correction
// Return values:
//    . Range correction (unit: m)
//    . Angle correction (unit: radian)
//    . Time correction  (unit: s)
//---------------------------------------------------------------------------
RealArray Ionosphere::Correction()
{
   Real freq = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / waveLength;
   Real tec = TEC();                  // Equation 6.70 of MONTENBRUCK and GILL
   Real drho = 40.3*tec/(freq*freq);  // Equation 6.69 of MONTENBRUCK and GILL		// unit: meter
   Real dphi = 0;                     //BendingAngle()*180/GmatConstants::PI;
                                      // It has not been defined yet
   Real dtime = drho/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;				// unit: s
   
   //MessageInterface::ShowMessage
   //   ("Ionosphere::Correction: freq = %.12lf MHz,  tec = %.12lfe16,  "
   //    "drho = %.12lfm, dphi = %f, dtime = %.12lfs\n", freq/1.0e6,
   //    tec/1.0e16, drho, dphi*3600, dtime);
   RealArray ra;
   ra.push_back(drho);
   ra.push_back(dphi);
   ra.push_back(dtime);
   
   return ra;
}


// IRI_SUB:
extern "C" int iri_sub__(logical *jf, integer *jmag, real *alati, real *along,
                integer *iyyyy, integer *mmdd, real *dhour, real *heibeg, real *heiend,
                real *heistp, real *outf, real *oarr, integer *ier);
int Ionosphere::iri_sub(logical *jf, integer *jmag, real *alati, real *
        along, integer *iyyyy, integer *mmdd, real *dhour, real *heibeg, real
        *heiend, real *heistp, real *outf, real *oarr, integer *ier)
{
        return iri_sub__(jf, jmag, alati, along, iyyyy, mmdd, dhour, heibeg,
                        heiend, heistp, outf, oarr, ier);
}

extern "C" int iri_web__(integer *jmag, logical *jf, real *alati, real *
        along, integer *iyyyy, integer *mmdd, integer *iut, real *dhour, real
        *height, real *h_tec_max__, integer *ivar, real *vbeg, real *vend,
        real *vstp, real *a, real *b, integer *ier);
int Ionosphere::iri_web(integer *jmag, logical *jf, real *alati, real *
        along, integer *iyyyy, integer *mmdd, integer *iut, real *dhour, real
        *height, real *h_tec_max__, integer *ivar, real *vbeg, real *vend,
        real *vstp, real *a, real *b, integer *ier)
{
        return iri_web__(jmag, jf, alati, along, iyyyy, mmdd, iut, dhour, height,
                        h_tec_max__, ivar, vbeg, vend, vstp, a, b, ier);
}


// IRI_TEC:
extern "C" int irit13_(real *alati, real *along, integer *jmag, logical
                *jf, integer *iy, integer *md, real *hour, real *hbeg, real *hend,
                real *tec, real *tecb, real *tect, integer *ier);
extern "C" doublereal ioncorr_(real *tec, real *f);
extern "C" int iri_tec__(real *hstart, real *hend, integer *istep, real
                *tectot, real *tectop, real *tecbot);

int Ionosphere::irit13(real *alati, real *along, integer *jmag, logical
        *jf, integer *iy, integer *md, real *hour, real *hbeg, real *hend,
        real *tec, real *tecb, real *tect, integer *ier)
{
        return irit13_(alati, along, jmag, jf, iy, md, hour, hbeg, hend,
                tec, tecb, tect, ier);
}

doublereal Ionosphere::ioncorr(real *tec, real *f)
{
    return ioncorr_(tec, f);
}

int Ionosphere::iri_tec(real *hstart, real *hend, integer *istep, real
        *tectot, real *tectop, real *tecbot)
{
    return iri_tec__(hstart, hend, istep, tectot, tectop, tecbot);
}


// IRI_FUN:
extern "C" int initialize_();
int Ionosphere:: initialize_irifun()
{
    return initialize_();
} /* initialize_irifun */

extern "C" doublereal xe1_(real *h);
doublereal Ionosphere::xe1(real *h)
{
    return xe1_(h);
} /* xe1 */

extern "C" doublereal topq_(real *h, real *no, real *hmax, real *ho);
doublereal Ionosphere::topq(real *h, real *no, real *hmax, real *ho)
{
    return topq_(h, no, hmax, ho);
} /* topq */

extern "C" doublereal zero_(real *delta);
doublereal Ionosphere::zero(real *delta)
{
    return zero_(delta);
} /* zero */


extern "C" doublereal dxe1n_(real *h);
doublereal Ionosphere::dxe1n(real *h)
{
    return dxe1n_(h);
} /* dxe1n */


extern "C" doublereal xe2_(real *h);
doublereal Ionosphere::xe2(real *h)
{
    return xe2(h);
} /* xe2 */

extern "C" doublereal xe3_1__(real *h);
doublereal Ionosphere::xe3_1(real *h)
{
    return xe3_1__(h);
} /* xe3_1*/


extern "C" doublereal xe4_1__(real *h);
doublereal Ionosphere::xe4_1(real *h)
{
    return xe4_1__(h);
} /* xe4_1 */

extern "C" doublereal xe5_(real *h);
doublereal Ionosphere::xe5(real *h)
{
    return xe5_(h);
} /* xe5 */


extern "C" doublereal xe6_(real *h);
doublereal Ionosphere::xe6(real *h)
{
    return xe6_(h);
} /* xe6 */

extern "C" doublereal xe_1__(real *h);
doublereal Ionosphere::xe_1(real *h)
{
    return xe_1__(h);
} /* xe_1 */


extern "C" int calne_(integer *crd, real *invdip, real *fl, real *dimo,
                real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *f107,
                real *nne);
int Ionosphere::calne(integer *crd, real *invdip, real *fl, real *dimo,
        real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *f107,
        real *nne)
{
    return calne_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, f107, nne);
} /* calne */

extern "C" int nelow_(integer *crd, real *invdip, real *fl, real *dimo,
                real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d,
                real *nne);
int Ionosphere::nelow(integer *crd, real *invdip, real *fl, real *dimo,
        real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d,
        real *nne)
{
    return nelow_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, d, nne);
} /* nelow */


extern "C" int nehigh_(integer *crd, real *invdip, real *fl, real *dimo,
                 real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d,
                real *nne);
int Ionosphere::nehigh(integer *crd, real *invdip, real *fl, real *dimo,
         real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d,
         real *nne)
{
    return nehigh_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, d, nne);
} /* nehigh */


extern "C" int elteik_(integer *crd, integer *f107y, integer *seasy,
                real *invdip, real *fl, real *dimo, real *b0, real *dipl, real *mlt,
                real *alt, integer *ddd, real *f107, real *te, real *sigte);
int Ionosphere::elteik(integer *crd, integer *f107y, integer *seasy,
        real *invdip, real *fl, real *dimo, real *b0, real *dipl, real *mlt,
        real *alt, integer *ddd, real *f107, real *te, real *sigte)
{
    return elteik_(crd, f107y, seasy, invdip, fl, dimo, b0, dipl, mlt,
                alt, ddd, f107, te, sigte);
} /* elteik */

extern "C" int spharm_ik__(real *c, integer *l, integer *m, real *colat, real *az);
int Ionosphere::spharm_ik(real *c, integer *l, integer *m, real *colat, real *az)
{
    return spharm_ik__(c, l, m, colat, az);
} /* spharm_ik */


extern "C" int teba_(real *dipl, real *slt, integer *ns, real *te);
int Ionosphere::teba(real *dipl, real *slt, integer *ns, real *te)
{
    return teba_(dipl, slt, ns, te);
} /* teba */


extern "C" int spharm_(real *c, integer *l, integer *m, real *colat, real *az);
int Ionosphere::spharm(real *c, integer *l, integer *m, real *colat, real *az)
{
    return spharm_(c, l, m, colat, az);
} /* spharm */

extern "C" doublereal elte_(real *h);
doublereal Ionosphere::elte(real *h)
{
    return elte_(h);
} /* elte */


extern "C" doublereal tede_(real *h, real *den, real *cov);
doublereal Ionosphere::tede(real *h, real *den, real *cov)
{
    return tede_(h, den, cov);
} /* tede */

extern "C" doublereal ti_(real *h);
doublereal Ionosphere::ti(real *h)
{
    return ti_(h);
} /* ti */


extern "C" doublereal teder_(real *h);
doublereal Ionosphere::teder(real *h)
{
    return teder_(h);
} /* teder */


extern "C" doublereal tn_(real *h, real *tinf, real *tlbd, real *s);
doublereal Ionosphere::tn(real *h, real *tinf, real *tlbd, real *s)
{
    return tn_(h, tinf, tlbd, s);
} /* tn */

extern "C" doublereal dtndh_(real *h, real *tinf, real *tlbd, real *s);
doublereal Ionosphere::dtndh(real *h, real *tinf, real *tlbd, real *s)
{
    return dtndh_(h, tinf, tlbd, s);
} /* dtndh */

extern "C" doublereal rpid_(real *h, real *h0, real *n0, integer *m,
                real *st, integer *id, real *xs);
doublereal Ionosphere::rpid(real *h, real *h0, real *n0, integer *m, real *st,
                integer *id, real *xs)
{
    return rpid_(h, h0, n0, m, st, id, xs);
} /* rpid */


extern "C" int rdhhe_(real *h, real *hb, real *rdoh, real *rdo2h,
                real *rno, real *pehe, real *rdh, real *rdhe);
int Ionosphere::rdhhe(real *h, real *hb, real *rdoh, real *rdo2h,
        real *rno, real *pehe, real *rdh, real *rdhe)
{
    return rdhhe_(h, hb, rdoh, rdo2h, rno, pehe, rdh, rdhe);
} /* rdhhe */

extern "C" doublereal rdno_(real *h, real *hb, real *rdo2h, real *rdoh, real *rno);
doublereal Ionosphere::rdno(real *h, real *hb, real *rdo2h, real *rdoh, real *rno)
{
    return rdno_(h, hb, rdo2h, rdoh, rno);
} /* rdno */


extern "C" int koefp1_(real *pg1o);
int Ionosphere::koefp1(real *pg1o)
{
    return koefp1_(pg1o);
} /* koefp1 */


extern "C" int koefp2_(real *pg2o);
int Ionosphere::koefp2(real *pg2o)
{
    return koefp2_(pg2o);
} /* koefp2 */

extern "C" int koefp3_(real *pg3o);
int Ionosphere::koefp3(real *pg3o)
{
    return koefp3_(pg3o);
} /* koefp3 */


extern "C" int sufe_(real *field, real *rfe, integer *m, real *fe);
int Ionosphere::sufe(real *field, real *rfe, integer *m, real *fe)
{
    return sufe_(field, rfe, m, fe);
} /* sufe */

extern "C" int ionco2_(real *hei, real *xhi, integer *it, real *f, real
                *r1, real *r2, real *r3, real *r4);
int Ionosphere::ionco2(real *hei, real *xhi, integer *it, real *f, real
        *r1, real *r2, real *r3, real *r4)
{
    return ionco2_(hei, xhi, it, f, r1, r2, r3, r4);
} /* ionco2 */

extern "C" int aprok_(integer *j1m, integer *j2m, real *h1, real *h2,
                real *r1m, real *r2m, real *rk1m, real *rk2m, real *hei, real *xhi,
                real *r1, real *r2);
int Ionosphere::aprok(integer *j1m, integer *j2m, real *h1, real *h2,
        real *r1m, real *r2m, real *rk1m, real *rk2m, real *hei, real *xhi,
        real *r1, real *r2)
{
    return aprok_(j1m, j2m, h1, h2,     r1m, r2m, rk1m, rk2m, hei, xhi,
                r1, r2);
} /* aprok */


extern "C" int ioncomp_(real *xy, integer *id, integer *ismo, real *xm,
                real *hx, real *zd, real *fd, real *fp, real *fs, real *dion);
int Ionosphere::ioncomp(real *xy, integer *id, integer *ismo, real *xm,
        real *hx, real *zd, real *fd, real *fp, real *fs, real *dion)
{
    return ioncomp_(xy, id, ismo, xm, hx, zd, fd, fp, fs, dion);
} /* ioncomp */

extern "C" int ionco1_(real *h, real *zd, real *fd, real *fs, real *t, real *cn);
int Ionosphere::ionco1(real *h, real *zd, real *fd, real *fs, real *t, real *cn)
{
    return ionco1_(h, zd, fd, fs, t, cn);
} /* ionco1 */

extern "C" int calion_(integer *crd, real *invdip, real *fl, real *dimo,
                 real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *f107,
                 real *no, real *nh, real *nhe, real *nn);
int Ionosphere::calion(integer *crd, real *invdip, real *fl, real *dimo,
         real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *f107,
         real *no, real *nh, real *nhe, real *nn)
{
    return calion_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, f107,
                 no, nh, nhe, nn);
} /* calion */

extern "C" int ionlow_(integer *crd, real *invdip, real *fl, real *dimo,
                 real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d,
                integer *ion, real *nion);
int Ionosphere::ionlow(integer *crd, real *invdip, real *fl, real *dimo,
         real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d,
        integer *ion, real *nion)
{
    return ionlow_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, d,
                 ion, nion);
} /* ionlow */

extern "C" int ionhigh_(integer *crd, real *invdip, real *fl, real *
                dimo, real *b0, real *dipl, real *mlt, real *alt, integer *ddd,
                real *d, integer *ion, real *nion);
int Ionosphere::ionhigh(integer *crd, real *invdip, real *fl, real *dimo, real *b0,
                real *dipl, real *mlt, real *alt, integer *ddd, real *d,
                integer *ion, real *nion)
{
    return ionhigh_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd,
                d, ion, nion);
} /* ionhigh */

extern "C" doublereal invdpc_(real *fl, real *dimo, real *b0, real *dipl, real *dtor);
doublereal Ionosphere::invdpc(real *fl, real *dimo, real *b0, real *dipl, real *dtor)
{
    return invdpc_(fl, dimo, b0, dipl, dtor);
} /* invdpc */

extern "C" doublereal fout_(real *xmodip, real *xlati, real *xlongi, real *ut, real *ff0);
doublereal Ionosphere::fout(real *xmodip, real *xlati, real *xlongi, real *ut, real *ff0)
{
    return fout_(xmodip, xlati, xlongi, ut, ff0);
} /* fout */


extern "C" doublereal xmout_(real *xmodip, real *xlati, real *xlongi, real *ut, real *xm0);
doublereal Ionosphere::xmout(real *xmodip, real *xlati, real *xlongi, real *ut, real *xm0)
{
    return xmout_(xmodip, xlati, xlongi, ut, xm0);
} /* xmout */

extern "C" doublereal hmf2ed_(real *xmagbr, real *r, real *x, real *xm3);
doublereal Ionosphere::hmf2ed(real *xmagbr, real *r, real *x, real *xm3)
{
    return hmf2ed_(xmagbr, r, x, xm3);
} /* hmf2ed */

extern "C" doublereal xm3000hm_(real *xmagbr, real *r, real *x, real *hmf2);
doublereal Ionosphere::xm3000hm(real *xmagbr, real *r, real *x, real *hmf2)
{
    return xm3000hm_(xmagbr, r, x, hmf2);
} /* xm3000hm */

extern "C" doublereal fof1ed_(real *ylati, real *r, real *chi);
doublereal Ionosphere::fof1ed(real *ylati, real *r, real *chi)
{
    return fof1ed_(ylati, r, chi);
} /* fof1ed */

extern "C" doublereal f1_c1__(real *xmodip, real *hour, real *suxnon, real *saxnon);
doublereal Ionosphere::f1_c1(real *xmodip, real *hour, real *suxnon, real *saxnon)
{
    return f1_c1__(xmodip, hour, suxnon, saxnon);
} /* f1_c1 */

extern "C" int f1_prob__(real *sza, real *glat, real *rz12, real *f1prob, real *f1probl);
int Ionosphere::f1_prob(real *sza, real *glat, real *rz12, real *f1prob, real *f1probl)
{
    return f1_prob__(sza, glat, rz12, f1prob, f1probl);
} /* f1_prob */

extern "C" doublereal foeedi_(real *cov, real *xhi, real *xhim, real *xlati);
doublereal Ionosphere::foeedi(real *cov, real *xhi, real *xhim, real *xlati)
{
    return foeedi_(cov, xhi, xhim, xlati);
} /* foeedi */


extern "C" doublereal xmded_(real *xhi, real *r, real *yw);
doublereal Ionosphere::xmded(real *xhi, real *r, real *yw)
{
    return xmded_(xhi, r, yw);
} /* xmded */

extern "C" doublereal gamma1_(real *smodip, real *slat, real *slong, real *hour,
                integer *iharm, integer *nq, integer *k1, integer *m, integer *mm,
                integer *m3, real *sfe);
doublereal Ionosphere::gamma1(real *smodip, real *slat, real *slong,
        real *hour, integer *iharm, integer *nq, integer *k1, integer *m,
        integer *mm, integer *m3, real *sfe)
{
    return gamma1_(smodip, slat, slong, hour, iharm, nq, k1, m, mm, m3, sfe);
} /* gamma1 */

extern "C" doublereal b0_98__(real *hour, real *sax, real *sux, integer *nseasn,
                real *r, real *zlo, real *zmodip);
doublereal Ionosphere::b0_98(real *hour, real *sax, real *sux, integer *nseasn,
                real *r, real *zlo, real *zmodip)
{
    return b0_98__(hour, sax, sux, nseasn, r, zlo, zmodip);
} /* b0_98 */

extern "C" int tal_(real *shabr, real *sdelta, real *shbr, real *sdtdh0,
                 logical *aus6, real *spt);
int Ionosphere::tal(real *shabr, real *sdelta, real *shbr, real *sdtdh0,
         logical *aus6, real *spt)
{
    return tal_(shabr, sdelta, shbr, sdtdh0, aus6, spt);
} /* tal */

extern "C" int valgul_(real *xhi, real *hvb, real *vwu, real *vwa, real *vdp);
int Ionosphere::valgul(real *xhi, real *hvb, real *vwu, real *vwa, real *vdp)
{
    return valgul_(xhi, hvb, vwu, vwa, vdp);
} /* valgul */

extern "C" int ggm_(integer *art, real *long__, real *lati, real *mlong, real *mlat);
int Ionosphere::ggm(integer *art, real *long__, real *lati, real *mlong, real *mlat)
{
    return ggm_(art, long__, lati, mlong, mlat);
} /* ggm */

extern "C" int fieldg_(real *dlat, real *dlong, real *alt, real *x,
                real *y, real *z, real *f, real *dip, real *dec, real *smodip);
int Ionosphere::fieldg(real *dlat, real *dlong, real *alt, real *x,
        real *y, real *z, real *f, real *dip, real *dec, real *smodip)
{
    return fieldg_(dlat, dlong, alt, x, y, z, f, dip, dec, smodip);
} /* fieldg */

extern "C" int regfa1_(real *x11, real *x22, real *fx11, real *fx22,
                real *eps, real *fw, E_fp f, logical *schalt, real *x);
int Ionosphere::regfa1(real *x11, real *x22, real *fx11, real *fx22,
        real *eps, real *fw, E_fp f, logical *schalt, real *x)
{
    return regfa1_(x11, x22, fx11, fx22, eps, fw, f, schalt, x);
} /* regfa1 */

extern "C" int soco_(integer *ld, real *t, real *flat, real *elon,
                real *height, real *declin, real *zenith, real *sunrse, real *sunset);
int Ionosphere::soco(integer *ld, real *t, real *flat, real *elon,
                real *height, real *declin, real *zenith, real *sunrse, real *sunset)
{
    return soco_(ld, t, flat, elon, height, declin, zenith, sunrse, sunset);
} /* soco */

extern "C" doublereal hpol_(real *hour, real *tw, real *xnw, real *sa, real *su,
                real *dsa, real *dsu);
doublereal Ionosphere::hpol(real *hour, real *tw, real *xnw, real *sa, real *su,
        real *dsa, real *dsu)
{
    return hpol_(hour, tw, xnw, sa, su, dsa, dsu);
} /* hpol */

extern "C" int moda_(integer *in, integer *iyear, integer *month,
                integer *iday, integer *idoy, integer *nrdaymo);
int Ionosphere::moda(integer *in, integer *iyear, integer *month,
        integer *iday, integer *idoy, integer *nrdaymo)
{
    return moda_(in, iyear, month, iday, idoy, nrdaymo);
} /* moda */

extern "C" int ut_lt__(integer *mode, real *ut, real *slt, real *glong,
                integer *iyyy, integer *ddd);
int Ionosphere::ut_lt(integer *mode, real *ut, real *slt, real *glong,
        integer *iyyy, integer *ddd)
{
    return ut_lt__(mode, ut, slt, glong, iyyy, ddd);
} /* ut_lt */

extern "C" doublereal rlay_(real *x, real *xm, real *sc, real *hx);
doublereal Ionosphere::rlay(real *x, real *xm, real *sc, real *hx)
{
    return rlay_(x, xm, sc, hx);
} /* rlay */

extern "C" doublereal d1lay_(real *x, real *xm, real *sc, real *hx);
doublereal Ionosphere::d1lay(real *x, real *xm, real *sc, real *hx)
{
    return d1lay_(x, xm, sc, hx);
} /* d1lay */

extern "C" doublereal d2lay_(real *x, real *xm, real *sc, real *hx);
doublereal Ionosphere::d2lay(real *x, real *xm, real *sc, real *hx)
{
    return d2lay_(x, xm, sc, hx);
} /* d2lay */

extern "C" doublereal eptr_(real *x, real *sc, real *hx);
doublereal Ionosphere::eptr(real *x, real *sc, real *hx)
{
    return eptr_(x, sc, hx);
} /* eptr */

extern "C" doublereal epst_(real *x, real *sc, real *hx);
doublereal Ionosphere::epst(real *x, real *sc, real *hx)
{
    return epst_(x, sc, hx);
} /* epst */

extern "C" doublereal epstep_(real *y2, real *y1, real *sc, real *hx, real *x);
doublereal Ionosphere::epstep(real *y2, real *y1, real *sc, real *hx, real *x)
{
    return epstep_(y2, y1, sc, hx, x);
} /* epstep */

extern "C" doublereal epla_(real *x, real *sc, real *hx);
doublereal Ionosphere::epla(real *x, real *sc, real *hx)
{
    return epla_(x, sc, hx);
} /* epla */

extern "C" doublereal xe2to5_(real *h, real *hmf2, integer *nl, real *hx, real *sc,
                real *amp);
doublereal Ionosphere::xe2to5(real *h, real *hmf2, integer *nl, real *hx, real *sc,
        real *amp)
{
    return xe2to5_(h, hmf2, nl, hx, sc, amp);
} /* xe2to5 */

extern "C" doublereal xen_(real *h, real *hmf2, real *xnmf2, real *hme,
                integer *nl, real *hx, real *sc, real *amp);
doublereal Ionosphere::xen(real *h, real *hmf2, real *xnmf2, real *hme, integer *nl,
        real *hx, real *sc, real *amp)
{
    return xen_(h, hmf2, xnmf2, hme, nl, hx, sc, amp);
} /* xen */

extern "C" int rogul_(integer *iday, real *xhi, real *sx, real *gro);
int Ionosphere::rogul(integer *iday, real *xhi, real *sx, real *gro)
{
    return rogul_(iday, xhi, sx, gro);
} /* rogul */

extern "C" int lnglsn_(integer *n, real *a, real *b, logical *aus);
int Ionosphere::lnglsn(integer *n, real *a, real *b, logical *aus)
{
    return lnglsn_(n, a, b, aus);
} /* lnglsn */

extern "C" int lsknm_(integer *n, integer *m, integer *m0, integer *m1,
                real *hm, real *sc, real *hx, real *w, real *x, real *y, real *var,
                logical *sing);
int Ionosphere::lsknm(integer *n, integer *m, integer *m0, integer *m1,
        real *hm, real *sc, real *hx, real *w, real *x, real *y, real *var,
        logical *sing)
{
    return lsknm_(n, m, m0, m1, hm, sc, hx, w, x, y, var, sing);
} /* lsknm */

extern "C" int inilay_(logical *night, logical *f1reg, real *xnmf2,
                real *xnmf1, real *xnme, real *vne, real *hmf2, real *hmf1, real *hme,
                 real *hv1, real *hv2, real *hhalf, real *hxl, real *scl, real *amp,
                integer *iqual);
int Ionosphere::inilay(logical *night, logical *f1reg, real *xnmf2,
        real *xnmf1, real *xnme, real *vne, real *hmf2, real *hmf1, real *hme,
        real *hv1, real *hv2, real *hhalf, real *hxl, real *scl, real *amp,
        integer *iqual)
{
    return inilay_(night, f1reg, xnmf2, xnmf1, xnme, vne, hmf2, hmf1, hme,
                hv1, hv2, hhalf, hxl, scl, amp, iqual);
} /* inilay */

extern "C" int tcon_(integer *yr, integer *mm, integer *day, integer *idn,
                real *rz, real *ig, real *rsn, integer *nmonth);
int Ionosphere::tcon(integer *yr, integer *mm, integer *day, integer *idn,
        real *rz, real *ig, real *rsn, integer *nmonth)
{
    return tcon_(yr, mm, day, idn, rz, ig, rsn, nmonth);
} /* tcon */

extern "C" int lstid_(real *fi, integer *icez, real *r, real *ae,
                real *tm, real *sax, real *sux, real *ts70, real *df0f2, real *dhf2);
int Ionosphere::lstid(real *fi, integer *icez, real *r, real *ae,
        real *tm, real *sax, real *sux, real *ts70, real *df0f2, real *dhf2)
{
    return lstid_(fi, icez, r, ae, tm, sax, sux, ts70, df0f2, dhf2);
} /* lstid */

extern "C" int apf_(integer *iyyyy, integer *imn, integer *id, real *hour,
                integer *iap);
int Ionosphere::apf(integer *iyyyy, integer *imn, integer *id, real *hour,
        integer *iap)
{
    return apf_(iyyyy, imn, id, hour, iap);
} /* apf */

extern "C" int apf_only__(integer *iyyyy, integer *imn, integer *id,
                real *f107d, real *f107m);
int Ionosphere::apf_only(integer *iyyyy, integer *imn, integer *id,
        real *f107d, real *f107m)
{
    return apf_only__(iyyyy, imn, id, f107d, f107m);
} /* apf_only */

extern "C" int conver_(real *rga, real *rgo, real *rgma);
int Ionosphere::conver(real *rga, real *rgo, real *rgma)
{
    return conver_(rga, rgo, rgma);
} /* conver */

extern "C" int storm_(integer *ap, real *rga, real *rgo, integer *coor,
                real *rgma, integer *ut, integer *doy, real *cf);
int Ionosphere::storm(integer *ap, real *rga, real *rgo, integer *coor,
        real *rgma, integer *ut, integer *doy, real *cf)
{
    return storm_(ap, rga, rgo, coor, rgma, ut, doy, cf);
} /* storm */

extern "C" int vdrift_(real *xt, real *xl, real *param, real *y);
int Ionosphere::vdrift(real *xt, real *xl, real *param, real *y)
{
    return vdrift_(xt, xl, param, y);
} /* vdrift */

extern "C" doublereal bspl4_time__(integer *i, real *x1);
doublereal Ionosphere::bspl4_time(integer *i, real *x1)
{
    return bspl4_time__(i, x1);
} /* bspl4_time */

extern "C" doublereal bspl4_long__(integer *i, real *x1);
doublereal Ionosphere::bspl4_long(integer *i, real *x1)
{
    return bspl4_long__(i, x1);
} /* bspl4_long */

extern "C" int g_(real *param, real *funct, real *x);
int Ionosphere::g(real *param, real *funct, real *x)
{
    return g_(param, funct, x);
} /* g */

extern "C" int stormvd_(integer *flag, integer *ip, real *ae, real *slt,
                real *promptvd, real *dynamovd, real *vd);
int Ionosphere::stormvd(integer *flag, integer *ip, real *ae, real *slt,
        real *promptvd, real *dynamovd, real *vd)
{
    return stormvd_(flag, ip, ae, slt, promptvd, dynamovd, vd);
} /* stormvd */

extern "C" doublereal bspl4_ptime__(integer *i, real *x1);
doublereal Ionosphere::bspl4_ptime(integer *i, real *x1)
{
    return bspl4_ptime__(i, x1);
} /* bspl4_ptime */

extern "C" int spreadf_brazil__(integer *idoy, integer *idiy, real *f107,
                real *geolat, real *osfbr);
int Ionosphere::spreadf_brazil(integer *idoy, integer *idiy, real *f107,
        real *geolat, real *osfbr)
{
    return spreadf_brazil__(idoy, idiy, f107, geolat, osfbr);
} /* spreadf_brazil */

extern "C" doublereal bspl4t_(integer *i, real *t1);
doublereal Ionosphere::bspl4t(integer *i, real *t1)
{
    return bspl4t_(i, t1);

} /* bspl4t */

extern "C" doublereal bspl2s_(integer *i, real *t1);
doublereal Ionosphere::bspl2s(integer *i, real *t1)
{
    return bspl2s_(i, t1);
} /* bspl2s */

extern "C" doublereal bspl2l_(integer *i, real *t1);
doublereal Ionosphere::bspl2l(integer *i, real *t1)
{
    return bspl2l_(i, t1);

} /* bspl2l */

extern "C" doublereal bspl2f_(integer *i, real *t1);
doublereal Ionosphere::bspl2f(integer *i, real *t1)
{
    return bspl2f_(i, t1);
} /* bspl2f */


// IRI_DREG:
extern "C" int dregion_(real *z, integer *it, real *f, real *vkp,
        real *f5sw, real *f6wa, real *elg);

int Ionosphere::dregion(real *z, integer *it, real *f, real *vkp,
        real *f5sw, real *f6wa, real *elg)
{
        return dregion_(z, it, f, vkp, f5sw, f6wa, elg);
} /* dregion */

extern "C" int f00_(real *hgt, real *glat1, integer *iday, real *zang,
                real *f107t, real *edens, integer *ierror);
int Ionosphere::f00(real *hgt, real *glat1, integer *iday, real *zang,
        real *f107t, real *edens, integer *ierror)
{
        return f00_(hgt, glat1, iday, zang, f107t, edens, ierror);
} /* f00 */


//IGRF:
extern "C" int igrf_sub__(real *xlat, real *xlong, real *year, real *height,
                real *xl, integer *icode, real *dipl, real *babs, integer *ier);
int Ionosphere::igrf_sub(real *xlat, real *xlong, real *year, real *height,
        real *xl, integer *icode, real *dipl, real *babs, integer *ier)
{
    return igrf_sub__(xlat, xlong, year, height, xl, icode, dipl, babs, ier);
} /* igrf_sub */

extern "C" int igrf_dip__(real *xlat, real *xlong, real *year, real *height,
                real *dip, real *dipl, real *ymodip, integer *ier);
int Ionosphere::igrf_dip(real *xlat, real *xlong, real *year, real *height,
        real *dip, real *dipl, real *ymodip, integer *ier)
{
    return igrf_dip__(xlat, xlong, year, height, dip, dipl, ymodip, ier);
} /* igrf_dip */

extern "C" int findb0_(real *stps, real *bdel, logical *value, real *bequ, real *rr0);
int Ionosphere::findb0(real *stps, real *bdel, logical *value, real *bequ, real *rr0)
{
    return findb0_(stps, bdel, value, bequ, rr0);
} /* findb0 */

extern "C" int shellg_0_(int n, real *glat, real *glon, real *alt,
                real *dimo, real *fl, integer *icode, real *b0, real *v);
int Ionosphere::shellg_0(int n, real *glat, real *glon, real *alt,
        real *dimo, real *fl, integer *icode, real *b0, real *v)
{
    return shellg_0_(n, glat, glon, alt, dimo, fl, icode, b0, v);
} /* shellg_0 */

extern "C" int shellg_(real *glat, real *glon, real *alt, real *dimo,
                real *fl, integer *icode, real *b0);
int Ionosphere::shellg(real *glat, real *glon, real *alt, real *dimo,
        real *fl, integer *icode, real *b0)
{
    return shellg_(glat, glon, alt, dimo, fl, icode, b0);
} /* shellg */

extern "C" int shellc_(real *v, real *fl, real *b0);
int Ionosphere::shellc(real *v, real *fl, real *b0)
{
    return shellc_(v, fl, b0);
} /* shellc */

extern "C" int stoer_(real *p, real *bq, real *r);
int Ionosphere::stoer(real *p, real *bq, real *r)
{
    return stoer_(p, bq, r);
} /* stoer */

extern "C" int feldg_0_(int n, real *glat, real *glon, real *alt,
                real *bnorth, real *beast, real *bdown, real *babs, real *v, real *b);
int Ionosphere::feldg_0(int n, real *glat, real *glon, real *alt,
        real *bnorth, real *beast, real *bdown, real *babs, real *v, real *b)
{
    return feldg_0_(n, glat, glon, alt, bnorth, beast, bdown, babs, v, b);
} /* feldg_0 */

extern "C" int feldg_(real *glat, real *glon, real *alt, real *bnorth,
                real *beast, real *bdown, real *babs);
int Ionosphere::feldg(real *glat, real *glon, real *alt, real *bnorth,
        real *beast, real *bdown, real *babs)
{
    return feldg_(glat, glon, alt, bnorth, beast, bdown, babs);
} /* feldg */

extern "C" int feldc_(real *v, real *b);
int Ionosphere::feldc(real *v, real *b)
{
    return feldc_(v, b);
} /* feldc */

extern "C" int feldi_();
int Ionosphere::feldi()
{
    return feldi_();
} /* feldi */

extern "C" int feldcof_(real *year, real *dimo, integer *ier);
int Ionosphere::feldcof(real *year, real *dimo, integer *ier)
{
    return feldcof_(year, dimo, ier);
} /* feldcof */

extern "C" int getshc_(integer *iu, char *fspec, integer *nmax, real *erad,
                real *gh, integer *ier, ftnlen fspec_len);
int Ionosphere::getshc(integer *iu, char *fspec, integer *nmax, real *erad,
        real *gh, integer *ier, ftnlen fspec_len)
{
    return getshc_(iu, fspec, nmax, erad, gh, ier, fspec_len);
} /* getshc */

extern "C" int intershc_(real *date, real *dte1, integer *nmax1, real *gh1,
                real *dte2, integer *nmax2, real *gh2, integer *nmax, real *gh);
int Ionosphere::intershc(real *date, real *dte1, integer *nmax1, real *gh1,
        real *dte2, integer *nmax2, real *gh2, integer *nmax, real *gh)
{
    return intershc_(date, dte1, nmax1, gh1, dte2, nmax2, gh2, nmax, gh);
} /* intershc */

extern "C" int extrashc_(real *date, real *dte1, integer *nmax1,
        real *gh1, integer *nmax2, real *gh2, integer *nmax, real *gh);
int Ionosphere::extrashc(real *date, real *dte1, integer *nmax1,
        real *gh1, integer *nmax2, real *gh2, integer *nmax, real *gh)
{
    return extrashc_(date, dte1, nmax1, gh1, nmax2, gh2, nmax, gh);
} /* extrashc */

extern "C" int initize_();
int Ionosphere::initize()
{
    return initize_();
} /* initize */

extern "C" int geodip_(integer *iyr, real *sla, real *slo, real *dla,
                real *dlo, integer *j);
int Ionosphere::geodip(integer *iyr, real *sla, real *slo, real *dla,
        real *dlo, integer *j)
{
    return geodip_(iyr, sla, slo, dla, dlo, j);
} /* geodip */

extern "C" int sphcar_(real *r, real *teta, real *phi, real *x, real *y,
                real *z, integer *j);
int Ionosphere::sphcar(real *r, real *teta, real *phi, real *x, real *y,
        real *z, integer *j)
{
    return sphcar_(r, teta, phi, x, y, z, j);
} /* sphcar */

extern "C" int geomag_(real *xgeo, real *ygeo, real *zgeo, real *xmag,
                real *ymag, real *zmag, integer *j, integer *iyr);
int Ionosphere::geomag(real *xgeo, real *ygeo, real *zgeo, real *xmag,
        real *ymag, real *zmag, integer *j, integer *iyr)
{
    return geomag_(xgeo, ygeo, zgeo, xmag, ymag, zmag, j, iyr);
} /* geomag */

extern "C" int recalc_(integer *iyr, integer *iday, integer *ihour,
                integer *min, integer *isec);
int Ionosphere::recalc(integer *iyr, integer *iday, integer *ihour,
        integer *min, integer *isec)
{
    return recalc_(iyr, iday, ihour, min, isec);
} /* recalc */



// CIRA:
extern "C" int cira86_(integer *iday, real *sec, real *glat, real *glong,
        real *stl, real *f107a, real *tinf, real *tlb, real *sigma);
int Ionosphere::cira86(integer *iday, real *sec, real *glat, real *glong,
        real *stl, real *f107a, real *tinf, real *tlb, real *sigma)
{
        return cira86_(iday, sec, glat, glong, stl, f107a, tinf, tlb, sigma);
} /* cira86 */
