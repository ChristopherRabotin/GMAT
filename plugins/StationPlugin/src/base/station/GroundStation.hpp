//$Id$
//------------------------------------------------------------------------------
//                            GroundStation
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/08/01
// Modified:
//    2010.06.03 Tuan Nguyen
//      - Add AddHardware parameter and verify added hardware
//    2010.03.15 Thomas Grubb
//      - Changed visiblity of PARAMETER_TEXT, PARAMETER_TYPE, and enum from
//        protected to public
//      - Overrode Copy method
//
/**
 * Defines the Groundstation class used to model ground based tracking stations.
 */
//------------------------------------------------------------------------------

#ifndef GroundStation_hpp
#define GroundStation_hpp

#include "StationDefs.hpp"
#include "SpacePoint.hpp"
#include "GroundstationInterface.hpp"
#include "LatLonHgt.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "Hardware.hpp"


class STATION_API GroundStation : public GroundstationInterface
{
public:
   GroundStation(const std::string &itsName);
   virtual ~GroundStation();
   GroundStation(const GroundStation& gs);
   GroundStation& operator=(const GroundStation& gs);

   // all leaf classes derived from GmatBase must supply this Clone method
   virtual GmatBase*       Clone() const;
   virtual void            Copy(const GmatBase* orig);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   // made changes by Tuan Nguyen
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;

   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                        const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                        const std::string &name = "");

   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);


   virtual bool         HasRefObjectTypeArray();
   virtual const        ObjectTypeArray& GetRefObjectTypeArray();


   virtual bool         Initialize();

//   virtual Integer         GetEstimationParameterID(const std::string &param);
//   virtual Integer         SetEstimationParameter(const std::string &param);
   virtual bool            IsEstimationParameterValid(const Integer id);
   virtual Integer         GetEstimationParameterSize(const Integer id);
   virtual Real*           GetEstimationParameterValue(const Integer id);

   virtual bool            IsValidID(const std::string &id);

   DEFAULT_TO_NO_CLONES

protected:
   /// Ground station ID
   std::string          stationId;

   // Added hardware of the ground station
   StringArray	         hardwareNames;       // made changes by Tuan Nguyen
   ObjectArray          hardwareList;        // made changes by Tuan Nguyen
	
	
public:
   /// Published parameters for ground stations
   enum
   {
      STATION_ID = BodyFixedPointParamCount,
      ADD_HARDWARE,								// made changes by Tuan Nguyen
      GroundStationParamCount,
   };

   static const std::string
      PARAMETER_TEXT[GroundStationParamCount - BodyFixedPointParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[GroundStationParamCount - BodyFixedPointParamCount];

private:
   bool 		        VerifyAddHardware();			// made changes by Tuan Nguyen
};

#endif /*GroundStation_hpp*/
