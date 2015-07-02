//$Header: /cygdrive/p/dev/cvs/test/TestForceModel/TestForcesAgain.cpp,v 1.1 2004/08/18 18:12:22 wshoan Exp $
//------------------------------------------------------------------------------
//                               TestManeuvers
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/1/14
//
/**
 * Unit test program for impulsive and (eventually) finite maneuvers. 
 */
//------------------------------------------------------------------------------


#include "ForceModel.hpp"
#include "PointMassForce.hpp"
#include "DragForce.hpp"
#include "GravityField.hpp"
#include "SolarRadiationPressure.hpp"

#include "SolarSystem.hpp"
#include "SlpFile.hpp"
#include "ConsoleAppException.hpp"
#include "Spacecraft.hpp"

#include <iostream>


void DumpObjectData(GmatBase *obj)
{
    std::cout << "  Name:  " << obj->GetName() << "\n";
    std::cout << "  Type:  " << obj->GetTypeName() << "\n";
    Integer parms = obj->GetParameterCount(), i, type;
    for (i = 0; i < parms; ++i)
    {
        std::cout << "  " << obj->GetParameterText(i) << " = ";
        type = obj->GetParameterType(i);

        switch (type) {
            case Gmat::INTEGER_TYPE:
                std::cout << obj->GetIntegerParameter(i) << "\n";   
                break;
                
            case Gmat::REAL_TYPE:
                std::cout << obj->GetRealParameter(i) << "\n";   
                break;
                
            case Gmat::STRING_TYPE:
                std::cout << obj->GetStringParameter(i) << "\n";   
                break;
                
            case Gmat::STRINGARRAY_TYPE:
                {
                   const StringArray dat = obj->GetStringArrayParameter(i);
                   std::string desc = "{";
                   for (StringArray::const_iterator i = dat.begin(); 
                        i != dat.end(); ++i)
                   {
                      if (desc != "{")
                         desc += ", ";
                      desc += (*i);
                   }
                   desc += "}";
                   std::cout << desc << "\n";   
                   break;
                }
                
            case Gmat::BOOLEAN_TYPE:
                std::cout << obj->GetBooleanParameter(i) << "\n";   
                break;
                
            default:
                break;
        }
    }
    if (obj->GetType() == Gmat::BURN)
    {
        Integer id = obj->GetParameterID("CoordinateFrame");
        StringArray frames = obj->GetStringArrayParameter(id);
        std::cout << "  Available frames:\n";
        for (StringArray::iterator iter = frames.begin(); 
             iter != frames.end(); ++iter)
        {
            std::cout << "    " << *iter << "\n";
        }
    }                  
}


void TestForceModel(Real *instate = NULL)
{
    Real state[6];
    const Real *dv;
    //Integer id;

    
    ForceModel::SetScriptAlias("PrimaryBodies", "GravityField");
    ForceModel::SetScriptAlias("PointMasses", "PointMassForce");
    ForceModel::SetScriptAlias("Drag", "DragForce");
    ForceModel::SetScriptAlias("SRP", "SolarRadiationPressure");
    
    SolarSystem *solar = new SolarSystem("DefaultSolarSystem");
    /// @todo Need to fix the file path/name for platform dependencies
    SlpFile     *ephem = new SlpFile("/GMAT/dev/datafiles/slp/mn2000.unix");
    solar->SetSourceFile(ephem);

    // Check out the soalr system interfaces to be sure we have a Suna and Earth
    CelestialBody *sun   = solar->GetBody("Sun");
    CelestialBody *earth = solar->GetBody("Earth");
    CelestialBody *luna  = solar->GetBody("Luna");
    
    if (!sun || !earth || !luna)
    {
        delete solar;
        throw ConsoleAppException("Sun or Earth or Moon pointer is NULL");
    }
    
    earth->SetAtmosphereModel("Exponential");

    Spacecraft *sat = new Spacecraft("TestCraft");
    ForceModel *fm = new ForceModel("fm");
//    fm->SetSolarSystem(solar);
    
    PhysicalModel *pmf, *pmm, *pms, *srp, *df, *current, *grLuna;
    
    if (0)
    {
       pmf = new PointMassForce;
       pmf->SetStringParameter("BodyName", "Earth");
    }
    else
    {
       pmf = new GravityField("", "");
       pmf->SetStringParameter("BodyName", "Earth");
       pmf->SetStringParameter("Filename", "Earth.grv");
    }
    
    pmm = new PointMassForce;
    pmm->SetStringParameter("BodyName", "Luna");
    pms = new PointMassForce;
    pms->SetStringParameter("BodyName", "Sun");
    srp = new SolarRadiationPressure;
    df  = new DragForce;

    grLuna = new GravityField("LunaGrav", "Luna");
    grLuna->SetStringParameter("Filename", "lp165p.grv");
    
    fm->AddForce(pmf);
    fm->AddForce(df);
    fm->AddForce(pmm);
    fm->AddForce(pms);
    fm->AddForce(srp);
    fm->AddForce(grLuna);
    
    fm->AddSpaceObject(sat);
    fm->SetSolarSystem(solar);
    fm->Initialize();

    std::cout.precision(16);
    
    // A1Mjd atTime
    Rvector6 rvSun = sun->GetState(21545.0);
    std::cout << "Sun position:   " 
              << rvSun[0] << "  " << rvSun[1] << "  " 
              << rvSun[2] << "\n";
    Rvector6 rvEarth = earth->GetState(21545.0);
    std::cout << "Earth position: "
              << rvEarth[0] << "  " << rvEarth[1] << "  "
              << rvEarth[2] << "\n";
    Rvector6 rvMoon = luna->GetState(21545.0);
    std::cout << "Moon position: "
              << rvMoon[0] << "  " << rvMoon[1] << "  "
              << rvMoon[2] << "\n";
    
    std::cout << "Earth uses the " 
              << earth->GetAtmosphereModelType() 
              << " atmosphere model\n\n";
    
    // Angular momentum vector is hardcoded right now...       
    // Real *w = earth->GetAngularVelocity();
    // std::cout << "Earth angular velocity: " 
    //           << w[0] << "  " << w[1] << "  " << w[2]
    //           << "\n\n";
    
    state[0] = 6000.0;
    state[1] = 1000.0;
    state[2] = 3000.0;
    state[3] = 1.0;
    state[4] = 6.7;
    state[5] = -3.0;
    
    std::cout << "Input State\n   Position: " 
              << state[0] << ", " << state[1] << ", " << state[2] << "\n"
              << "   Velocity: "
              << state[3] << ", " << state[4] << ", " << state[5]
              << std::endl;
    
    std::cout << "Calling GetDerivatives\n" << std::endl;
    fm->GetDerivatives(state, 0.0, 1);

    for (int i = 0; i < 7; ++i)
    {
       switch (i) {
          case 0:
             current = fm;
             break;             
          case 1:
             current = pmf;
             break;             
          case 2:
             current = pmm;
             break;             
          case 3:
             current = pms;
             break;             
          case 4:
             current = srp;
             break;
          case 5:
             current = df;
             break;
          case 6:
             current = grLuna;
             break;
          default:
             current = fm;
       }

       std::cout << "\nCurrent force is " << current->GetTypeName();
       dv = current->GetDerivativeArray();
       
       std::string body = current->GetStringParameter("BodyName");
       if (body != "STRING_PARAMETER_UNDEFINED")
          std::cout << " for body " << body;
       std::cout << std::endl;
    
       if (dv == NULL)
       {
          std::cout << "ERROR: dv = 0" << std::endl;
          delete fm;
          delete sat;
          delete solar;    
          throw ConsoleAppException("Derivative pointer is NULL");
       }
    
       std::cout << "Derivatives are: " << dv[0] << ", " 
                 << dv[1] << ", " 
                 << dv[2] << ", " 
                 << dv[3] << ", " 
                 << dv[4] << ", " 
                 << dv[5] << "\n";\
    }
    
    std::cout << "\n\nForceModel data:\n";
    DumpObjectData(fm);
    
    // Now dump the individual forces
    ObjectArray models = fm->GetRefObjectArray("GravityField");
    for (ObjectArray::iterator i = models.begin(); i != models.end(); ++i)
    {
       std::cout << "\n********************************************\n";
       DumpObjectData(*i);
    }
    models = fm->GetRefObjectArray("PointMasses");
    for (ObjectArray::iterator i = models.begin(); i != models.end(); ++i)
    {
       std::cout << "\n********************************************\n";
       DumpObjectData(*i);
    }
    models = fm->GetRefObjectArray("Drag");
    for (ObjectArray::iterator i = models.begin(); i != models.end(); ++i)
    {
       std::cout << "\n********************************************\n";
       DumpObjectData(*i);
    }
    models = fm->GetRefObjectArray("SRP");
    for (ObjectArray::iterator i = models.begin(); i != models.end(); ++i)
    {
       std::cout << "\n********************************************\n";
       DumpObjectData(*i);
    }

    delete fm;
    delete sat;
    delete solar;
}


int main(int argc, char **argv)
{
   if (argc < 2)
   {
      std::cout << "Please run again with output filename as an input" << std::endl;
   }
   for (int q=0; q<argc; q++)
   std::cout << "Argument " << q << " is " << argv[q] << std::endl;
   freopen(argv[1], "w", stdout);
      
    try
   {
        std::cout << "************************************************\n"
                  << "*** Force Model Unit Test Program\n"
                  << "************************************************\n\n";
        TestForceModel();
    }
    catch (BaseException &ex)
   {
        std::cout << ex.GetMessage() << std::endl;
    }
}
