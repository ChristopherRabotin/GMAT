//$Id$
//------------------------------------------------------------------------------
//                            SolarFluxReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Farideh Farahnak
// Created: 2014/11/14
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II.
//
/**
 * SolarFluxReader class will be used by the AtomsphereModel class.
 */
//------------------------------------------------------------------------------

#include "SolarFluxReader.hpp"
#include "SolarSystemException.hpp"


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SolarFluxReader()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::SolarFluxReader():
   obsFileName       (""),
   predictFileName   ("")
{
   if (!obsFluxData.empty())
      obsFluxData.clear();

   if (!predictFluxData.empty())
      predictFluxData.clear();

   beg_ObsTag = "BEGIN OBSERVED";
   end_ObsTag = "END OBSERVED";
   begObs = endObs = begData = -1;
   
   line = new char[256];
   memset(line, 0, 256);
   
}


//------------------------------------------------------------------------------
// ~SolarFluxReader()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::~SolarFluxReader(void)
{
   delete [] line;
}


//------------------------------------------------------------------------------
// SolarFluxReader(const SolarFluxReader &)
//------------------------------------------------------------------------------
/**
 * copy constructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::SolarFluxReader(const SolarFluxReader &sfr)
{
   obsFileName = sfr.obsFileName;
   predictFileName = sfr.predictFileName;
   obsFluxData = sfr.obsFluxData;
   predictFluxData = sfr.predictFluxData;
   beg_ObsTag = sfr.beg_ObsTag;
   end_ObsTag = sfr.end_ObsTag;
   begObs = sfr.begObs;
   endObs = sfr.endObs;
   begData = sfr.begData;
   line = sfr.line;

   if (sfr.line != NULL)
   {
      line = new char[256];
      strcpy(line, sfr.line);
   }
}


//------------------------------------------------------------------------------
// SolarFluxReader& SolarFluxReader::operator=(const SolarFluxReader& sfr)
//------------------------------------------------------------------------------
/**
 * assignment operator
 */
//------------------------------------------------------------------------------
SolarFluxReader& SolarFluxReader::operator=(const SolarFluxReader &sfr)
{
   if (this == &sfr)
      return *this;

   obsFileName = sfr.obsFileName;
   predictFileName = sfr.predictFileName;
   obsFluxData = sfr.obsFluxData;
   predictFluxData = sfr.predictFluxData;
   beg_ObsTag = sfr.beg_ObsTag;
   end_ObsTag = sfr.end_ObsTag;
   begObs = sfr.begObs;
   endObs = sfr.endObs;
   begData = sfr.begData;
   line = sfr.line;

   return *this;
}


//------------------------------------------------------------------------------
// SolarFluxReader::FluxData& SolarFluxReader::FluxData::operator=()
//------------------------------------------------------------------------------
/**
* Operator=() for FluxData data structure
*/
//------------------------------------------------------------------------------
inline SolarFluxReader::FluxData& SolarFluxReader::FluxData::operator=(const SolarFluxReader::FluxData &fD)
{
   if (this == &fD)
      return *this;

   epoch = fD.epoch;
   for (Integer j = 0; j < 8; j++)
   {
      kp[j] = fD.kp[j];
      ap[j] = fD.ap[j];
   }
   apAvg = fD.apAvg;
   adjF107 = fD.adjF107;
   adjCtrF107a = fD.adjCtrF107a;
   obsF107 = fD.obsF107;
   obsCtrF107a = fD.obsCtrF107a;
   for (Integer j = 0; j < 9; j++)
      F107a[j] = fD.F107a[j];
   for (Integer j = 0; j < 3; j++)
      apSchatten[j] = fD.apSchatten[j];
   index = fD.index;
   id = fD.id;

   return *this;
}


//------------------------------------------------------------------------------
// bool Open()
//------------------------------------------------------------------------------
/**
 * Opens both Observed and Schatten input file streams.
 *
 * This method will open input file streams
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::Open()
{
   if (!obsFileName.empty())
   {
      inObs.open(obsFileName.c_str(), std::ios_base::in);
   }
   if (!predictFileName.empty())
   {
      inPredict.open(predictFileName.c_str(), std::ios_base::in);
   }

   return true;
}


//------------------------------------------------------------------------------
// bool LoadFluxData()
//------------------------------------------------------------------------------
/**
 * Loads both Observed and Schatten input file data.
 *
 * This method will load input file data.
 * 
 * @param Observed file name and Predicted(Schatten) file name.
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::LoadFluxData(const std::string &obsFile, const std::string &predictFile)
{
   if (!obsFile.empty())
      obsFileName = obsFile;
   if (!predictFile.empty())
      predictFileName = predictFile;

   // Open the files to load
   Open();
   
   if (obsFileName != "")
   {
      if (inObs.is_open())
      {
         inObs.seekg(0, std::ios_base::beg);

         while (true)
         {
            inObs.getline(line, 256);

            if (std::string(line).find(beg_ObsTag) != std::string::npos)
            {
               begObs = inObs.tellg();
            }
            else if (std::string(line).find(end_ObsTag) != std::string::npos)
            {
               endObs = inObs.tellg();
               endObs = endObs - strlen(line) - 2 ;
               break;
            }
         }

         LoadObsData();
      }
      else
      {
         //throw an exception
         throw SolarSystemException("SolarFluxReader: Historic/Observed File " +
               obsFileName + " could not be opened.\n");
      }
   }

   if (predictFileName != "")
   {
      if (inPredict.is_open())
      {
         inPredict.seekg(0, std::ios_base::beg);
         std::string strLine = "";

         while (true)
         {
            inPredict.getline(line, 256);

            strLine = line;
            // if we read the line with "Nominal Timing" and "Early Timing" content, continue reading two more lines to get
            // into the data lines.
            if (strLine.find("NOMINAL TIMING") != std::string::npos && strLine.find("EARLY TIMING") != std::string::npos)
            {
               inPredict.getline(line, 256);
               inPredict.getline(line, 256);

               begData = inPredict.tellg();

               break;
            }
         }

         LoadPredictData();
      }
      else
      {
         //throw an exception
         throw SolarSystemException("SolarFluxReader: The Schatten predict "
               "file " + predictFileName + " could not be opened.\n");
      }
   }

   return(Close());
}


//------------------------------------------------------------------------------
// bool Close()
//------------------------------------------------------------------------------
/**
 * Closes both Observed and Schatten input file streams.
 *
 * This method will close input file streams
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::Close()
{
   if (inObs.is_open())
      inObs.close();
   if (inPredict.is_open())
      inPredict.close();

   return true;
}


//------------------------------------------------------------------------------
// bool LoadObsData()
//------------------------------------------------------------------------------
/**
 * Loads Observed input file data.
 *
 * This method will load Observed input file data.
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::LoadObsData()
{
   Integer hour = 0, minute = 0;
   Real sec = 0.0;

   inObs.seekg(begObs, std::ios_base::beg);
   while (true)
   {
      inObs.getline(line, 256);
      if (inObs.tellg() >= endObs)
         break;

      if (strlen(line) > 8)
      {
         std::istringstream buf(line);
         std::istream_iterator<std::string> beg(buf), end;
         std::vector<std::string> tokens(beg, end);

         FluxData fD;
         Real mjd = ModifiedJulianDate(atoi(tokens[0].c_str()), atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), hour, minute, sec);
         // because it starts from noon, we subtract it by 0.5 to move it back a half a day.
         fD.epoch = mjd - 0.5;

         for (Integer l=0; l<8; l++)
            fD.kp[l] = atof(tokens[tokens.size()-28+l].c_str());
         for (Integer l=0; l<8; l++)
            fD.ap[l] = atof(tokens[tokens.size()-19+l].c_str());
         fD.apAvg = atof(tokens[tokens.size()-19+8].c_str());
         fD.adjF107 = atof(tokens[tokens.size()-7].c_str());
         fD.adjCtrF107a = atof(tokens[tokens.size()-5].c_str());
         fD.obsF107 = atof(tokens[tokens.size()-3].c_str());
         fD.obsCtrF107a = atof(tokens[tokens.size()-2].c_str());
         fD.index = -1;
         for (Integer l = 0; l<9; l++)
            fD.F107a[l] = -1;
         for (Integer l =0; l<3; l++)
            fD.apSchatten[l] = -1;

         obsFluxData.push_back(fD);
      }
   }

   return true;
}


//------------------------------------------------------------------------------
// bool LoadPredictData()
//------------------------------------------------------------------------------
/**
 * Loads Predict input file data.
 *
 * This method will load Predict input file data.
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::LoadPredictData()
{ 
   Integer hour = 0, minute = 0;
   Real sec = 0.0;

   inPredict.seekg(begData, std::ios_base::beg);

   while (!inPredict.eof())
   {
      inPredict.getline(line, 256);
      //last line in file may or may not have "END DATA" tag,
      // if it has, break 
      if(std::string(line).find("END_DATA") != std::string::npos)
         break;

      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> tokens(beg, end);

      FluxData fD;
      Real mjd = ModifiedJulianDate(atoi(tokens[1].c_str()), atoi(tokens[0].c_str()), 0, hour, minute, sec); 
      // because it starts from noon, we subtract it by 0.5 to move it back a half a day.
      fD.epoch = mjd - 0.5;

      for (Integer l=0; l<3; l++)
         fD.F107a[l] = atof(tokens[l+2].c_str());
      fD.apSchatten[0] = atof(tokens[5].c_str());
      for (Integer l=0; l<3; l++)
         fD.F107a[l+3] = atof(tokens[l+6].c_str());
      fD.apSchatten[1] = atof(tokens[9].c_str());
      for (Integer l=0; l<3; l++)
         fD.F107a[l+6] = atof(tokens[l+10].c_str());
      fD.apSchatten[2] = atof(tokens[13].c_str());
      fD.index = -1;
      fD.id = -1;

      for (Integer l = 0; l<8; l++)
         fD.kp[l] = -1;
      for (Integer l=0; l<8; l++)
         fD.ap[l] = -1;
      fD.apAvg = -1;
      fD.adjF107 = -1;
      fD.adjCtrF107a = -1;
      fD.obsF107 = -1;
      fD.obsCtrF107a = -1;

      predictFluxData.push_back(fD);

   }

   std::vector<FluxData>::iterator it = predictFluxData.begin();
   it->index = 0;
   it->id = 0;
   for (it = it+1; it != predictFluxData.end(); it++)
   {
      it->index = (Integer) (it->epoch - (it-1)->epoch);
      it->index += (it-1)->index;
      it->id = (it-1)->id + 1;
   }

   return true;
}

//------------------------------------------------------------------------------
// bool GetInputs()
//------------------------------------------------------------------------------
/**
 * Gets Observed or Predicted input file data.
 *
 * This method will get Observed or Predicted input file data.
 * 
 * @param float
 *
 * @return bool
 */
//------------------------------------------------------------------------------
SolarFluxReader::FluxData SolarFluxReader::GetInputs(GmatEpoch epoch)
{
   Integer index;
   GmatEpoch epoch_1st;
   FluxData fD;

   // Requirement list in google docs:
   // HistoricAndNearTerm source is always given precedence if requested epoch is in the historic data section.  
   // Otherwise, predicted data is used from the predicted source.
   // If requested data is not on a file (regardless of the file type), 
   // use closest data from the requested epoch and issue a single warning message 
   // that data was not available so using closest data and from which file source.
   epoch_1st = obsFluxData.at(0).epoch;
   index = (Integer) (epoch - epoch_1st);
   // if the requested epoch fall beyond the 
   // last item in the obsFluxData, then search in predictFluxData
   if (index >= (Integer) obsFluxData.size())
   {
      epoch_1st = predictFluxData.at(0).epoch;
      index = (Integer) (epoch - epoch_1st) - 1;
      std::vector<FluxData>::iterator it;
      for ( it = predictFluxData.begin(); it != predictFluxData.end(); it++)
      {
         if ( index == it->index)
         {
            fD = predictFluxData[it->id];
            break;
         }
      }

   }
   else
   {
      fD = obsFluxData[index];
      fD.index = index;
   }

   return fD;
}

//------------------------------------------------------------------------------
// PrepareApData(GmatEpoch epoch, Integer index, FluxData &fD)
//------------------------------------------------------------------------------
/**
 * Function that replaces the ap data with data MSISE models need
 *
 * @param fD The data record that contains raw data for the requested epoch
 *
 * @return void
*/
//------------------------------------------------------------------------------
void SolarFluxReader::PrepareApData(SolarFluxReader::FluxData &fD )
{
   FluxData fD_OneBefore = obsFluxData[fD.index - 1];
   FluxData fD_TwoBefore = obsFluxData[fD.index - 2];
   FluxData fD_ThreeBefore = obsFluxData[fD.index - 3];

   // Fill in fD.ap so it contains these data:
   /*  (1) DAILY AP */
   /*  (2) 3 HR AP INDEX FOR CURRENT TIME */
   /*  (3) 3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME */   
   /*  (4) 3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME */
   /*  (5) 3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME */
   /*  (6) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR */
   /*        TO CURRENT TIME */
   /*  (7) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 57 HRS PRIOR */
   /*        TO CURRENT TIME */

//# yy mm dd BSRN ND Kp Kp Kp Kp Kp Kp Kp Kp Sum Ap  Ap  Ap  Ap  Ap  Ap  Ap  Ap  Avg Cp C9 ISN F10.7 Q Ctr81 Lst81 F10.7 Ctr81 Lst81
//# --------------------------------------------------------------------------------------------------------------------------------
//2009 01 12 2394 12  0  0  0  0  0  0  3  0   3   0   0   0   0   0   0   ((2   0)   0 0.0 0   8  67.0 0  66.7  66.2  69.3  68.8  67.9
//2009 01 13 2394 13 10 20 17  0 10 10 13 10  90   (4   7   6   0   4   4))   ((5   4)   4 0.1 0   0  68.2 0  66.7  66.2  70.5  68.8  68.0
//2009 01 14 2394 14 23 20 10 17 23  7 13 13 127   (9   7   4   6   9   3))   (5) (5)   6 0.3 1   0  68.9 0  66.7  66.2  71.2  68.8  68.0
//2009 01 15 2394 15 13 20 10 13  7 20 23 23 130   (5) (7)   4   5   3   7   9  9 (6) 0.3 1   0  68.8 0  66.8  66.2  71.1  68.8  68.1

   fD.ap[2] = fD.ap[0];
   fD.ap[0] = fD.apAvg;
   fD.ap[1] = fD.ap[1];
   
   fD.ap[3] = fD_OneBefore.ap[7];
   fD.ap[4] = fD_OneBefore.ap[6];

   for (Integer i =0; i<6; i++)
      fD.ap[5] += fD_OneBefore.ap[i];
   for (Integer i=0; i<2; i++)
      fD.ap[5] += fD_TwoBefore.ap[i+6];
   fD.ap[5] /= 8;

   for (Integer i =0; i<6; i++)
      fD.ap[6] += fD_TwoBefore.ap[i];
   for (Integer i=0; i<2; i++)
      fD.ap[6] += fD_ThreeBefore.ap[i+6];
   fD.ap[6] /= 8;

   return;
}
