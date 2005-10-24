//$Header$
//------------------------------------------------------------------------------
//                                  DeFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy C. Shoan
// Created: 2004/02/17
//
/**
 * This is the class that handles access to a DE file.
 *
 * @todo : complete Convert method for converting to binary
 *
 * @note For Build 2, conversion from ASCII to Binary not done
 *       (assume  preprocessed).  Also, only 200 and 405 formats are handled.
 *
 * @note Code reused from or based on JPL/JSC (D. Hoffman) code.
 *
 * @note If an ASCII file is input on creation, the file will be converted to
 *       a file in native binary format, for efficiency.  This conversion
 *       assumes that there is an appropriate header file in the same directory
 *       as the ASCII file.  The header file should be called header.FMT, where
 *       FMT is the format (e.g. 200, etc.) of the DE file.
 */
//------------------------------------------------------------------------------
#ifndef DeFile_hpp
#define DeFile_hpp

#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "PlanetaryEphem.hpp"

#include <stdio.h> // for FILE, etc. (for JPL/JSC code (Hoffman))

class DeFile : public PlanetaryEphem
{
public:

   // default constructor
   DeFile(Gmat::DeFileType ofType, std::string fileName = "",
          Gmat::DeFileFormat fmt = Gmat::DE_BINARY);  
   // copy constructor
   DeFile(const DeFile& def);
   // operator=
   DeFile& operator=(const DeFile& sdef);
   // destructor
   ~DeFile();

    //loj: added so that constructor won't throw an exception
   // method to initialize the DeFile - must be done before De file can be read
   bool Initialize();

   // method to return the body ID for the requested body
   Integer  GetBodyID(std::string bodyName);

   // method to return the ASCII file name (unknown if created with
   // BINARY file)
   std::string GetAsciiFileName() const;

   // method to return the BINARY file name
   std::string GetBinaryFileName() const;

   // method to return the type of De File
   Gmat::DeFileType GetDeFileType() const;

   // method to return the position and velocity of the specified body
   // at the specified time
   Real* GetPosVel(Integer forBody, A1Mjd atTime, 
                   bool overrideTimeSystem = false);

   // method to return the day-of-year and year of the start time of the
   // DE file.
   Integer* GetStartDayAndYear();

   // method to convert an ASCII file to a binary file; this method assumes that
   // there is  an appropriate header file in teh ssame directory as the
   // ASCII file.
   std::string Convert(std::string deFileNameAscii); 


   // static values for the IDs for each celestial body, as used on teh DE files.
   static const Integer SUN_ID;
   static const Integer MERCURY_ID;
   static const Integer VENUS_ID;
   static const Integer EARTH_ID;
   static const Integer MOON_ID;
   static const Integer MARS_ID;
   static const Integer JUPITER_ID;
   static const Integer SATURN_ID;
   static const Integer URANUS_ID;
   static const Integer NEPTUNE_ID;
   static const Integer PLUTO_ID;
   static const Integer SS_BARY_ID;
   static const Integer EM_BARY_ID;
   static const Integer NUTATIONS_ID;
   static const Integer LIBRATIONS_ID;

   static const Real    JD_MJD_OFFSET;// = 2430000.0;
   // seconds offset to get from A1 to TDT (TT)
   static const Real    TT_OFFSET;//     = 32.184;

   static const Integer ARRAY_SIZE_200 = 826;
   static const Integer ARRAY_SIZE_405 = 1018;
   static const Integer MAX_ARRAY_SIZE = 1018;

   static const Integer FAILURE        = 1; // from JPL/JSC ephem_types.h
   static const Integer SUCCESS        = 0; // from JPL/JSC ephem_types.h


protected:

   // structs representing the state date (positiona nd velocity)
   // (from JPL/JSC code - Hoffman)
   struct stateData{
         double Position[3];
         double Velocity[3];
      };

   typedef struct stateData stateType;
   
   // structs representing the format of the header records
   // (from JPL/JSC code - Hoffman)
   // wcs - added constructors, and operator=
#pragma pack(push, 1)
   struct recOneData {
      recOneData()  // default constructor
   {
         int i, j;
         for (i=0;i<3;i++)
         {
            strcpy(label[i],"\0");
            timeData[i]  = 0.0;
            libratPtr[i] = 0;
            for (j=0;j<12;j++) coeffPtr[j][i] = 0;
         }
         for (i=0;i<400;i++)    strcpy(constName[i],"\0");
         numConst        = 0;
         AU              = 0.0;
         EMRAT           = 0.0;
         DENUM           = (long int) 0;
   }
      recOneData(const recOneData& r)  // copy constructor
   {
         int i, j;
         for (i=0;i<3;i++)
         {
            strcpy(label[i], r.label[i]);
            timeData[i]  = r.timeData[i];
            libratPtr[i] = r.libratPtr[i];
            for (j=0;j<12;j++) coeffPtr[j][i] = r.coeffPtr[j][i];
         }
         for (i=0;i<400;i++)    strcpy(constName[i],r.constName[i]);
         numConst        = r.numConst;
         AU              = r.AU;
         EMRAT           = r.EMRAT;
         DENUM           = r.DENUM;
   }
      recOneData& operator=(const recOneData& r) 
   {
         int i, j;
         for (i=0;i<3;i++)
         {
            strcpy(label[i], r.label[i]);
            timeData[i]  = r.timeData[i];
            libratPtr[i] = r.libratPtr[i];
            for (j=0;j<12;j++) coeffPtr[j][i] = r.coeffPtr[j][i];
         }
         for (i=0;i<400;i++)    strcpy(constName[i],r.constName[i]);
         numConst        = r.numConst;
         AU              = r.AU;
         EMRAT           = r.EMRAT;
         DENUM           = r.DENUM;
         return *this;
   }
      // data
      char label[3][84];
      char constName[400][6];
      double timeData[3];
      long int numConst;
      double AU;
      double EMRAT;
      long int coeffPtr[12][3];
      long int DENUM;
      long int libratPtr[3];
       long int RSize; //loj: 4/14/04 added
   };

   struct recTwoData {
      // default constructor
      recTwoData()
   {
         int i;
         for (i=0;i<400;i++) constValue[i] = 0.0;
   }
      // copy constructor
      recTwoData(const recTwoData& r)
   {
         int i;
         for (i=0;i<400;i++) constValue[i] = r.constValue[i];
   }
      // operator=
      recTwoData& operator=(const recTwoData& r)
   {
         int i;
         for (i=0;i<400;i++) constValue[i] = r.constValue[i];
         return *this;
   }
      //data
      double constValue[400];
   };
#pragma pack(pop)
    
   typedef struct recOneData recOneType;
   typedef struct recTwoData recTwoType;

   // structs representing the formats of the header records - need ARRAY_SIZE?
   struct headerOne {
         recOneType data;
         char pad[MAX_ARRAY_SIZE*sizeof(double) - sizeof(recOneType)];  // MAX
      };

   struct headerTwo {
      recTwoType data;
      char pad[MAX_ARRAY_SIZE*sizeof(double) - sizeof(recTwoType)];  // MAX
   };

   typedef struct headerOne headOneType;
   typedef struct headerTwo headTwoType;


private:

   /// ASCII file name - will be unknown if DeFile created with binary file
   std::string asciiFileName;

   /// BINARY file name - may be input on cretion, or may be result of
   /// conversion of input ASCIi file; date will be read from this file.
   std::string binaryFileName; 

   /// type of DE file
   Gmat::DeFileType defType;

   /// array size for the file format we're using
   Integer arraySize;

   /// data from JPL/JSC code (Hoffman) ephem_read.c
   headOneType  H1;
   headTwoType  H2;
   recOneType   R1;
   FILE        *Ephemeris_File;
   double       Coeff_Array[MAX_ARRAY_SIZE];   // MAX
   double       T_beg , T_end , T_span;
   double       mFileBeg;
   
   Integer EPHEMERIS; // caps because of reuse from JPL/JSC code (Hoffman)

   

   // method to initialize the DeFile - must be done before De file can be read
   void InitializeDeFile(std::string fName, Gmat::DeFileFormat fileFmt);


   // --------------- methods from JPL/JSC code (Hoffman) ______________________


   /*-------------------------------------------------------------------------*/
   /*  Read_Coefficients      - from JPL/JSC code (Hoffman)                   */
   /*-------------------------------------------------------------------------*/
   void Read_Coefficients( double Time );

   /*-------------------------------------------------------------------------*/
   /*  Initialize_Ephemeris      - from JPL/JSC code (Hoffman)                */
   /*-------------------------------------------------------------------------*/
   int Initialize_Ephemeris( char *fileName );

   /*-------------------------------------------------------------------------*/
   /*  Interpolate_Libration     - from JPL/JSC code (Hoffman)                */
   /*-------------------------------------------------------------------------*/
   void Interpolate_Libration( double Time, int Target, double Libration[3] );

   /*-------------------------------------------------------------------------*/
   /*  Interpolate_Nutation     - from JPL/JSC code (Hoffman)                 */
   /*-------------------------------------------------------------------------*/
   void Interpolate_Nutation( double Time , int Target , double Nutation[2] );

   /*-------------------------------------------------------------------------*/
   /*  Interpolate_Position      - from JPL/JSC code (Hoffman)                */
   /*-------------------------------------------------------------------------*/
   void Interpolate_Position( double Time , int Target , double Position[3] );

   /*-------------------------------------------------------------------------*/
   /*  Interpolate_State     - from JPL/JSC code (Hoffman)                    */
   /*-------------------------------------------------------------------------*/
   void Interpolate_State( double Time , int Target , stateType *p );

   /*-------------------------------------------------------------------------*/
   /*  Find_Value     - from JPL/JSC code (Hoffman)                           */
   /*-------------------------------------------------------------------------*/
   double Find_Value( char    name[]             ,
                             char    name_array[400][6] ,
                             double  value_array[400]   );

   /*-------------------------------------------------------------------------*/
   /**  Gregorian_to_Julian     - from JPL/JSC code (Hoffman)                 */
   /*-------------------------------------------------------------------------*/
   double Gregorian_to_Julian( int     year ,   int     month   ,
                                      int     day  ,   int     hour    ,
                                      int     min  ,   double  seconds );

   /*-------------------------------------------------------------------------*/
   /*  Integer modulo function.     - from JPL/JSC code (Hoffman)             */
   /*-------------------------------------------------------------------------*/
   int mod( int x, int y );

   /*-------------------------------------------------------------------------*/
   /*  Read_File_Line     - from JPL/JSC code (Hoffman)                       */
   /*-------------------------------------------------------------------------*/
   int Read_File_Line( FILE *inFile, int filter, char lineBuffer[82]);

   /*-------------------------------------------------------------------------*/
   /*  Read_Group_Header     - from JPL/JSC code (Hoffman)                    */
   /*-------------------------------------------------------------------------*/
   int Read_Group_Header( FILE *inFile );

   /*-------------------------------------------------------------------------*/
   /*  Warning     - from JPL/JSC code (Hoffman)                              */
   /*-------------------------------------------------------------------------*/

   //extern void Warning( int errorCode );
   


};
#endif // DeFile_hpp
