//$Header: /cygdrive/p/dev/cvs/test/TestCoordSystem/TestCoord3.cpp,v 1.2 2005/05/06 19:08:16 wshoan Exp $
//------------------------------------------------------------------------------
//                                  TestCoord
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2004/12/27
//
/**
 * Test driver for coordinate system stuff
 * Testing MJ2000Eq and BodyFixed for non-Earth bodies
 */
//------------------------------------------------------------------------------
#include <map>
#include <iostream>
#include <string>
#include <list>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystemFactory.hpp"
#include "AxisSystem.hpp"
#include "SolarSystem.hpp"
#include "SpacePoint.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "Moon.hpp"
#include "Planet.hpp"
#include "A1Mjd.hpp"
#include "SolarSystemException.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "Linear.hpp"
#include "MessageWindow.hpp"
#include "GmatBaseException.hpp"
#include "Spacecraft.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateSystemException.hpp"
#include "CoordinateConverter.hpp"
#include "MJ2000EqAxes.hpp"
#include "MJ2000EcAxes.hpp"
#include "BodyFixedAxes.hpp"
#include "EquatorAxes.hpp"
#include "SlpFile.hpp"
#include "DeFile.hpp"
#include "TimeSystemConverter.hpp"
#include "LeapSecsFileReader.hpp"


using namespace std;

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string eopFileName    = "/GMAT/dev/datafiles/EOPFiles/eopc04.62-now";
   std::string nutFileName    = "/GMAT/dev/datafiles/ITRF/NUTATION.DAT";
   std::string planFileName   = "/GMAT/dev/datafiles/ITRF/NUT85.DAT";
   std::string SLPFileName    = "/GMAT/dev/datafiles/mac/DBS_mn2000.dat";
   std::string DEFileName     = "/GMAT/dev/datafiles/DEascii/macp1941.405";
   std::string LeapFileName   = "/GMAT/dev/datafiles/tai-utcFiles/tai-utc.dat";
   //std::string eopFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/eop/eopc04.62-now";
   //std::string nutFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf/NUTATION.DAT";
   //std::string planFileName   = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf//NUT85.DAT";
   //std::string SLPFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/slp/DBS_mn2000.dat";
   //std::string DEFileName     = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/de/macp1941.405";
   //std::string LeapFileName   = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/time/tai-utc.dat";
   
   cout << "=-=-=-=-=-=-= TEST coordinate system (3) ....." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout.setf(ios::fixed);
   cout.precision(16);

   
   cout << "\n==> First, test the AxisSystemFactory <==" << endl;
   AxisSystemFactory *asf = new AxisSystemFactory();
   cout << "AxisSystemFactory created .........." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   std::string
      AXES_TYPE[13] =
   {
      "MJ2000Eq",
      "MJ2000Ec",
      "TOEEq",
      "TOEEc",
      "MOEEq",
      "MOEEc",
      "TODEq",
      "TODEc",
      "MODEq",
      "MODEc",
      "ObjectReferenced",
      "Equator",
      "BodyFixed",
   };
   
   AxisSystem*   as;
   MJ2000EqAxes* mj;
   MJ2000EqAxes* mjMars;
   BodyFixedAxes* bf;
   EquatorAxes*   eq1;
   mj = (MJ2000EqAxes*)  asf->CreateAxisSystem("MJ2000Eq", "MJ1");
   mjMars = (MJ2000EqAxes*)  asf->CreateAxisSystem("MJ2000Eq", "MJ1");
   bf = (BodyFixedAxes*) asf->CreateAxisSystem("BodyFixed", "BF1");
   eq1 = (EquatorAxes*) asf->CreateAxisSystem("Equator", "EQ1");

   try
   {
      LeapSecsFileReader* ls     = new LeapSecsFileReader(LeapFileName);
      ls->Initialize();
      EopFile *eop               = new EopFile(eopFileName);
      eop->Initialize();
      ItrfCoefficientsFile* itrf = new ItrfCoefficientsFile(nutFileName, planFileName);
      itrf->Initialize();
      bf->SetEopFile(eop);
      bf->SetCoefficientsFile(itrf);
      TimeConverterUtil::SetLeapSecsFileReader(ls);
      TimeConverterUtil::SetEopFile(eop);
   }
   catch (BaseException &bbee)
   {
      cout << "ERROR !!!!! " << bbee.GetMessage() << endl;
   }

   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   if (mj)
   {
      cout << "MJ2000EqAxes object exists with name " << mj->GetName() << 
      " and type " << mj->GetTypeName() << endl;
   }
   if (mjMars)
   {
      cout << "MJ2000EqAxes object exists with name " << mjMars->GetName() << 
      " and type " << mjMars->GetTypeName() << endl;
   }
   if (bf)
   {
      cout << "BodyFixed object exists with name " << bf->GetName() << 
      " and type " << bf->GetTypeName() << endl;
   }
   if (eq1)
   {
      cout << "Equator object exists with name " << eq1->GetName() << 
      " and type " << eq1->GetTypeName() << endl;
   }
   
   SolarSystem*   ss;
   Star*          sol;
   Planet*        earth;
   Planet*        mars;
   Planet*        jupiter;
   Moon*          luna;
   std::string    j2000BN = "";
   SpacePoint*    j2000B  = NULL;
   try
   {
      cout << "\n==> Create the solar system <==" << endl;
      
      ss              = new SolarSystem("TheSS");
      cout << "solar system name = " << ss->GetName() << endl;
      cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;

      earth = (Planet*) ss->GetBody(SolarSystem::EARTH_NAME);
      cout << "earth name = " << earth->GetName() << endl;
      cout << "earth's type name is " << earth->GetTypeName() << endl;
      
      sol = (Star*) ss->GetBody(SolarSystem::SUN_NAME);
      cout << "sol name = " << sol->GetName() << endl;
      cout << "sol's type name is " << sol->GetTypeName() << endl;
      cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;

      luna = (Moon*) ss->GetBody(SolarSystem::MOON_NAME);
      cout << "luna name = " << luna->GetName() << endl;
      cout << "luna's type name is " << luna->GetTypeName() << endl;

      mars = (Planet*) ss->GetBody(SolarSystem::MARS_NAME);
      cout << "mars name = " << mars->GetName() << endl;
      cout << "mars's type name is " << mars->GetTypeName() << endl;

      jupiter = (Planet*) ss->GetBody(SolarSystem::JUPITER_NAME);
      cout << "jupiter name = " << jupiter->GetName() << endl;
      cout << "jupiter's type name is " << jupiter->GetTypeName() << endl;

      SlpFile* anSLP;
      DeFile*  aDE;
      try
      {
         anSLP          = new SlpFile(SLPFileName);
         aDE            = new DeFile(Gmat::DE405,DEFileName);
         cout << "the SLP file is : " << anSLP->GetName() << endl;
         cout << "the DE file is : " << aDE->GetName() << endl;
      }
      catch (BaseException &be1)
      {
         cout << "ERROR with ephem file -> " << be1.GetMessage() << endl;
      }

      ss->SetSource(Gmat::SLP);
      ss->SetSourceFile(anSLP);
      //ss->SetSource(Gmat::DE_405);
      //ss->SetSourceFile(aDE);

      // set the j2000Body
      //j2000BN = "Earth";
      //j2000B  = earth;
      j2000BN = "Mars";
      j2000B  = mars;

      sol->SetJ2000BodyName(j2000BN);
      sol->SetJ2000Body(j2000B);
      earth->SetJ2000BodyName(j2000BN);
      earth->SetJ2000Body(j2000B);
      luna->SetJ2000BodyName(j2000BN);
      luna->SetJ2000Body(j2000B);
      mars->SetJ2000BodyName(j2000BN);
      mars->SetJ2000Body(j2000B);
      jupiter->SetJ2000BodyName(j2000BN);
      jupiter->SetJ2000Body(j2000B);
      
   }
   catch (GmatBaseException &ex)
   {
      cout << "Some kind of error ..........." << endl;
   }
   
   bool isOK = false;
   cout << "\n==> Now creating CoordinateSystems <==" << endl;
   CoordinateSystem* mj2000 = new CoordinateSystem("", "CoordSystemMJ2000");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   mj2000->SetSolarSystem(ss);
   mj2000->SetStringParameter("Origin","Mars"); 
   mj2000->SetStringParameter("J2000Body",j2000BN); 
   isOK = mj2000->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = mj2000->SetRefObject(mars,Gmat::SPACE_POINT,"Mars");
   isOK = mj2000->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = mj2000->SetRefObject(mj, Gmat::AXIS_SYSTEM, mj->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   mj2000->Initialize();
   

   CoordinateSystem* mj2000Mars = new CoordinateSystem("", "CoordSystemMJ2000Mars");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   mj2000Mars->SetSolarSystem(ss);
   mj2000Mars->SetStringParameter("Origin","Mars"); 
   mj2000Mars->SetStringParameter("J2000Body",j2000BN); 
   isOK = mj2000Mars->SetRefObject(mars,Gmat::SPACE_POINT,"Mars");
   isOK = mj2000Mars->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = mj2000Mars->SetRefObject(mjMars, Gmat::AXIS_SYSTEM, mjMars->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   mj2000Mars->Initialize();
   
   CoordinateSystem* bfCS = new CoordinateSystem("", "CoordSystemBodyFixed");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   bfCS->SetSolarSystem(ss);
   bfCS->SetStringParameter("Origin",SolarSystem::MARS_NAME); 
   bfCS->SetStringParameter("J2000Body",j2000BN);
   isOK = bfCS->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = bfCS->SetRefObject(mars,Gmat::SPACE_POINT,"Mars");
   //isOK = bfCS->SetRefObject(sol,Gmat::SPACE_POINT,SolarSystem::SUN_NAME);
   //isOK = mj20002->SetRefObject(mars,Gmat::SPACE_POINT,"Earth");
   isOK = bfCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = bfCS->SetRefObject(bf, Gmat::AXIS_SYSTEM, bf->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   bfCS->Initialize();
   
   CoordinateSystem* eqCS = new CoordinateSystem("", "CoordSystemEquator");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   eqCS->SetSolarSystem(ss);
   eqCS->SetStringParameter("Origin",SolarSystem::MARS_NAME); 
   eqCS->SetStringParameter("J2000Body",j2000BN);
   isOK = eqCS->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = eqCS->SetRefObject(mars,Gmat::SPACE_POINT,"Mars");
   //isOK = bfCS->SetRefObject(sol,Gmat::SPACE_POINT,SolarSystem::SUN_NAME);
   //isOK = mj20002->SetRefObject(mars,Gmat::SPACE_POINT,"Earth");
   isOK = eqCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = eqCS->SetRefObject(eq1, Gmat::AXIS_SYSTEM, eq1->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   eqCS->Initialize();
   /*
   try
   {
      cout << "Now checking CoordinateSystems' AxisSystems ..." << endl;
      GmatBase *mjObj = mj2000->GetRefObject(Gmat::AXIS_SYSTEM, mj->GetName());
      if (mjObj) 
      {
         cout << "mj2000 has a pointer to the axis system " << mjObj->GetName() << 
         " of type " << mjObj->GetTypeName() << endl;
      }
      else       cout << "ERROR -  no pointer to the axis system ????????" << endl;
      GmatBase *bfObj2 = bfCS->GetRefObject(Gmat::AXIS_SYSTEM, bf->GetName());
      if (bfObj2) 
      {
         cout << "mj2000ec has a pointer to the axis system " << bfObj2->GetName() << 
         " of type " << bfObj2->GetTypeName() << endl;
      }
      else       cout << "ERROR -  no pointer to the axis system ????????" << endl;
   }
   catch (BaseException &be)
   {
      cout << "ERROR ------------- : " << be.GetMessage() << endl;
   }
    */
   
   CoordinateConverter *cc = new CoordinateConverter();
   //cc->SetJ2000BodyName("Earth");
   //cc->SetJ2000Body(earth);

   //Rvector6 theState(19999.99999999997,0.0,0.0,
    //                 0.0, 4.23521190349, 4.23521190349);
   //Rvector6 theState(15999.99999999998,0.0,0.0,
   //                  0.0, 3.8662018270519716, 3.8662018270519711);
   //Rvector6 theState(5102.5096,6123.01152,6378.1363,
   //                  -4.74321960,  0.79053660,  5.53375619);
   Rvector6 theState(-6821.47284360074,1261.20410114993,775.40938920649,
                    -0.43821296011, -2.43981897400, 0.09021855328);
   Rvector6 outState;
   
   A1Mjd atTime1;
   
   Real ut1_utc = -0.5036517;  // 2005/01/01
   //Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(2005,1,1,12,0,ut1_utc));
   //Real ut1_utc = 0.4025210;  // 1991/04/06
   //Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(1991,4,6,7,51,28.39+ut1_utc));
   //Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(1991,4,6,7,51,28.39));
   Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(1991,4,6,7,51,28.39));
   A1Mjd testTime(atTime);
   cout << "The test time is " << testTime.Get() << endl;
   cout << "The test Rvector6 is " << theState << endl;
   
   cout << "About to try to convert from Mars-Centered MJ2000Eq to Mars-Centered MJ2000Eq" << endl;
   
   try
   {
      cc->Convert(testTime,theState, mj2000, outState, mj2000Mars);
      cout << "The output state is : " << outState << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   
   cout << "About to try to convert from Mars-Centered MJ2000Eq to Mars-Centered Equator" << endl;
   
   try
   {
      cc->Convert(testTime,theState, mj2000, outState, eqCS);
      cout << "The output state is : " << outState << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }

   cout << "About to try to convert from Mars-Centered MJ2000Eq to Mars-Centered BodyFixed" << endl;
   
   try
   {
      cc->Convert(testTime,theState, mj2000, outState, bfCS);
      cout << "The output state is : " << outState << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   
   cout << "Now delete SolarSystem ............." << endl;
   delete ss;
   cout << "Now delete CoordinateConverter ............." << endl;
   delete cc;
   cout << "Now delete CoordinateSystem (and its AxisSystem) ............." << endl;
   delete mj2000;
   cout << "Now delete CoordinateSystem (and its AxisSystem) ............." << endl;
   delete mj2000Mars;
   cout << "Now delete other CoordinateSystem (and its AxisSystem) ............." << endl;
   delete bfCS;
   cout << "Now delete other CoordinateSystem (and its AxisSystem) ............." << endl;
   delete eqCS;
   //cout << "Now delete AxisSystem ............." << endl;
   //delete mj;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "Now delete everything else ............." << endl;
   delete asf;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "=-=-=-=-=-=-= END TEST coordinate system ....." << endl;

}
