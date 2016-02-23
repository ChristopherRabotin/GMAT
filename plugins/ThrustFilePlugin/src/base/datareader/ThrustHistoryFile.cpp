//------------------------------------------------------------------------------
//                           ThrustHistoryFile
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * 
 */
//------------------------------------------------------------------------------


#include "ThrustHistoryFile.hpp"
#include "ThfDataSegment.hpp"
#include "InterfaceException.hpp"
#include "TextParser.hpp"
#include "StringUtil.hpp"

// Classes/namespaces needed for conversions
#include "TimeSystemConverter.hpp"
#include "GmatConstants.hpp"
#include "FileManager.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_FILE_DATA
//#define DEBUG_FILE_READ



const std::string ThrustHistoryFile::PARAMETER_LABEL[ThrustHistoryFileParamCount - GmatBaseParamCount] =
{
      "FileName",                         // FILENAME
      "AddThrustSegment",                 // SEGMENTS,
};

const Gmat::ParameterType ThrustHistoryFile::PARAMETER_TYPE[ThrustHistoryFileParamCount - GmatBaseParamCount] =
{
      Gmat::FILENAME_TYPE,
      Gmat::STRINGARRAY_TYPE,
};



//------------------------------------------------------------------------------
// ThrustHistoryFile::ThrustHistoryFile(const std::string& theName) :
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param theName Name of this thrust history file reader
 */
//------------------------------------------------------------------------------
ThrustHistoryFile::ThrustHistoryFile(const std::string& theName) :
   FileReader        ("ThrustHistoryFile", theName),
   thrustFileName    ("")
{
   objectTypes.push_back(Gmat::INTERFACE);
   objectTypeNames.push_back("ThrustHistoryFile");
   parameterCount = ThrustHistoryFileParamCount;

   // "BeginThrust and EndThrust are treated separately
   keywords.push_back("Start_Epoch");
   keywords.push_back("Thrust_Vector_Coordinate_System");
   keywords.push_back("Thrust_Vector_Interpolation_Method");
   keywords.push_back("Mass_Flow_Rate_Interpolation_Method");

   dataStartKeys.push_back("ModelThrustOnly");
   dataStartKeys.push_back("ModelThrustAndMassRate");
   dataStartKeys.push_back("ModelAccelOnly");
   dataStartKeys.push_back("ModelAccelAndMassRate");

   interpolationTypes.push_back("None");
   interpolationTypes.push_back("Linear");
   interpolationTypes.push_back("CubicSpline");
   interpolationTypes.push_back("ThrustVectorMethod");

   theForce.SetName(instanceName + "_FileThrust");
}

//------------------------------------------------------------------------------
// ~ThrustHistoryFile()
//------------------------------------------------------------------------------
/**
 * Desturctor
 */
//------------------------------------------------------------------------------
ThrustHistoryFile::~ThrustHistoryFile()
{
}

ThrustHistoryFile::ThrustHistoryFile(const ThrustHistoryFile& thf) :
   FileReader           (thf),
   thrustFileName       (thf.thrustFileName),
   segments             (thf.segments),
   segmentNames         (thf.segmentNames),
   massSources          (thf.massSources),
   keywords             (thf.keywords),
   dataStartKeys        (thf.dataStartKeys),
   interpolationTypes   (thf.interpolationTypes),
   theForce             (thf.theForce)
{
}

ThrustHistoryFile& ThrustHistoryFile::operator =(const ThrustHistoryFile& thf)
{
   if (this != &thf)
   {
      FileReader::operator =(thf);

      thrustFileName = thf.thrustFileName;
      segments = thf.segments;
      segmentNames = thf.segmentNames;
      massSources = thf.massSources;
      keywords = thf.keywords;
      dataStartKeys = thf.dataStartKeys;
      interpolationTypes = thf.interpolationTypes;
      theForce = thf.theForce;
   }

   return *this;
}

GmatBase* ThrustHistoryFile::Clone() const
{
   return new ThrustHistoryFile(*this);
}

bool ThrustHistoryFile::RenameRefObject(Gmat::ObjectType type,
      const std::string& oldname, const std::string& newname)
{
   bool retval = false;

   return retval;
}

//------------------------------------------------------------------------------
// bool ThrustHistoryFile::ReadData()
//------------------------------------------------------------------------------
/**
 * Parser for the Thrust History File
 *
 * @return true if the file parsed correctly
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::ReadData()
{
   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage("Entered ThrustHistoryFile::ReadData()\n");
   #endif

   bool retval = false;
   TextParser parser;

   theStream = new std::ifstream();

   // Check for the file.  If it is not found on teh raw file name, use the
   // file manager to build an alternative path and check there.
   theStream->open(thrustFileName.c_str());
   if (!theStream->is_open())
   {
      FileManager *fm = FileManager::Instance();
      std::string thePath = fm->GetPathname(FileManager::OUTPUT_PATH);
      thePath += thrustFileName;
      theStream->open(thePath.c_str());
   }

   if (theStream->is_open())
   {
      #ifdef DEBUG_FILE_READ
         MessageInterface::ShowMessage("File opened successfully\n");
      #endif

      std::string theLine;
      char filedata[2048];

      #ifdef DEBUG_FILE_DATA
         Integer lineNo = 1;
      #endif

      while (!theStream->eof())
      {
         theStream->getline(filedata, 2048);
         theLine = filedata;
         /// Strip off trailing new line chars
         size_t nlChar = theLine.find("\n");
         if (nlChar != std::string::npos)
            theLine = theLine.substr(0, nlChar);
         nlChar = theLine.find("\r");
         if (nlChar != std::string::npos)
            theLine = theLine.substr(0, nlChar);

         #ifdef DEBUG_FILE_DATA
            MessageInterface::ShowMessage("%3d: %s\n", lineNo, theLine.c_str());
            ++lineNo;
         #endif

         // Each THF contains one or more blocks of data, called segments.
         // Segments start with a "BeginThrust" keyword, followed by the segment
         // name.
         if (theLine.find("BeginThrust") != std::string::npos)
         {
            ThfDataSegment theSegment;
            #ifdef DEBUG_FILE_DATA
               MessageInterface::ShowMessage("Reading Thrust History File "
                     "header information\n");
            #endif

            // Find the segment name
            StringArray chunks = parser.Decompose(theLine, "{}", false, true);
            if (chunks.size() == 2)
            {
               Integer start = chunks[1].find("{");
               Integer end = chunks[1].find("}");
               if ((start != std::string::npos) && (end != std::string::npos))
                  theSegment.segmentName = GmatStringUtil::Trim(
                        chunks[1].substr(start+1, (end - start - 1)));
            }

            #ifdef DEBUG_FILE_DATA
               MessageInterface::ShowMessage("   Segment name: \"%s\"\n",
                     theSegment.segmentName.c_str());
            #endif

            // Following the name come the fields that describe the profile
            bool readingHeader = true;
            bool dataKeywordFound = false;
            do
            {
               theStream->getline(filedata, 2048);
               theLine = filedata;
               /// Strip off trailing new line chars
               size_t nlChar = theLine.find("\n");
               if (nlChar != std::string::npos)
                  theLine = theLine.substr(0, nlChar);
               nlChar = theLine.find("\r");
               if (nlChar != std::string::npos)
                  theLine = theLine.substr(0, nlChar);

               dataKeywordFound = CheckDataStart(theLine, theSegment);
               if (!dataKeywordFound)
                  readingHeader = SetHeaderField(theLine, theSegment);
            } while (readingHeader && !dataKeywordFound);

            // Finally comes the thrust profile.
            if (dataKeywordFound)
            {
               #ifdef DEBUG_FILE_DATA
                  MessageInterface::ShowMessage("   Reading the Thrust profile\n");
               #endif
               bool dataEndFound = ReadThrustProfile(theSegment);
            }
            ValidateSegment(theSegment);
            SetSegmentData(theSegment);
         }
      }
      theStream->close();
      retval = true;
   }
   else
      throw InterfaceException("Failed to open the thrust history file " +
            thrustFileName + "; is the file in the search path?");

   return retval;
}


std::string ThrustHistoryFile::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ThrustHistoryFileParamCount)
      return PARAMETER_LABEL[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}

Integer ThrustHistoryFile::GetParameterID(const std::string& str) const
{
   Integer id = -1;

   for (Integer i = GmatBaseParamCount; i < ThrustHistoryFileParamCount; i++)
    {
       if (str == PARAMETER_LABEL[i - GmatBaseParamCount])
       {
          id = i;
          break;
       }
    }

    if (id != -1)
    {
       #ifdef DEBUG_BURN_PARAM
       MessageInterface::ShowMessage("Burn::GetParameterID() returning %d\n", id);
       #endif

       return id;
    }

    return GmatBase::GetParameterID(str);
}

Gmat::ParameterType ThrustHistoryFile::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ThrustHistoryFileParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}

std::string ThrustHistoryFile::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

std::string ThrustHistoryFile::GetStringParameter(const Integer id) const
{
   if (id == FILENAME)
      return thrustFileName;

   return FileReader::GetStringParameter(id);
}

bool ThrustHistoryFile::SetStringParameter(const Integer id, const char* value)
{
   if (id == FILENAME)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Setting ThrustHistoryFile %s\n", value);
      #endif
      thrustFileName = value;
      return true;
   }

   if (id == SEGMENTS)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Adding ThrustHistoryFile segment %s\n",
               value);
      #endif

      if (find(segmentNames.begin(), segmentNames.end(), value) == segmentNames.end())
         segmentNames.push_back(value);
      return true;
   }

   return FileReader::SetStringParameter(id, value);
}

bool ThrustHistoryFile::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == FILENAME)
   {
      thrustFileName = value;
      return true;
   }

   if (id == SEGMENTS)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Block string %s\n", value.c_str());
      #endif

      if (find(segmentNames.begin(), segmentNames.end(), value) == segmentNames.end())
         segmentNames.push_back(value);
      return true;
   }

   return FileReader::SetStringParameter(id, value);
}

std::string ThrustHistoryFile::GetStringParameter(const Integer id,
      const Integer index) const
{
   return FileReader::GetStringParameter(id, index);
}

bool ThrustHistoryFile::SetStringParameter(const Integer id, const char* value,
      const Integer index)
{
   return FileReader::SetStringParameter(id, value, index);
}

bool ThrustHistoryFile::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   return FileReader::SetStringParameter(id, value, index);
}

const StringArray& ThrustHistoryFile::GetStringArrayParameter(
      const Integer id) const
{
   return FileReader::GetStringArrayParameter(id);
}

const StringArray& ThrustHistoryFile::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   return FileReader::GetStringArrayParameter(id, index);
}

std::string ThrustHistoryFile::GetStringParameter(
      const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}

bool ThrustHistoryFile::SetStringParameter(const std::string& label,
      const char* value)
{
   return SetStringParameter(GetParameterID(label), value);
}

bool ThrustHistoryFile::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}

std::string ThrustHistoryFile::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

bool ThrustHistoryFile::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

const StringArray& ThrustHistoryFile::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

const StringArray& ThrustHistoryFile::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// void SetSegmentData(ThfDataSegment seg)
//------------------------------------------------------------------------------
/**
 *
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
void ThrustHistoryFile::SetSegmentData(ThfDataSegment seg)
{
   bool segmentFound = false;

   for (UnsignedInt i = 0; i < segments.size(); ++i)
      if (segments[i].GetName() == seg.segmentName)
      {
         segments[i].SetDataSegment(seg);
      }

   // Segment was not found, so add it
   if (!segmentFound)
   {
      ThrustSegment newSegment(seg.segmentName);
      newSegment.SetDataSegment(seg);
      segments.push_back(newSegment);
   }
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Reads the thrust history file and prepares the file thrust for use
 *
 * @return true if the object initialized successfully
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::Initialize()
{
   bool retval = false;

   retval = ReadData();
   if (retval)
      theForce.SetSegmentList(&segments);

   return retval;
}

//------------------------------------------------------------------------------
// bool CheckDataStart(std::string theLine, ThfDataSegment &theSegment)
//------------------------------------------------------------------------------
/**
 * Checks for teh data start keywords
 *
 * @param theLine The line from the thrust history file thta is being checked
 * @param theSegment The segment that received the data
 *
 * @return true if a keyword was found, false if not
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::CheckDataStart(std::string theLine,
      ThfDataSegment &theSegment)
{
   bool retval = false;
   for (UnsignedInt i = 0; i < dataStartKeys.size(); ++i)
   {
      if (theLine.find(dataStartKeys[i]) != std::string::npos)
      {
         #ifdef DEBUG_FILE_READ
            MessageInterface::ShowMessage("   Data Start Keyword %s in line \"%s\"\n",
                  dataStartKeys[i].c_str(), theLine.c_str());
         #endif
         // @todo  Sanity check theLine to be sure it's not gorp
         theSegment.modelFlag = dataStartKeys[i];
         retval = true;
      }
   }
   return retval;
}

//------------------------------------------------------------------------------
// bool ThrustHistoryFile::SetHeaderField(std::string theLine,
//       ThfDataSegment &theSegment)
//------------------------------------------------------------------------------
/**
 * Stores segment header data
 *
 * @param theLine The line from the file under analysis
 * @param theSegment The segment that receives the setting
 *
 * @return true if a field keyword was found
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::SetHeaderField(std::string theLine,
      ThfDataSegment &theSegment)
{
   bool retval = false;

   StringArray chunks = GmatStringUtil::SeparateBy(theLine, "=");
   if (chunks.size() == 2)
   {
      std::string temp = GmatStringUtil::Trim(chunks[0]);
      for (UnsignedInt i = 0; i < keywords.size(); ++i)
      {
         if (temp == keywords[i])
         {
            #ifdef DEBUG_FILE_READ
               MessageInterface::ShowMessage("      Header Keyword %s in line \"%s\" "
                     "is setting the field to \"%s\"\n",
                     keywords[i].c_str(), theLine.c_str(),
                     GmatStringUtil::Trim(chunks[1]).c_str());
            #endif
            MapField(temp, GmatStringUtil::Trim(chunks[1]), theSegment);
            retval = true;
         }
      }
      if (!retval)   // Check to be sure there is no block end
      {
         if (theLine.find("EndThrust") != std::string::npos)
            throw InterfaceException("An EndSegment was found for the segment \"" +
                        theSegment.segmentName + "\" before any data was read "
                        "for the segment.");
      }
   }
   return retval;
}

//------------------------------------------------------------------------------
// void MapField(const std::string &key, const std::string &datum,
//          ThfDataSegment &theSegment)
//------------------------------------------------------------------------------
/**
 * Sets the values for the header fields
 *
 * @param key The keyword for the field being set
 * @param datum The value for the field
 * @param theSegment The segment storing the field
 */
//------------------------------------------------------------------------------
void ThrustHistoryFile::MapField(const std::string &key,
      const std::string &datum, ThfDataSegment &theSegment)
{
   if (key == "Start_Epoch")
      theSegment.startEpochString = datum;

   if (key == "Thrust_Vector_Coordinate_System")
      theSegment.csName = datum;

   if (key == "Thrust_Vector_Interpolation_Method")
   {
      if ((find(interpolationTypes.begin(), interpolationTypes.end(), datum) !=
            interpolationTypes.end()) && (datum != "ThrustVectorMethod"))
         theSegment.interpolationMethod = datum;
   }

   if (key == "Mass_Flow_Rate_Interpolation_Method")
   {
      if (find(interpolationTypes.begin(), interpolationTypes.end(), datum) !=
            interpolationTypes.end())
         theSegment.massFlowInterpolationMethod = datum;
   }
}

//------------------------------------------------------------------------------
// bool ReadThrustProfile(ThfDataSegment& theSegment)
//------------------------------------------------------------------------------
/**
 * Fills the thrust profile data in the segment
 *
 * @param theSegment The segment that received the data
 *
 * @return true if at least one profile point was parsed
 */
//------------------------------------------------------------------------------
bool ThrustHistoryFile::ReadThrustProfile(ThfDataSegment& theSegment)
{
   bool retval = true; //false;

   // If mass flow is modeled, the modelFlag will contain the string "MassRate"
   bool includeMass = (theSegment.modelFlag.find("MassRate") == std::string::npos);
   Integer dataCount = (includeMass ? 5 : 4);

   TextParser parser;
   std::stringstream theLine;
   char filedata[2048];

   bool endFound = false;
   while (!theStream->eof() && !endFound)
   {
      theLine.str("");
      theStream->getline(filedata, 2048);
      theLine.str(filedata);
      theLine.clear();

      if (theLine.str().find("EndThrust") != std::string::npos)
      {
         // Find the segment name
         StringArray chunks = parser.Decompose(theLine.str(), "{}", false, true);
         std::string endName;
         if (chunks.size() == 2)
         {
            Integer start = chunks[1].find("{");
            Integer end = chunks[1].find("}");
            if ((start != std::string::npos) && (end != std::string::npos))
               endName = GmatStringUtil::Trim(
                     chunks[1].substr(start+1, (end - start - 1)));

            if (endName != theSegment.segmentName)
               throw InterfaceException("The EndSegment identifies a different segment (" +
                     endName + ") than the name name of the current segment, \"" +
                     theSegment.segmentName + "\"");

            #ifdef DEBUG_FILE_READ
               MessageInterface::ShowMessage("Data end found on line '%s'\n",
                     theLine.str().c_str());
            #endif
            endFound = true;
         }
      }
      // String has to be >= 7 characters long, "0 0 0 0", to have valid data
      else if (theLine.str().length() > 6)
      {
         Real theValues[5];
         ThfDataSegment::ThrustPoint point;
         for (Integer i = 0; i < dataCount; ++i)
            theLine >> theValues[i];

         point.time      = theValues[0];
         point.vector[0] = theValues[1];
         point.vector[1] = theValues[2];
         point.vector[2] = theValues[3];
         if (includeMass)
            point.mdot   = theValues[4];

         theSegment.profile.push_back(point);
         retval = true;
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// FileThrust* GetForce()
//------------------------------------------------------------------------------
/**
 * Returns the transient force pointer for the thrust associated with the file
 *
 * @return The force pointer. Ownership for the force remains with this object.
 */
//------------------------------------------------------------------------------
FileThrust* ThrustHistoryFile::GetForce()
{
   // Make sure that the force has access to the data
   theForce.SetSegmentList(&segments);
   return &theForce;
}

//------------------------------------------------------------------------------
// void ValidateSegment(ThfDataSegment& theSegment)
//------------------------------------------------------------------------------
/**
 * Sanity checks the segment, and throws if there is an issue
 *
 * @param theSegment The segment that is being validated
 */
//------------------------------------------------------------------------------
void ThrustHistoryFile::ValidateSegment(ThfDataSegment& theSegment)
{
   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage("Validating data segment\n");
      MessageInterface::ShowMessage("   Segment Name:  %s\n",
            theSegment.segmentName.c_str());
      MessageInterface::ShowMessage("   Start Epoch:   %s\n",
            theSegment.startEpochString.c_str());
      MessageInterface::ShowMessage("   >>>        :   %.12lf\n",
            theSegment.startEpoch);
      MessageInterface::ShowMessage("   Coord System:  %s\n",
            theSegment.csName.c_str());
      MessageInterface::ShowMessage("   Interpolator:  %s\n",
            theSegment.interpolationMethod.c_str());
      MessageInterface::ShowMessage("   Mass Flow:     %s\n",
            theSegment.massFlowInterpolationMethod.c_str());
      MessageInterface::ShowMessage("   Mode:          %s\n",
            theSegment.modelFlag.c_str());

      MessageInterface::ShowMessage("   Data:\n");
      for (UnsignedInt i = 0; i < theSegment.profile.size(); ++i)
      {
         ThfDataSegment::ThrustPoint point = theSegment.profile[i];
         MessageInterface::ShowMessage("      %lf    [%lf %lf %lf]   %lf\n",
               point.time, point.vector[0], point.vector[1], point.vector[2],
               point.mdot);
      }
   #endif

   // Perform conversions: epochs and vectors
   if (theSegment.startEpochString != "")
   {
      Real utcEpoch =
            TimeConverterUtil::ConvertGregorianToMjd(theSegment.startEpochString);
      theSegment.startEpoch = TimeConverterUtil::Convert(utcEpoch,
            TimeConverterUtil::UTCMJD, TimeConverterUtil::A1MJD);
   }

   GmatEpoch start = theSegment.startEpoch;
   for (UnsignedInt i = 0; i < theSegment.profile.size(); ++i)
   {
      // Convert time in sec to epoch offsets in days
      theSegment.profile[i].time /= GmatTimeConstants::SECS_PER_DAY;
   }

   theSegment.endEpoch = theSegment.startEpoch +
         theSegment.profile[theSegment.profile.size()-1].time;

   // Set the interpolation method types
   if (theSegment.interpolationMethod == "None")
      theSegment.accelIntType = ThfDataSegment::NONE;
   else if (theSegment.interpolationMethod == "Linear")
      theSegment.accelIntType = ThfDataSegment::LINEAR;
   else if (theSegment.interpolationMethod == "CubicSpline")
      theSegment.accelIntType = ThfDataSegment::SPLINE;
   else
      throw InterfaceException("The interpolation method " +
            theSegment.interpolationMethod + " is not a recognized method of "
                  "interpolation in the Thrust History File segment named " +
                  theSegment.segmentName);

   // Set the thrust or acceleration flag
   if ((theSegment.modelFlag == "ModelThrustOnly") ||
       (theSegment.modelFlag == "ModelThrustAndMassRate"))
      theSegment.modelThrust = true;
   else
      theSegment.modelThrust = false;


   // Set the interpolation method types
   if (theSegment.massFlowInterpolationMethod == "None")
      theSegment.massIntType = ThfDataSegment::NONE;
   else if (theSegment.massFlowInterpolationMethod == "Linear")
      theSegment.massIntType = ThfDataSegment::LINEAR;
   else if (theSegment.massFlowInterpolationMethod == "CubicSpline")
      theSegment.massIntType = ThfDataSegment::SPLINE;
   else
      throw InterfaceException("The interpolation method " +
            theSegment.interpolationMethod + " is not a recognized method of "
                  "interpolation in the Thrust History File segment named " +
                  theSegment.segmentName);

   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage("Segment data after processing\n");
      MessageInterface::ShowMessage("   Segment Name:  %s\n",
            theSegment.segmentName.c_str());
      MessageInterface::ShowMessage("   Start Epoch:   %s\n",
            theSegment.startEpochString.c_str());
      MessageInterface::ShowMessage("   >>>        :   %.12lf\n",
            theSegment.startEpoch);
      MessageInterface::ShowMessage("   Coord System:  %s\n",
            theSegment.csName.c_str());
      MessageInterface::ShowMessage("   Interpolator:  %s\n",
            theSegment.interpolationMethod.c_str());
      MessageInterface::ShowMessage("   Mass Flow:     %s\n",
            theSegment.massFlowInterpolationMethod.c_str());
      MessageInterface::ShowMessage("   Mode:          %s\n",
            theSegment.modelFlag.c_str());

      MessageInterface::ShowMessage("   Data:\n");
      for (UnsignedInt i = 0; i < theSegment.profile.size(); ++i)
      {
         ThfDataSegment::ThrustPoint point = theSegment.profile[i];
         MessageInterface::ShowMessage("      %lf    [%lf %lf %lf]   %lf\n",
               point.time, point.vector[0], point.vector[1], point.vector[2],
               point.mdot);
      }
   #endif
}
