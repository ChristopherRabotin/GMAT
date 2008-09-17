//$Id$
//------------------------------------------------------------------------------
//                                  SolarSystem
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/mm/dd
//
/**
 * This is the implementation of the SolarSystem class.
 *
 * @note It is assumed that no classes will be derived from this one.
 */
//------------------------------------------------------------------------------
#include <list>
#include <string>
#include "SolarSystem.hpp"              // class's header file
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "Planet.hpp"
#include "Moon.hpp"
#include "StringUtil.hpp"               // for ToString()
#include "FileManager.hpp"              // for GetFullPathname()
#include "MessageInterface.hpp"         // for debugging

//#define DEBUG_SS_COPY
//#define DEBUG_SS_CLONING
//#define DEBUG_SS_PLANETARY_FILE

//---------------------------------
// static data
//---------------------------------
const std::string
SolarSystem::PARAMETER_TEXT[SolarSystemParamCount - GmatBaseParamCount] =
{
   "BodiesInUse",
   "NumberOfBodies",
   "Ephemeris",
   "UseTTForEphemeris",
   "EphemerisUpdateInterval",
};

const Gmat::ParameterType
SolarSystem::PARAMETER_TYPE[SolarSystemParamCount - GmatBaseParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::REAL_TYPE,
};

const std::string
SolarSystem::PLANETARY_SOURCE_STRING[PlanetarySourceCount] =
{
   "Analytic",
   "SLP",
   "DE200",
   "DE405",
};

const std::string
SolarSystem::ANALYTIC_MODEL_STRING[AnalyticModelCount] =
{
   "Low Fidelity",
};

// define default names form solar system bodies
const std::string SolarSystem::SUN_NAME        = "Sun";

const std::string SolarSystem::MERCURY_NAME    = "Mercury";

const std::string SolarSystem::VENUS_NAME      = "Venus";

const std::string SolarSystem::EARTH_NAME      = "Earth";
const std::string SolarSystem::MOON_NAME       = "Luna";

const std::string SolarSystem::MARS_NAME       = "Mars";
const std::string SolarSystem::PHOBOS_NAME     = "Phobos";
const std::string SolarSystem::DEIMOS_NAME     = "Deimos";

const std::string SolarSystem::JUPITER_NAME    = "Jupiter";
const std::string SolarSystem::METIS_NAME      = "Metis";
const std::string SolarSystem::ADRASTEA_NAME   = "Adrastea";
const std::string SolarSystem::AMALTHEA_NAME   = "Amalthea";
const std::string SolarSystem::THEBE_NAME      = "Thebe";
const std::string SolarSystem::IO_NAME         = "Io";
const std::string SolarSystem::EUROPA_NAME     = "Europa";
const std::string SolarSystem::GANYMEDE_NAME   = "Ganymede";
const std::string SolarSystem::CALLISTO_NAME   = "Callisto";

const std::string SolarSystem::SATURN_NAME     = "Saturn";
const std::string SolarSystem::PAN_NAME        = "Pan";
const std::string SolarSystem::ATLAS_NAME      = "Atlas";
const std::string SolarSystem::PROMETHEUS_NAME = "Promethus";
const std::string SolarSystem::PANDORA_NAME    = "Pandora";
const std::string SolarSystem::EPIMETHEUS_NAME = "Epimetheus";
const std::string SolarSystem::JANUS_NAME      = "Janus";
const std::string SolarSystem::MIMAS_NAME      = "Mimas";
const std::string SolarSystem::ENCELADUS_NAME  = "Enceladus";
const std::string SolarSystem::TETHYS_NAME     = "Tethys";
const std::string SolarSystem::TELESTO_NAME    = "Telesto";
const std::string SolarSystem::CALYPSO_NAME    = "Calypso";
const std::string SolarSystem::DIONE_NAME      = "Dione";
const std::string SolarSystem::HELENE_NAME     = "Helene";
const std::string SolarSystem::RHEA_NAME       = "Rhea";
const std::string SolarSystem::TITAN_NAME      = "Titan";
const std::string SolarSystem::IAPETUS_NAME    = "Iapetus";
const std::string SolarSystem::PHOEBE_NAME     = "Phoebe";

const std::string SolarSystem::URANUS_NAME     = "Uranus";
const std::string SolarSystem::CORDELIA_NAME   = "Cordelia";
const std::string SolarSystem::OPHELIA_NAME    = "Ophelia";
const std::string SolarSystem::BIANCA_NAME     = "Bianca";
const std::string SolarSystem::CRESSIDA_NAME   = "Cressida";
const std::string SolarSystem::DESDEMONA_NAME  = "Desdemona";
const std::string SolarSystem::JULIET_NAME     = "Juliet";
const std::string SolarSystem::PORTIA_NAME     = "Portia";
const std::string SolarSystem::ROSALIND_NAME   = "Rosalind";
const std::string SolarSystem::BELINDA_NAME    = "Belinda";
const std::string SolarSystem::PUCK_NAME       = "Puck";
const std::string SolarSystem::MIRANDA_NAME    = "Miranda";
const std::string SolarSystem::ARIEL_NAME      = "Ariel";
const std::string SolarSystem::UMBRIEL_NAME    = "Umbriel";
const std::string SolarSystem::TITANIA_NAME    = "Titania";
const std::string SolarSystem::OBERON_NAME     = "Oberon";

const std::string SolarSystem::NEPTUNE_NAME    = "Neptune";
const std::string SolarSystem::NAIAD_NAME      = "Naiad";
const std::string SolarSystem::THALASSA_NAME   = "Thalassa";
const std::string SolarSystem::DESPINA_NAME    = "Despina";
const std::string SolarSystem::GALATEA_NAME    = "Galatea";
const std::string SolarSystem::LARISSA_NAME    = "Larissa";
const std::string SolarSystem::PROTEUS_NAME    = "Proteus";
const std::string SolarSystem::TRITON_NAME     = "Triton";

const std::string SolarSystem::PLUTO_NAME      = "Pluto";
const std::string SolarSystem::CHARON_NAME     = "Charon";

// add other moons, asteroids, comets, as needed
// what about libration points?

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  SolarSystem(std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SolarSystem class (default constructor).
 *
 * @param <withName> optional parameter indicating the name of the solar
 *                   system (default is "").
 */
//------------------------------------------------------------------------------
SolarSystem::SolarSystem(std::string withName)
   : GmatBase(Gmat::SOLAR_SYSTEM, "SolarSystem", withName)
{
   objectTypes.push_back(Gmat::SOLAR_SYSTEM);
   objectTypeNames.push_back("SolarSystem");
   parameterCount      = SolarSystemParamCount;
   pvSrcForAll         = Gmat::DE_405;
   anMethodForAll      = Gmat::LOW_FIDELITY;
   thePlanetaryEphem   = NULL;
   overrideTimeForAll  = false;
   ephemUpdateInterval = 0.0;
   
   // create and add the default bodies
   Star* theSun     = new Star(SUN_NAME);
   theSun->SetCentralBody(EARTH_NAME);  // central body here is a reference body
   Planet* theEarth = new Planet(EARTH_NAME,   SUN_NAME);
   Moon* theMoon    = new Moon(MOON_NAME,      EARTH_NAME);
   Planet* mercury  = new Planet(MERCURY_NAME, SUN_NAME);
   Planet* venus    = new Planet(VENUS_NAME,   SUN_NAME);
   Planet* mars     = new Planet(MARS_NAME,    SUN_NAME);
   Planet* jupiter  = new Planet(JUPITER_NAME, SUN_NAME);
   Planet* saturn   = new Planet(SATURN_NAME,  SUN_NAME);
   Planet* uranus   = new Planet(URANUS_NAME,  SUN_NAME);
   Planet* neptune  = new Planet(NEPTUNE_NAME, SUN_NAME);
   Planet* pluto    = new Planet(PLUTO_NAME,   SUN_NAME);

   AddBody(theSun);
   AddBody(theEarth);
   AddBody(theMoon);
   AddBody(mercury);
   AddBody(venus);
   AddBody(mars);
   AddBody(jupiter);
   AddBody(saturn);
   AddBody(uranus);
   AddBody(neptune);
   AddBody(pluto);
   
   // at least for now, give each body a pointer to its central body
   theSun->SetRefObject(theEarth, Gmat::CELESTIAL_BODY, SolarSystem::EARTH_NAME); // for reference!!!
   theEarth->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   theMoon->SetRefObject(theEarth, Gmat::CELESTIAL_BODY, SolarSystem::EARTH_NAME);
   mercury->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   venus->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   mars->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   jupiter->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   saturn->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   uranus->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   neptune->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   pluto->SetRefObject(theSun, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   
   theDefaultSlpFile   = NULL;
   theDefaultDeFile    = NULL;
   
   CreatePlanetarySource();
}


//------------------------------------------------------------------------------
//  SolarSystem(const SolarSystem &ss)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SolarSystem class as a copy of the
 * specified SolarSystem class (copy constructor).
 *
 * @param <ss> SolarSystem object to copy.
 */
//------------------------------------------------------------------------------
SolarSystem::SolarSystem(const SolarSystem &ss) :
   GmatBase            (ss),
   pvSrcForAll         (ss.pvSrcForAll),
   anMethodForAll      (ss.anMethodForAll),
   thePlanetaryEphem   (NULL),
   overrideTimeForAll  (ss.overrideTimeForAll),
   ephemUpdateInterval (ss.ephemUpdateInterval),
   bodyStrings         (ss.bodyStrings)
{
   theDefaultSlpFile = NULL;
   theDefaultDeFile  = NULL;
   parameterCount    = SolarSystemParamCount;
   
   // create planetary source first, but do not create default
   CreatePlanetarySource(false);
   
   // copy current planetary source is use
   thePlanetarySourceTypesInUse = ss.thePlanetarySourceTypesInUse;
   
   #ifdef DEBUG_SS_PLANETARY_FILE
   MessageInterface::ShowMessage
      ("SolarSystem::copy constructor() num ephem source types=%d\n",
       thePlanetarySourceTypesInUse.size());
   for (UnsignedInt i=0; i<thePlanetarySourceTypesInUse.size(); i++)
      MessageInterface::ShowMessage
         ("   '%s'\n", thePlanetarySourceTypesInUse[i].c_str());
   #endif
   
   // set current planetary source
   SetPlanetarySourceTypesInUse(thePlanetarySourceTypesInUse);
   
   CloneBodiesInUse(ss);
}


//------------------------------------------------------------------------------
//  SolarSystem& operator= (const SolarSystem& ss)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SolarSystem class.
 *
 * @param <ss> the solar system object whose data to assign to "this"
 *             solar system.
 *
 * @return "this" solarsystem with data of input solarsystem ss.
 */
//------------------------------------------------------------------------------
SolarSystem& SolarSystem::operator=(const SolarSystem &ss)
{
   GmatBase::operator=(ss);
   pvSrcForAll         = ss.pvSrcForAll;
   anMethodForAll      = ss.anMethodForAll;
   thePlanetaryEphem   = NULL;
   overrideTimeForAll  = ss.overrideTimeForAll;
   ephemUpdateInterval = ss.ephemUpdateInterval;
   bodyStrings         = ss.bodyStrings;
   parameterCount      = SolarSystemParamCount;
   theDefaultSlpFile   = NULL;
   theDefaultDeFile    = NULL;
   
   // create planetary source first, but do not create default
   CreatePlanetarySource(false);
   
   // copy current planetary source in use
   thePlanetarySourceTypesInUse = ss.thePlanetarySourceTypesInUse;
   
   // set current planetary source
   SetPlanetarySourceTypesInUse(thePlanetarySourceTypesInUse);
   
   CloneBodiesInUse(ss);
   
   return *this;
}


//------------------------------------------------------------------------------
//  ~SolarSystem()
//------------------------------------------------------------------------------
/**
 * Destructor for the SolarSystem class.
 */
//------------------------------------------------------------------------------
SolarSystem::~SolarSystem()
{
   DeleteBodiesInUse();
   
   #ifdef DEBUG_SS_CLONING   
   MessageInterface::ShowMessage
      ("SolarSystem::~SolarSystem() <%s> thePlanetaryEphem=%p\n",
       GetName().c_str(), thePlanetaryEphem);
   #endif
   
   if (theDefaultSlpFile != NULL)
   {      
      #ifdef DEBUG_SS_CLONING   
      MessageInterface::ShowMessage
         ("   deleting theDefaultSlpFile=%p\n",  theDefaultSlpFile);
      #endif
      
      delete theDefaultSlpFile;
   }
   
   if (theDefaultDeFile != NULL)
   {
      #ifdef DEBUG_SS_CLONING   
      MessageInterface::ShowMessage
         ("   deleting theDefaultDeFile=%p\n", theDefaultDeFile);
      #endif
      
      delete theDefaultDeFile;
   }
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool SolarSystem::Initialize()
{
   // Initialize bodies in use
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      (*cbi)->Initialize();      
      ++cbi;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// void CreatePlanetarySource(bool setDefault)
//------------------------------------------------------------------------------
/*
 * Creates planetary epemeris source.
 *
 * @param <setDefault> set to true if default ephemeris is to be created (true)
 */
//------------------------------------------------------------------------------
void SolarSystem::CreatePlanetarySource(bool setDefault)
{
   #ifdef DEBUG_SS_PLANETARY_FILE
   MessageInterface::ShowMessage("SolarSystem initializing planetary source...\n");
   #endif
   
   // initialize planetary source
   for (int i=0; i<PlanetarySourceCount; i++)
   {
      thePlanetarySourcePriority[i] = 0;
      isPlanetarySourceInUse[i] = false;
      
      thePlanetarySourceTypes.push_back(PLANETARY_SOURCE_STRING[i]);
   }
   
   for (int i=0; i<AnalyticModelCount; i++)
   {
      theAnalyticModelNames.push_back(ANALYTIC_MODEL_STRING[i]);
   }
   
   theAnalyticMethod = Gmat::LOW_FIDELITY;
   
   FileManager *fm = FileManager::Instance();
   
   //initialize file names
   thePlanetarySourceNames.push_back("NA");
   thePlanetarySourceNames.push_back(fm->GetFullPathname("SLP_FILE"));
   thePlanetarySourceNames.push_back(fm->GetFullPathname("DE200_FILE"));
   thePlanetarySourceNames.push_back(fm->GetFullPathname("DE405_FILE"));
   
   // Set planetary ephemeris source
   if (setDefault)
      SetDefaultPlanetarySource();
}


//------------------------------------------------------------------------------
// const StringArray& GetPlanetarySourceTypes()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types
 */
//------------------------------------------------------------------------------
const StringArray& SolarSystem::GetPlanetarySourceTypes()
{
   return thePlanetarySourceTypes;
}

//------------------------------------------------------------------------------
// const StringArray& GetPlanetarySourceNames()
//------------------------------------------------------------------------------
/**
 * @return a planetary source file names
 */
//------------------------------------------------------------------------------
const StringArray& SolarSystem::GetPlanetarySourceNames()
{
   return thePlanetarySourceNames;
}

//------------------------------------------------------------------------------
// const StringArray& GetPlanetarySourceTypesInUse()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types in use
 */
//------------------------------------------------------------------------------
const StringArray& SolarSystem::GetPlanetarySourceTypesInUse()
{
   theTempFileList.clear();
   for (unsigned int i=0; i<thePlanetarySourceTypesInUse.size(); i++)
      theTempFileList.push_back(thePlanetarySourceTypesInUse[i]);

   return theTempFileList;
}


//------------------------------------------------------------------------------
// const StringArray& GetAnalyticModelNames()
//------------------------------------------------------------------------------
/**
 * @return available planetary analytic model names.
 */
//------------------------------------------------------------------------------
const StringArray& SolarSystem::GetAnalyticModelNames()
{
   return theAnalyticModelNames;
}


//------------------------------------------------------------------------------
// bool SetAnalyticModelToUse(const std::string &modelName)
//------------------------------------------------------------------------------
bool SolarSystem::SetAnalyticModelToUse(const std::string &modelName)
{
   int modelId = 0;
   for (int i=0; i<AnalyticModelCount; i++)
   {
      if (ANALYTIC_MODEL_STRING[i] == modelName)
      {
         modelId = i;
         break;
      }
   }
   
   // because NO_ANALYTIC_METHOD = 0 in CelestialBody.hpp
   theAnalyticMethod = Gmat::AnalyticMethod(modelId + 1);
   
   #if DEBUG_PLANETARY_FILE
   MessageInterface::ShowMessage
      ("SolarSystem::SetAnalyticModelToUse() theAnalyticMethod=%d\n",
       theAnalyticMethod);
   #endif
   
   return SetAnalyticMethod(theAnalyticMethod);
}


//------------------------------------------------------------------------------
// bool SetPlanetarySourceName(const std::string &sourceType,
//                           const std::string &fileName)
//------------------------------------------------------------------------------
bool SolarSystem::SetPlanetarySourceName(const std::string &sourceType,
                                         const std::string &fileName)
{
   bool status = false;
   Integer id = GetPlanetarySourceId(sourceType);

   if (id >= 0)
   {
      thePlanetarySourceNames[id] = fileName;
      status = true;
   }
   
   return status;
}


//------------------------------------------------------------------------------
// std::string GetPlanetarySourceName(const std::string &sourceType)
//------------------------------------------------------------------------------
std::string SolarSystem::GetPlanetarySourceName(const std::string &sourceType)
{
   Integer id = GetPlanetarySourceId(sourceType);

   if (id >= 0)
      return thePlanetarySourceNames[id];
   else
      return "Unknown Source Type";
}


//------------------------------------------------------------------------------
// Integer SetPlanetarySourceTypesInUse(const StringArray &sourceTypes)
//------------------------------------------------------------------------------
/*
 * @param <sourceTypes> list of file type in the priority order of use
 *
 * @return 0, if error setting any of planetary file in the list.
 *         1, if error setting first planetary file in the list, but set to
 *            next available file.
 *         2, if successfuly set to first planetary file in the list
 */
//------------------------------------------------------------------------------
Integer SolarSystem::SetPlanetarySourceTypesInUse(const StringArray &sourceTypes)
{
   #ifdef DEBUG_SS_PLANETARY_FILE
   MessageInterface::ShowMessage
      ("SolarSystem::SetPlanetarySourceTypesInUse() num source types=%d\n",
       sourceTypes.size());
   for (UnsignedInt i=0; i<sourceTypes.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", sourceTypes[i].c_str());
   #endif
   
   bool status = false;
   Integer sourceTypeInUse = -1;
   Integer retCode = 0;
   
   // update planetary file types
   if (&thePlanetarySourceTypesInUse != &sourceTypes)
   {
      #ifdef DEBUG_SS_PLANETARY_FILE
      MessageInterface::ShowMessage
         ("SolarSystem::SetPlanetarySourceTypesInUse() updating planetary source\n");
      #endif
      
      thePlanetarySourceTypesInUse.clear();
      
      for (unsigned int i=0; i<sourceTypes.size(); i++)
      {
         thePlanetarySourceTypesInUse.push_back(sourceTypes[i]);
      }
   }
   
   theTempFileList.clear();
   for (unsigned int i=0; i<thePlanetarySourceTypesInUse.size(); i++)
      theTempFileList.push_back(thePlanetarySourceTypesInUse[i]);
   
   // create planetary ephem file if non-analytic
   for (unsigned int i=0; i<thePlanetarySourceTypesInUse.size(); i++)
   {
      if (thePlanetarySourceTypesInUse[i] == PLANETARY_SOURCE_STRING[ANALYTIC])
      {
         thePlanetarySourcePriority[ANALYTIC] = 0;
         status = SetAnalyticMethod(theAnalyticMethod);
         if (status)
         {
            thePlanetarySourcePriority[ANALYTIC] = HIGHEST_PRIORITY - i;
            isPlanetarySourceInUse[ANALYTIC] = true;
            sourceTypeInUse = ANALYTIC;
            break;
         }
         else
         {
            MessageInterface::ShowMessage
               ("*** Error *** Failed to Set AnalyticMethod: %d\n",
                theAnalyticMethod);
         }
      }
      else if (thePlanetarySourceTypesInUse[i] == PLANETARY_SOURCE_STRING[SLP])
      {
         thePlanetarySourcePriority[SLP] = 0;
         status = CreateSlpFile(thePlanetarySourceNames[SLP]);
         if (status)
         {
            thePlanetarySourcePriority[SLP] = HIGHEST_PRIORITY - i;
            isPlanetarySourceInUse[SLP] = true;
            sourceTypeInUse = SLP;
            break;
         }
      }
      else if (thePlanetarySourceTypesInUse[i] == PLANETARY_SOURCE_STRING[DE200])
      {
         #ifdef DEBUG_SS_PLANETARY_FILE
         MessageInterface::
            ShowMessage("SolarSystem::SetPlanetarySourceTypesInUse() create DE200\n");
         #endif
         
         thePlanetarySourcePriority[DE200] = 0;
         status = CreateDeFile(DE200, thePlanetarySourceNames[DE200]);
         if (status)
         {
            thePlanetarySourcePriority[DE200] = HIGHEST_PRIORITY - i;
            isPlanetarySourceInUse[DE200] = true;
            sourceTypeInUse = DE200;
            break;
         }
      }
      else if (thePlanetarySourceTypesInUse[i] == PLANETARY_SOURCE_STRING[DE405])
      {
         #ifdef DEBUG_SS_PLANETARY_FILE
         MessageInterface::
            ShowMessage("SolarSystem::SetPlanetarySourceTypesInUse() create DE405\n");
         #endif
         
         thePlanetarySourcePriority[DE405] = 0;
         status = CreateDeFile(DE405, thePlanetarySourceNames[DE405]);
         if (status)
         {
            thePlanetarySourcePriority[DE405] = HIGHEST_PRIORITY - i;
            isPlanetarySourceInUse[DE405] = true;
            sourceTypeInUse = DE405;
            break;
         }
      }
   }
   
   // set SolarSystem to use the file
   if (sourceTypeInUse == -1)
   {
      MessageInterface::ShowMessage("SolarSystem::SetPlanetarySourceTypesInUse() NO "
                                    "Planetary file is set to use \n");
      retCode = 0;
   }
   else
   {
      #ifdef DEBUG_SS_PLANETARY_FILE
      MessageInterface::
         ShowMessage("SolarSystem::SetPlanetarySourceTypesInUse() "
                     "Set Planetary Source to use:%d\n", sourceTypeInUse);
      #endif
      switch (sourceTypeInUse)
      {
      case ANALYTIC:
         if (SetSource(Gmat::ANALYTIC))
            if (SetAnalyticMethod(theAnalyticMethod))
               retCode = 1;
         break;
      case SLP:
         if (SetSource(Gmat::SLP))
            if (SetSourceFile(theDefaultSlpFile))
               retCode = 1;
         break;
      case DE200:
         if (SetSource(Gmat::DE_200))
            if (SetSourceFile(theDefaultDeFile))
               retCode = 1;
         break;
      case DE405:
         if (SetSource(Gmat::DE_405))
            if (SetSourceFile(theDefaultDeFile))
               retCode = 1;
         break;
      default:
         break;
      }
   }
   
   // if planetary file is set to first type in the list
   if (retCode == 1 && PLANETARY_SOURCE_STRING[sourceTypeInUse] == sourceTypes[0])
      retCode = 2;
   
   
   // if error setting given planetary file, re-arrange planetary file list
   if (retCode == 1)
   {      
      thePlanetarySourceTypesInUse.clear();

      for (unsigned int i=0; i<theTempFileList.size(); i++)
      {            
         if (theTempFileList[i] == PLANETARY_SOURCE_STRING[ANALYTIC])
         {
            if (thePlanetarySourcePriority[ANALYTIC] > 0)
               thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[ANALYTIC]);
         }
         else if (theTempFileList[i] == PLANETARY_SOURCE_STRING[SLP])
         {
            if (thePlanetarySourcePriority[SLP] > 0)
               thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[SLP]);
         }
         else if (theTempFileList[i] == PLANETARY_SOURCE_STRING[DE200])
         {
            if (thePlanetarySourcePriority[DE200] > 0)
               thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[DE200]);
         }
         else if (theTempFileList[i] == PLANETARY_SOURCE_STRING[DE405])
         {
            if (thePlanetarySourcePriority[DE405] > 0)
               thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[DE405]);
         }
      }
      
      #ifdef DEBUG_SS_PLANETARY_FILE
      for (unsigned int i=0; i<thePlanetarySourceTypesInUse.size(); i++)
      {
         MessageInterface::ShowMessage
            ("thePlanetarySourceTypesInUse[%d]=%s\n", i,
             thePlanetarySourceTypesInUse[i].c_str());
      }
      #endif
   }
   
   if (retCode > 0)
      MessageInterface::ShowMessage
         ("Successfully set Planetary Source to use: %s\n",
          PLANETARY_SOURCE_STRING[sourceTypeInUse].c_str());
   
   theCurrentPlanetarySource = PLANETARY_SOURCE_STRING[sourceTypeInUse];
   
   #ifdef DEBUG_SS_PLANETARY_FILE
   MessageInterface::ShowMessage
      ("SolarSystem::SetPlanetarySourceTypesInUse() returning %d, "
       "theCurrentPlanetarySource is set to '%s'\n", retCode,
       theCurrentPlanetarySource.c_str());
   #endif
   
   return retCode;
}

//------------------------------------------------------------------------------
// Integer GetPlanetarySourceId(const std::string &sourceType)
//------------------------------------------------------------------------------
Integer SolarSystem::GetPlanetarySourceId(const std::string &sourceType)
{
   for (int i=0; i<PlanetarySourceCount; i++)
   {
      if (sourceType == PLANETARY_SOURCE_STRING[i])
         return i;
   }
   
   return -1;
}


//------------------------------------------------------------------------------
// std::string GetPlanetarySourceNameInUse()
//------------------------------------------------------------------------------
std::string SolarSystem::GetCurrentPlanetarySource()
{
   return theCurrentPlanetarySource;
}


//------------------------------------------------------------------------------
// void ResetToDefaults()
//------------------------------------------------------------------------------
void SolarSystem::ResetToDefaults()
{
   pvSrcForAll         = Gmat::DE_405;
   anMethodForAll      = Gmat::LOW_FIDELITY;
   thePlanetaryEphem   = NULL;
   overrideTimeForAll  = false;
   ephemUpdateInterval = 0.0;
   
   // Set it for each of the bodies
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      (*cbi)->SetSource(pvSrcForAll);
      (*cbi)->SetAnalyticMethod(anMethodForAll);
      (*cbi)->SetOverrideTimeSystem(overrideTimeForAll);
      (*cbi)->SetEphemUpdateInterval(ephemUpdateInterval);
      (*cbi)->SetEphemUpdateInterval(ephemUpdateInterval);
      (*cbi)->SetUsePotentialFile(false);
      ++cbi;
   }
}


//------------------------------------------------------------------------------
//  bool AddBody(CelestialBody* cb)
//------------------------------------------------------------------------------
/**
 * This method adds to the list of bodies in use for this solar system.
 *
 * @param <cb>  body to add to the list for this solar system.
 *
 * @return flag indicating success of the operation
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::AddBody(CelestialBody* cb)
{
   if (cb == NULL)
   {
      return false;    // write some kind of error or throw an exception?
   }
   
   bodiesInUse.push_back(cb);
   bodyStrings.push_back(cb->GetName());

   #ifdef DEBUG_SS_ADD_BODY
   MessageInterface::ShowMessage
      ("   %s, Added <%p> %s\n", GetName().c_str(), cb, cb->GetName().c_str());
   #endif
   
   // Set the source, source file, analytic method, and override flag for the 
   // new body
   if (!cb->SetSource(pvSrcForAll))  return false; 
   if (!cb->SetAnalyticMethod(anMethodForAll))  return false; 
   if (thePlanetaryEphem)
      if (!cb->SetSourceFile(thePlanetaryEphem))  return false; 
   if (!cb->SetOverrideTimeSystem(overrideTimeForAll))  return false; 
   
   return true;
}

//------------------------------------------------------------------------------
//  CelestialBody* GetBody(std::string withName)
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to the requested celestial body.
 *
 * @param <withName>  name of the requested body.
 *
 * @return a pointer to the requested body.
 *
 */
//------------------------------------------------------------------------------
CelestialBody* SolarSystem::GetBody(std::string withName)
{
   return FindBody(withName);
}

//------------------------------------------------------------------------------
//  bool IsBodyInUse(std::string theBody)
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether the specified celestial body
 * is in use.
 *
 * @param <theBody>  name of the body.
 *
 * @return a flag indicating whether the specified body is in use.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::IsBodyInUse(std::string theBody)
{
   // Search through bodiesInUse for the body with the name withName
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->GetName() == theBody)
      {
         return true;
      }
      ++cbi;
   }
   return false;
}

//------------------------------------------------------------------------------
//  Gmat::PosVelSource GetPosVelSource() const
//------------------------------------------------------------------------------
/**
 * This method returns the source of position and velocity for the bodies in
 * use (assuming all have the same source).
 *
 * @return position/velocity source for the bodies.
 *
 */
//------------------------------------------------------------------------------
Gmat::PosVelSource   SolarSystem::GetPosVelSource() const
{
   return pvSrcForAll;
}

//------------------------------------------------------------------------------
//  Gmat::AnalyticMethod GetAnalyticMethod() const
//------------------------------------------------------------------------------
/**
 * This method returns the analytic method for the bodies in
 * use (assuming all have the same method), when the source is ANALYTIC_METHOD.
 *
 * @return analytic method for the bodies.
 *
 */
//------------------------------------------------------------------------------
Gmat::AnalyticMethod SolarSystem::GetAnalyticMethod() const
{
   return anMethodForAll;
}

//------------------------------------------------------------------------------
//  std::string GetSourceFileName() const
//------------------------------------------------------------------------------
/**
 * This method returns the source file name for the bodies in
 * use (assuming all have the same source).
 *
 * @return source file name for the bodies.
 *
 */
//------------------------------------------------------------------------------
std::string SolarSystem::GetSourceFileName() const
{
   if (thePlanetaryEphem == NULL) return "";
   return thePlanetaryEphem->GetName();
}


//------------------------------------------------------------------------------
// bool GetOverrideTimeSystem() const
//------------------------------------------------------------------------------
bool SolarSystem::GetOverrideTimeSystem() const
{
   return overrideTimeForAll;
}


//------------------------------------------------------------------------------
// Real GetEphemUpdateInterval() const
//------------------------------------------------------------------------------
Real SolarSystem::GetEphemUpdateInterval() const
{
   return ephemUpdateInterval;
}
 

//------------------------------------------------------------------------------
// StringArray GetValidModelList(Gmat::ModelType m, const std::string &forBody)
//------------------------------------------------------------------------------
StringArray SolarSystem::GetValidModelList(Gmat::ModelType m, 
                         const std::string &forBody)
{
   for (std::vector<CelestialBody*>::iterator i = bodiesInUse.begin();
        i != bodiesInUse.end(); ++i)
   {
      if ((*i)->GetName() == forBody)
         return (*i)->GetValidModelList(m);
   }
   throw SolarSystemException("Model list requested for unknown body " 
                               + forBody);
}


//------------------------------------------------------------------------------
//  bool SetSource(Gmat::PosVelSource pvSrc)
//------------------------------------------------------------------------------
/**
 * This method sets the source for the bodies in
 * use (assuming all have the same source).
 *
 * @param <pvSrc> source (for pos and vel) for all of the bodies.
 *
* @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetSource(Gmat::PosVelSource pvSrc)
{
   // Search through bodiesInUse for the body with the name withName
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetSource(pvSrc) == false)  return false;
      ++cbi;
   }
   pvSrcForAll = pvSrc;
   return true;
}

//------------------------------------------------------------------------------
//  bool SetSource(const std::string &pvSrc)
//------------------------------------------------------------------------------
/**
 * This method sets the source for the bodies in
 * use (assuming all have the same source).
 *
 * @param <pvSrc> source (for pos and vel) for all of the bodies.
 *
 * @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetSource(const std::string &pvSrc)
{
   Gmat::PosVelSource theSrc = Gmat::PosVelSourceCount;
   for (Integer i = 0; i < Gmat::PosVelSourceCount; i++)
   {
      if (pvSrc == Gmat::POS_VEL_SOURCE_STRINGS[i]) 
         theSrc = (Gmat::PosVelSource) i; 
   }
   if (theSrc == Gmat::PosVelSourceCount)
      throw SolarSystemException("Unknown ephemeris source " + pvSrc);
   
   // Search through bodiesInUse and set the source for all
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetSource(theSrc) == false)  return false;
      ++cbi;
   }
   pvSrcForAll = theSrc;
   return true;
}

//------------------------------------------------------------------------------
//  bool SetSourceFile(PlanetaryEphem *src)
//------------------------------------------------------------------------------
/**
 * This method sets the source file for the bodies in
 * use (assuming all have the same method).
 *
 * @param <src> planetary ephem - source for all of the bodies.
 *
 * @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetSourceFile(PlanetaryEphem *src)
{   
   // check for null src
   if (src == NULL)
      return false;
   
   thePlanetaryEphem = src;
   
   #ifdef DEBUG_SS_PLANETARY_FILE
   MessageInterface::ShowMessage
      ("SolarSystem::SetSourceFile() Setting source file to <%p>\n   %s\n", src,
       src->GetName().c_str(), src);
   #endif
   
   // Search through bodiesInUse for the body with the name withName
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetSourceFile(thePlanetaryEphem) == false) return false;
      ++cbi;
   }
   
   return true;
}


//------------------------------------------------------------------------------
//  bool SetAnalyticMethod(Gmat::AnalyticMethod aM)
//------------------------------------------------------------------------------
/**
* This method sets the analytic method for the bodies in
 * use (assuming all have the same method).
 *
 * @param aMc> analytic method selection for all of the bodies.
 *
 * @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetAnalyticMethod(Gmat::AnalyticMethod aM)
{
   // Search through bodiesInUse for the body with the name withName
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetAnalyticMethod(aM) == false)  return false;
      ++cbi;
   }
   anMethodForAll = aM;
   return true;
}

//------------------------------------------------------------------------------
//  bool SetAnalyticMethod(const std::string &aM)
//------------------------------------------------------------------------------
/**
 * This method sets the analytic method for the bodies in
 * use (assuming all have the same method).
 *
 * @param aMc> analytic method selection for all of the bodies.
 *
 * @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetAnalyticMethod(const std::string &aM)
{
   Gmat::AnalyticMethod theMethod = Gmat::AnalyticMethodCount;
   for (Integer i = 0; i < Gmat::AnalyticMethodCount; i++)
   {
      if (aM == Gmat::ANALYTIC_METHOD_STRINGS[i]) 
         theMethod = (Gmat::AnalyticMethod) i; 
   }
   if (theMethod == Gmat::AnalyticMethodCount)
      throw SolarSystemException("Unknown analytic method " + aM);
   
   // Search through bodiesInUse and set it for all
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetAnalyticMethod(theMethod) == false)  return false;
      ++cbi;
   }
   anMethodForAll = theMethod;
   return true;
}

//------------------------------------------------------------------------------
//  bool SetOverrideTimeSystem(bool overrideIt)
//------------------------------------------------------------------------------
/**
 * This method sets the override time system flag.
 *
 * @param <overrideIt> override time system selection for all of the bodies?.
 *
 * @return success flag for the operation.
 *
 */
//------------------------------------------------------------------------------
bool SolarSystem::SetOverrideTimeSystem(bool overrideIt)
{
   // Set it for each of the bodies
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetOverrideTimeSystem(overrideIt) == false)  return false;
      ++cbi;
   }
   overrideTimeForAll = overrideIt;
   return true;
}


//------------------------------------------------------------------------------
// bool SetEphemUpdateInterval(Real intvl)
//------------------------------------------------------------------------------
bool SolarSystem::SetEphemUpdateInterval(Real intvl)
{
   if (intvl < 0.0)
   {
      SolarSystemException sse;
      sse.SetDetails(errorMessageFormat.c_str(),
                     GmatStringUtil::ToString(intvl, GetDataPrecision()).c_str(),
                     "Ephemeris Update Interval", "Real Number >= 0.0");
      throw sse;
   }
   
   // Set it for each of the bodies
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->SetEphemUpdateInterval(intvl) == false)  return false;
      ++cbi;
   }
   ephemUpdateInterval = intvl;
   return true;
}


//------------------------------------------------------------------------------
// bool AddValidModelName(Gmat::ModelType m, const std::string &forBody,
//                        const std::string &theModel)
//------------------------------------------------------------------------------
bool SolarSystem::AddValidModelName(Gmat::ModelType m, 
                  const std::string &forBody,
                  const std::string &theModel)
{
   for (std::vector<CelestialBody*>::iterator i = bodiesInUse.begin();
        i != bodiesInUse.end(); ++i)
   {
      if ((*i)->GetName() == forBody)
         return (*i)->AddValidModelName(m, theModel);
   }
   throw SolarSystemException("Cannot set new model for unknown body " 
                              + forBody);
}


//------------------------------------------------------------------------------
// bool SolarSystem::RemoveValidModelName(Gmat::ModelType m, 
//                  const std::string & forBody,
//                  const std::string &theModel)
//------------------------------------------------------------------------------
bool SolarSystem::RemoveValidModelName(Gmat::ModelType m, 
                  const std::string & forBody,
                  const std::string &theModel)
{
   for (std::vector<CelestialBody*>::iterator i = bodiesInUse.begin();
        i != bodiesInUse.end(); ++i)
   {
      if ((*i)->GetName() == forBody)
         return (*i)->RemoveValidModelName(m, theModel);
   }
   throw SolarSystemException("Cannot remove model for unknown body " 
                              + forBody);
}

//------------------------------------------------------------------------------
//  const StringArray& GetBodiesInUse() const
//------------------------------------------------------------------------------
/**
 * This method returns a list of strings representing the bodies in use.
 *
 * @return list of strings representing the bodies in use.
 *
 */
//------------------------------------------------------------------------------
const StringArray& SolarSystem::GetBodiesInUse() const
{
   return bodyStrings;
}

//------------------------------------------------------------------------------
//  SolarSystem* Clone(void) const
//------------------------------------------------------------------------------
/**
* This method returns a clone of the SolarSystem.
 *
 * @return a pointer to a clone of the SolarSystem.
 *
 */
//------------------------------------------------------------------------------
SolarSystem* SolarSystem::Clone(void) const
{
   // clone all objects in the Solar System as well
   
   return new SolarSystem(*this);
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Set this instance to match the one passed in.
 * 
 * @param <orig> The object that is being copied.
 */
//---------------------------------------------------------------------------
void SolarSystem::Copy(const GmatBase* orig)
{
   // We dont want to clone the bodies so, just copy
   
   SolarSystem *ss = (SolarSystem*)orig;
   
   pvSrcForAll         = ss->pvSrcForAll;
   anMethodForAll      = ss->anMethodForAll;
   overrideTimeForAll  = ss->overrideTimeForAll;
   ephemUpdateInterval = ss->ephemUpdateInterval;
   bodyStrings         = ss->bodyStrings;
   
   // The SolarSystem has its own PlnetaryEphem files, so do not copy pointers
   //thePlanetaryEphem   = ss->thePlanetaryEphem;
   //theDefaultSlpFile   = ss->theDefaultSlpFile;
   //theDefaultDeFile    = ss->theDefaultDeFile;
   
   // copy the SS bodies
   for (std::vector<CelestialBody*>::const_iterator cbi = ss->bodiesInUse.begin();
        cbi != ss->bodiesInUse.end(); ++cbi)
   {
      CelestialBody *cb = (*cbi);
      CelestialBody *body = FindBody(cb->GetName());
      if (body != NULL)
      {
         body->Copy(cb);
         
         #ifdef DEBUG_SS_COPY
         MessageInterface::ShowMessage
            ("   Copied %s from <%p> to <%p>\n", cb->GetName().c_str(), cb, body);
         #endif
      }
      else
      {
         SolarSystemException ss;
         ss.SetDetails("%s not found in the solar system named "
                       "\"%s\"\n", cb->GetName().c_str(), GetName().c_str());
         throw ss;
      }
   }
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string SolarSystem::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SolarSystemParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer SolarSystem::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < SolarSystemParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType SolarSystem::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SolarSystemParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   
   return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string SolarSystem::GetParameterTypeString(const Integer id) const
{
   return SolarSystem::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer SolarSystem::GetIntegerParameter(const Integer id) const
{
   if (id == NUMBER_OF_BODIES)          return bodiesInUse.size();
   
   return GmatBase::GetIntegerParameter(id); 
}


//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* This method returns the Integer parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer SolarSystem::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real SolarSystem::GetRealParameter(const Integer id) const
{
   if (id == EPHEM_UPDATE_INTERVAL) return ephemUpdateInterval;
   return GmatBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real SolarSystem::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real SolarSystem::SetRealParameter(const Integer id, const Real value)
{
   if (id == EPHEM_UPDATE_INTERVAL)
   {
      SetEphemUpdateInterval(value);
      return true;
   }
   return GmatBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
Real SolarSystem::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label),value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool SolarSystem::GetBooleanParameter(const Integer id) const
{
   if (id == OVERRIDE_TIME_SYSTEM) return overrideTimeForAll;
   return GmatBase::GetBooleanParameter(id); 
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
bool SolarSystem::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
bool SolarSystem::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == OVERRIDE_TIME_SYSTEM)
   {
      SetOverrideTimeSystem(value);
      return true;
   }
   return GmatBase::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
bool SolarSystem::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool SolarSystem::SetStringParameter(const Integer id,
                                     const std::string &value)
{
   if (id == EPHEMERIS)
   {
      StringArray parts = GmatStringUtil::SeparateBy(value, "{}, ");
      
      #ifdef DEBUG_SS_SET
      MessageInterface::ShowMessage
         ("SolarSystem::SetStringParameter() Has %d ephmeris types\n", parts.size());
      for (UnsignedInt i=0; i<parts.size(); i++)
         MessageInterface::ShowMessage
            ("   ephemType[%d] = '%s'\n", i, parts[i].c_str());
      #endif
      
      Integer status = SetPlanetarySourceTypesInUse(parts);
      if (status == 2)
         return true;
      else
         return false;
   }
   
   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool SolarSystem::SetStringParameter(const std::string &label,
                                     const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);   
}


//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& SolarSystem::GetStringArrayParameter(const Integer id) const
{
   if (id == BODIES_IN_USE)
      return bodyStrings;
   else if (id == EPHEMERIS)
      return thePlanetarySourceTypes;
   
   return GmatBase::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& 
SolarSystem::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer GetOwnedObjectCount()
//------------------------------------------------------------------------------
Integer SolarSystem::GetOwnedObjectCount()
{
   return bodiesInUse.size();
}


//------------------------------------------------------------------------------
// GmatBase* GetOwnedObject(Integer whichOne)
//------------------------------------------------------------------------------
GmatBase* SolarSystem::GetOwnedObject(Integer whichOne)
{
   Integer count = bodiesInUse.size();
   if (whichOne < count)
      return bodiesInUse[whichOne];
   
   return NULL;
}


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CelestialBody* FindBody(std::string withName)
//------------------------------------------------------------------------------
/**
* Return a pointer to a CelestialBody with the name withName.
 *
 * @param <withName>  name of body requested.
 *
 * @return pointer to a CelestialBody wiith the requested name.
 */
//------------------------------------------------------------------------------
CelestialBody* SolarSystem::FindBody(std::string withName) 
{
   // Search through bodiesInUse for the body with the name withName
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      if ((*cbi)->GetName() == withName)
      {
         return (*cbi);
      }
      ++cbi;
   }
   
   return NULL;
}


//------------------------------------------------------------------------------
// void SetJ2000Body()
//------------------------------------------------------------------------------
/*
 * Sets J2000Body used for Earth to all bodies in the solar system.
 */
//------------------------------------------------------------------------------
void SolarSystem::SetJ2000Body()
{
   CelestialBody *earth = FindBody(SolarSystem::EARTH_NAME);
   
   if (earth != NULL)
   {
      std::string j2000BodyName = earth->GetJ2000BodyName();
      CelestialBody *j2000Body = FindBody(j2000BodyName);
      if (j2000Body != NULL)
      {
         std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
         while (cbi != bodiesInUse.end())
         {
            (*cbi)->SetJ2000Body(j2000Body);
            ++cbi;
         }
      }
   }
}


//------------------------------------------------------------------------------
// void CloneBodiesInUse(const SolarSystem &ss)
//------------------------------------------------------------------------------
void SolarSystem::CloneBodiesInUse(const SolarSystem &ss)
{
   #ifdef DEBUG_SS_CLONING
   MessageInterface::ShowMessage("   ===> Cloning %d bodies\n", ss.bodiesInUse.size());
   #endif
   
   bodiesInUse.clear();
   
   // clone the SS bodies
   for (std::vector<CelestialBody*>::const_iterator cbi = ss.bodiesInUse.begin();
        cbi != ss.bodiesInUse.end(); ++cbi)
   {
      CelestialBody *cb = (CelestialBody*)((*cbi)->Clone());
      bodiesInUse.push_back(cb);
      
      #ifdef DEBUG_SS_CLONING
      MessageInterface::ShowMessage("   Cloned <%p> %s\n", cb, cb->GetName().c_str());
      #endif
   }
   
   // set references to cloned bodies
   CelestialBody *sun = FindBody(SolarSystem::SUN_NAME);
   CelestialBody *earth = FindBody(SolarSystem::EARTH_NAME);
   
   if (sun != NULL && earth != NULL)
   {
      for (std::vector<CelestialBody*>::const_iterator cbi = bodiesInUse.begin();
           cbi != bodiesInUse.end(); ++cbi)
      {
         CelestialBody *cb = (*cbi);
         cb->SetRefObject(sun, Gmat::CELESTIAL_BODY, sun->GetName());
         cb->SetRefObject(earth, Gmat::CELESTIAL_BODY, earth->GetName());
      }
   }
}


//------------------------------------------------------------------------------
// void DeleteBodiesInUse()
//------------------------------------------------------------------------------
void SolarSystem::DeleteBodiesInUse()
{
   #ifdef DEBUG_SS_CLONING
   MessageInterface::ShowMessage("   ===> Deleting %d bodies\n", bodiesInUse.size());
   #endif
   
   std::vector<CelestialBody*>::iterator cbi = bodiesInUse.begin();
   while (cbi != bodiesInUse.end())
   {
      #ifdef DEBUG_SS_CLONING
      MessageInterface::ShowMessage
         ("   Deleting <%p> %s\n", (*cbi), (*cbi)->GetName().c_str());
      #endif
      
      delete (*cbi);       // delete each body first
      (*cbi) = NULL;
      ++cbi;
   }
   
   bodiesInUse.clear();
}


//------------------------------------------------------------------------------
// void SetDefaultPlanetarySource()
//------------------------------------------------------------------------------
void SolarSystem::SetDefaultPlanetarySource()
{
   #ifdef DEBUG_SS_PLANETARY_FILE
   MessageInterface::ShowMessage("SolarSystem setting default planetary source...\n");
   #endif
   
   // initialize planetary file types/names in use
   // Set DE405 as default
   thePlanetarySourceTypesInUse.clear();
   thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[DE405]);
   thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[ANALYTIC]); 
   thePlanetarySourceTypesInUse.push_back(PLANETARY_SOURCE_STRING[SLP]);
   
   SetPlanetarySourceTypesInUse(thePlanetarySourceTypesInUse);
}


//------------------------------------------------------------------------------
// bool CreateSlpFile(const std::string &fileName)
//------------------------------------------------------------------------------
bool SolarSystem::CreateSlpFile(const std::string &fileName)
{
   #ifdef DEBUG_SS_PLANETARY_FILE
   MessageInterface::ShowMessage
      ("SolarSystem::CreateSlpFile() fileName=%s\n", fileName.c_str());
   #endif
   
   bool status = false;
   
   if (isPlanetarySourceInUse[SLP])
   {
      MessageInterface::ShowMessage
         ("SolarSystem::CreateSlpFile() SlpFile already created\n");
      status = true;
   }
   else
   {
      theDefaultSlpFile = new SlpFile(fileName);
      
      #ifdef DEBUG_SS_PLANETARY_FILE
      MessageInterface::ShowMessage("SolarSystem::CreateSlpFile() SlpFile created\n");
      #endif
      
      if (theDefaultSlpFile != NULL)
         status = true;
   }
   
   return status;
}


//------------------------------------------------------------------------------
// bool CreateDeFile(const Integer id, const std::string &fileName,
//                   Gmat::DeFileFormat format = Gmat::DE_BINARY)
//------------------------------------------------------------------------------
bool SolarSystem::CreateDeFile(Integer id, const std::string &fileName,
                               Gmat::DeFileFormat format)
{
   bool status = false;
   Gmat::DeFileType deFileType;
   
   if (isPlanetarySourceInUse[id])
   {
      MessageInterface::ShowMessage
         ("SolarSystem::CreateDeFile() DeFile already created\n");
      status = true;
   }
   else
   {
      switch (id)
      {
      case DE200:
         deFileType = Gmat::DE200;
         break;
      case DE405:
         deFileType = Gmat::DE405;
         break;
      default:
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "SolarSystem::CreateDeFile() unsupported DE file type");
         return false;
      }
      
      #ifdef DEBUG_SS_PLANETARY_FILE
      MessageInterface::ShowMessage
         ("SolarSystem::CreateDeFile() creating DeFile. type=%d,\n   "
          "fileName=%s, format=%d\n", deFileType, fileName.c_str(),
          format);
      #endif
      
      FILE *defile = fopen(fileName.c_str(), "rb");
      if (defile == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "Error opening DE file:%s. \n"
             "Please check file path. "
             "The next filetype in the list will be used.\n", fileName.c_str());
      }
      else
      {
         fclose(defile);
         
         try
         {
            theDefaultDeFile = new DeFile(deFileType, fileName, format);
            
            if (theDefaultDeFile != NULL)
               status = true;
         }
         catch (...)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_,
                "SolarSystem::CreateDeFile() Error creating %s. "
                "The next filetype in the list will "
                "be created.\n", fileName.c_str());
         }
      }
   }
   return status;
}
