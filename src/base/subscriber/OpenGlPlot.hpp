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
#include <map>

class OpenGlPlot : public Subscriber
{
public:
   OpenGlPlot(const std::string &name);
   OpenGlPlot(const OpenGlPlot &ogl);
   virtual ~OpenGlPlot(void);

   // methods inherited from Subscriber
   virtual bool Initialize(); //loj: 3/8/04 added
    
   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

   // methods inherited from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual bool GetBooleanParameter(const Integer id) const;
   virtual bool SetBooleanParameter(const Integer id, const bool value);

   //virtual UnsignedInt GetUnsignedIntParameter(const Integer id) const;
   UnsignedInt GetUnsignedIntParameter(const Integer id, const std::string &item);
   //virtual UnsignedInt GetUnsignedIntParameter(const std::string &label) const;
   UnsignedInt GetUnsignedIntParameter(const std::string &label,
                                       const std::string &item);
   UnsignedInt SetUnsignedIntParameter(const Integer id, const std::string &item,
                                       const UnsignedInt value);
   //virtual UnsignedInt SetUnsignedIntParameter(const Integer id,
   //                                     const UnsignedInt value);
   //virtual UnsignedInt SetUnsignedIntParameter(const std::string &label,
   //                                     const UnsignedInt value);
   //loj: 6/2/04 need to add to GmatBase?
   UnsignedInt  SetUnsignedIntParameter(const std::string &label,
                                        const std::string &item,
                                        const UnsignedInt value);

   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
//     virtual std::string GetStringParameter(const std::string &label,
//                                            const std::string &item) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
//     //loj: 6/2/04 need to add to GmatBase
//     bool SetStringParameter(const std::string &label,
//                             const std::string &item,
//                             const std::string &value);
   
   virtual const StringArray& GetStringArrayParameter(const Integer id) const;
   virtual const StringArray& GetStringArrayParameter(const std::string &label) const;

protected:

   bool AddSpacecraft(const std::string &name);
   void ClearSpacecraftList();
   
   bool mDrawAxis;
   bool mDrawEquatorialPlane;
   bool mDrawWireFrame;
    
   Integer mDataCollectFrequency;
   Integer mUpdatePlotFrequency;

   Integer mNumData;
   Integer mNumCollected;
   
   Integer mScCount;
   StringArray mScNameArray;
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
      SPACECRAFT_LIST,
      CLEAR_SPACECRAFT_LIST,
      ORBIT_COLOR,
      TARGET_COLOR,
      DRAW_AXIS,
      DRAW_EQUATORIAL_PLANE,
      WIRE_FRAME,
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
};

#endif
