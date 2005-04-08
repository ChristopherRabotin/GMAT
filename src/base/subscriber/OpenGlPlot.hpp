//$Header$
//------------------------------------------------------------------------------
//                                  OpenGlPlot
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/12/16
//
/**
 * Declares OpenGlPlot class.
 */
//------------------------------------------------------------------------------
#ifndef OpenGlPlot_hpp
#define OpenGlPlot_hpp

#include "Subscriber.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
//#include "CoordinateConverter.hpp"
#include <map>

class OpenGlPlot : public Subscriber
{
public:
   OpenGlPlot(const std::string &name);
   OpenGlPlot(const OpenGlPlot &ogl);
   virtual ~OpenGlPlot(void);

   // methods inherited from Subscriber
   virtual bool Initialize();
   
   UnsignedInt GetColor(const std::string &item, const std::string &scName);
   bool SetColor(const std::string &item, const std::string &scName,
                 const UnsignedInt value);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   virtual bool SetName(const std::string &who);
   
   virtual bool TakeAction(const std::string &action,  
                           const std::string &actionData = "");
   
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool        IsParameterReadOnly(const Integer id) const;

   virtual Integer GetIntegerParameter(const Integer id) const;
   virtual Integer GetIntegerParameter(const std::string &label) const;
   virtual Integer SetIntegerParameter(const Integer id, const Integer value);
   virtual Integer SetIntegerParameter(const std::string &label,
                                       const Integer value);
   
   virtual UnsignedInt SetUnsignedIntParameter(const Integer id,
                                               const UnsignedInt value,
                                               const Integer index);
   virtual UnsignedInt SetUnsignedIntParameter(const std::string &label,
                                               const UnsignedInt value,
                                               const Integer index);
   virtual const UnsignedIntArray&
           GetUnsignedIntArrayParameter(const Integer id) const; 
   virtual const UnsignedIntArray& 
           GetUnsignedIntArrayParameter(const std::string &label) const;
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
   
   virtual bool SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value,
                                   const Integer index);
   
   virtual const StringArray& GetStringArrayParameter(const Integer id) const;
   virtual const StringArray& GetStringArrayParameter(const std::string &label) const;

   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);
   
   virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");

protected:
   
   bool AddSpacecraft(const std::string &name, Integer index);
   bool ClearSpacecraftList();
   bool RemoveSpacecraft(const std::string &name);
   Integer FindIndexOfElement(StringArray &labelArray,
                              const std::string &label);

   SolarSystem *mSolarSystem;
   CoordinateSystem *mOutCoordSystem;
//     CoordinateConverter mCoordConverter;
   
   bool mDrawEquatorialPlane;
   bool mDrawWireFrame;
   bool mDrawTarget;
   bool mOverlapPlot;
   bool mNeedCoordSysConversion;
   
   std::string mOldName;
   std::string mCoordSysName; //loj: 1/27/05 Added
   
   Integer mDataCollectFrequency;
   Integer mUpdatePlotFrequency;
   
   Integer mNumData;
   Integer mNumCollected;
   
   Integer mScCount;
   StringArray mScNameArray;
   StringArray mAllRefObjectNames;
   
   RealArray mScXArray;
   RealArray mScYArray;
   RealArray mScZArray;
   UnsignedIntArray mOrbitColorArray;
   UnsignedIntArray mTargetColorArray;
   
   std::map<std::string, UnsignedInt> mOrbitColorMap;
   std::map<std::string, UnsignedInt> mTargetColorMap;
   
   enum
   {
      ADD = SubscriberParamCount,
      ORBIT_COLOR,
      TARGET_COLOR,
      EQUATORIAL_PLANE,
      WIRE_FRAME,
      TARGET_STATUS,
      COORD_SYSTEM,
      OVERLAP_PLOT,
      DATA_COLLECT_FREQUENCY,
      UPDATE_PLOT_FREQUENCY,
      OpenGlPlotParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[OpenGlPlotParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[OpenGlPlotParamCount - SubscriberParamCount];
   
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real * dat, Integer len);
   
   const static int MAX_SC_COLOR = 15;
   static const UnsignedInt DEFAULT_ORBIT_COLOR[MAX_SC_COLOR];
   
};

#endif
