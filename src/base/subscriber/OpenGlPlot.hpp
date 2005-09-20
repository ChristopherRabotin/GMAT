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
#include "SpacePoint.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include <map>

class OpenGlPlot : public Subscriber
{
public:
   OpenGlPlot(const std::string &name);
   OpenGlPlot(const OpenGlPlot &ogl);
   virtual ~OpenGlPlot(void);
   
   const StringArray& GetSpacePointList();
   const StringArray& GetSpacecraftList();
   const StringArray& GetNonSpacecraftList();
   
   UnsignedInt GetColor(const std::string &item, const std::string &scName);
   bool SetColor(const std::string &item, const std::string &name,
                 UnsignedInt value);
   bool GetShowObject(const std::string &name);
   void SetShowObject(const std::string &name, bool value);
   
   // methods inherited from Subscriber
   virtual bool Initialize();
   
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
   
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
   
   virtual const Rvector& GetRvectorParameter(const Integer id) const;
   virtual const Rvector& GetRvectorParameter(const std::string &label) const;
   virtual const Rvector& SetRvectorParameter(const Integer id,
                                              const Rvector &value);
   virtual const Rvector& SetRvectorParameter(const std::string &label,
                                              const Rvector &value);
   
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
      
   virtual const StringArray&
                 GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                 GetStringArrayParameter(const std::string &label) const;
   
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);
   
   virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");
   
protected:
   
   bool AddSpacePoint(const std::string &name, Integer index);
   bool ClearSpacePointList();
   bool RemoveSpacePoint(const std::string &name);
   Integer FindIndexOfElement(StringArray &labelArray,
                              const std::string &label);
   
   void ClearDynamicArrays();
   void UpdateObjectList(SpacePoint *sp);
   
   SolarSystem *mSolarSystem;
   CoordinateSystem *mViewCoordSystem;
   CoordinateSystem *mViewUpCoordSystem;
   SpacePoint *mViewCoordSysOrigin;
   SpacePoint *mViewPointRefObj;
   SpacePoint *mViewPointVectorObj;
   SpacePoint *mViewDirectionObj;
   std::vector<SpacePoint*> mObjectArray;
   std::vector<SpacePoint*> mAllSpArray;
   std::vector<bool> mDrawOrbitArray;
   std::vector<bool> mShowObjectArray;
   
   std::string mEclipticPlane;
   std::string mEquatorialPlane;
   std::string mWireFrame;
   std::string mTargetStatus;
   std::string mOverlapPlot;
   std::string mLockView;
   std::string mPerspectiveMode;
   std::string mUseFixedFov;
   std::string mAxes;
   std::string mEarthSunLines;
   
   std::string mOldName;
   std::string mViewCoordSysName;
   std::string mViewPointRefName;
   std::string mViewPointVectorName;
   std::string mViewDirectionName;
   std::string mViewUpCoordSysName;
   std::string mViewUpAxisName;
   
   Rvector3 mViewPointRefVector;
   Rvector3 mViewPointVector;
   Rvector3 mViewDirectionVector;
   
   Real mViewScaleFactor;
   Real mFixedFovAngle;
   
   Integer mDataCollectFrequency;
   Integer mUpdatePlotFrequency;
   Integer mNumPointsToRedraw;
   Integer mNumData;
   Integer mNumCollected;
   
   Integer mAllSpCount;
   Integer mScCount;
   Integer mObjectCount;
   Integer mNonStdBodyCount;
   
   StringArray mScNameArray;
   StringArray mObjectNameArray;
   StringArray mAllSpNameArray;
   StringArray mAllRefObjectNames;
   
   RealArray mScXArray;
   RealArray mScYArray;
   RealArray mScZArray;
   RealArray mScVxArray;
   RealArray mScVyArray;
   RealArray mScVzArray;
   UnsignedIntArray mScOrbitColorArray;
   UnsignedIntArray mScTargetColorArray;
   UnsignedIntArray mOrbitColorArray;
   
   std::map<std::string, UnsignedInt> mOrbitColorMap;
   std::map<std::string, UnsignedInt> mTargetColorMap;
   std::map<std::string, bool> mDrawOrbitMap;
   std::map<std::string, bool> mShowObjectMap;
   
   enum
   {
      ADD = SubscriberParamCount,
      COORD_SYSTEM,
      VIEWPOINT_REF,
      VIEWPOINT_REF_VECTOR,
      VIEWPOINT_VECTOR,
      VIEWPOINT_VECTOR_VECTOR,
      VIEW_DIRECTION,
      VIEW_DIRECTION_VECTOR,
      VIEW_SCALE_FACTOR,
      FIXED_FOV_ANGLE,
      VIEW_UP_COORD_SYSTEM,
      VIEW_UP_AXIS,
      CELESTIAL_PLANE,
      EQUATORIAL_PLANE,
      WIRE_FRAME,
      TARGET_STATUS,
      AXES,
      EARTH_SUN_LINES,
      OVERLAP_PLOT,
      USE_VIEWPOINT_INFO,
      PERSPECTIVE_MODE,
      USE_FIXED_FOV,
      DATA_COLLECT_FREQUENCY,
      UPDATE_PLOT_FREQUENCY,
      NUM_POINTS_TO_REDRAW,
      ORBIT_COLOR,
      //TARGET_COLOR,
      OpenGlPlotParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[OpenGlPlotParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[OpenGlPlotParamCount - SubscriberParamCount];
   
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real * dat, Integer len);
   
   const static int MAX_SP_COLOR = 15;
   static const UnsignedInt DEFAULT_ORBIT_COLOR[MAX_SP_COLOR];
   
};

#endif
