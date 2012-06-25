//$Id$
//------------------------------------------------------------------------------
//                                  OrbitPlot
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number X-XXXX-X
//
// Author: Linda Jun
// Created: 2011/05/23
//
/**
 * Declares OrbitPlot class.
 */
//------------------------------------------------------------------------------
#ifndef OrbitPlot_hpp
#define OrbitPlot_hpp

#include "Subscriber.hpp"
#include "SpacePoint.hpp"
#include "CoordinateSystem.hpp"
#include <map>

class GMAT_API OrbitPlot : public Subscriber
{
public:
   OrbitPlot(const std::string &type, const std::string &name);
   OrbitPlot(const OrbitPlot &op);
   OrbitPlot& operator=(const OrbitPlot&);
   virtual ~OrbitPlot();
   
   const StringArray&   GetSpacePointList();
   const StringArray&   GetSpacecraftList();
   const StringArray&   GetNonSpacecraftList();
   
   UnsignedInt          GetColor(const std::string &item, const std::string &objName);
   bool                 SetColor(const std::string &item, const std::string &objName,
                                 UnsignedInt value);
   bool                 GetShowObject(const std::string &name);
   void                 SetShowObject(const std::string &name, bool value);
   
   // methods inherited from Subscriber
   virtual void         Activate(bool state = true);
   
   // methods inherited from GmatBase
   virtual bool         Validate();
   virtual bool         Initialize();
   
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         SetName(const std::string &who,
                                const std::string &oldName = "");
   
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   // methods for parameters
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id, const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual bool         SetStringParameter(const Integer id, const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   
   virtual UnsignedInt  SetUnsignedIntParameter(const Integer id,
                                                const UnsignedInt value,
                                                const Integer index);
   
   virtual const UnsignedIntArray&
                        GetUnsignedIntArrayParameter(const Integer id) const;   
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   
   virtual const BooleanArray&
                        GetBooleanArrayParameter(const Integer id) const;
   virtual const BooleanArray&
                        GetBooleanArrayParameter(const std::string &label) const;
   virtual bool         SetBooleanArrayParameter(const Integer id,
                                                 const BooleanArray &valueArray);
   virtual bool         SetBooleanArrayParameter(const std::string &label,
                                                 const BooleanArray &valueArray);
   
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   
   // for GUI population
   virtual Gmat::ObjectType
                        GetPropertyObjectType(const Integer id) const;
   
protected:
   
   CoordinateSystem *mViewCoordSystem;
   
   std::string mOldName;
   std::string mViewCoordSysName;
   
   // object names and arrays
   std::vector<SpacePoint*> mObjectArray;
   std::vector<SpacePoint*> mAllSpArray;
   BooleanArray mDrawOrbitArray;
   BooleanArray mDrawObjectArray;
   
   StringArray mScNameArray;
   StringArray mObjectNameArray;
   StringArray mAllSpNameArray;
   StringArray mAllRefObjectNames;
   
   Integer mAllSpCount;
   Integer mScCount;
   Integer mObjectCount;
   Integer mNonStdBodyCount;
   /// It uses predefined colors up to 15 objects, after 15 it uses red
   Integer mDefaultColorCount;
   
   // for data control
   Integer mDataCollectFrequency;
   Integer mUpdatePlotFrequency;
   Integer mNumPointsToRedraw;
   Integer mNumData;
   Integer mNumCollected;
   bool    mDrawingStatusChanged;
   
   // arrays for holding distributed data
   RealArray mScXArray;
   RealArray mScYArray;
   RealArray mScZArray;
   RealArray mScVxArray;
   RealArray mScVyArray;
   RealArray mScVzArray;
   
   // arrays for holding object colors
   UnsignedIntArray mScOrbitColorArray;
   UnsignedIntArray mScTargetColorArray;
   UnsignedIntArray mOrbitColorArray;
   UnsignedIntArray mTargetColorArray;
   
   // arrays for holding solver current data
   std::vector<StringArray> mCurrScArray;
   std::vector<Real> mCurrEpochArray;
   std::vector<RealArray> mCurrXArray;
   std::vector<RealArray> mCurrYArray;
   std::vector<RealArray> mCurrZArray;
   std::vector<RealArray> mCurrVxArray;
   std::vector<RealArray> mCurrVyArray;
   std::vector<RealArray> mCurrVzArray;
   
   // maps for object and color, flag, epoch
   std::map<std::string, UnsignedInt> mOrbitColorMap;
   std::map<std::string, UnsignedInt> mTargetColorMap;
   std::map<std::string, bool> mDrawOrbitMap;
   std::map<std::string, bool> mShowObjectMap;
   std::map<std::string, Real> mScInitialEpochMap;
   
   /// Calls PlotInterface for plotting non-solver data  
   virtual bool         UpdateData(const Real *dat, Integer len);
   
   /// Calls PlotInterface for plotting solver data
   virtual bool         UpdateSolverData();
   
   /// Buffers published spacecraft orbit data
   void                 BufferZeroData(Integer scIndex);
   virtual Integer      BufferOrbitData(const Real *dat, Integer len);
   
   /// Adds Spacecraft and other objects to object arrays and color maps
   bool                 AddSpacePoint(const std::string &name, Integer index,
                                      bool show = true);
   /// Clears all object arrays and color maps, called from TakeAction("Clear")
   bool                 ClearSpacePointList();
   /// Removes SpacePoint object from the object and color array, called from TakeAction("Remove")
   bool                 RemoveSpacePoint(const std::string &name);
   /// Finds the index of the element label from the element label array.
   Integer              FindIndexOfElement(StringArray &labelArray,
                                           const std::string &label);
   /// Builds dynamic arrays to pass to plotting canvas
   void                 BuildDynamicArrays();
   /// Clears dynamic arrays such as object name array, color array, etc.
   void                 ClearDynamicArrays();
   /// Updates plotting object information such as plotting object pointer, option flags, etc.
   void                 UpdateObjectList(SpacePoint *sp, bool show = false);
   
   /// Debug message
   void                 WriteCoordinateSystem(CoordinateSystem *cs,
                                              const std::string &label = "");
   
   enum
   {
      ADD = SubscriberParamCount,
      COORD_SYSTEM,
      DRAW_OBJECT,
      ORBIT_COLOR,
      TARGET_COLOR,
      DATA_COLLECT_FREQUENCY,
      UPDATE_PLOT_FREQUENCY,
      NUM_POINTS_TO_REDRAW,
      SHOW_PLOT,
      OrbitPlotParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[OrbitPlotParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[OrbitPlotParamCount - SubscriberParamCount];
   
   const static int MAX_SP_COLOR = 15;
   static const UnsignedInt DEFAULT_ORBIT_COLOR[MAX_SP_COLOR];
   
};

#endif
