//$Header$
//------------------------------------------------------------------------------
//                                  XyPlot
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/22
//
/**
 * Declares XyPlot class.
 */
//------------------------------------------------------------------------------
#ifndef XyPlot_hpp
#define XyPlot_hpp

#include "Subscriber.hpp"
#include "Parameter.hpp"
#include <map>

class XyPlot : public Subscriber
{
public:
   XyPlot(const std::string &name, Parameter *xParam = NULL,
          Parameter *firstYParam = NULL, const std::string &plotTitle = "",
          const std::string &xAxisTitle = "", const std::string &yAxisTitle = "",
          bool drawGrid = false);
   XyPlot(const XyPlot &copy);
   virtual ~XyPlot(void);

   Integer GetNumYParameters();
    
   bool SetXParameter(const std::string &paramName);
   //bool SetXParameter(Parameter *param);
    
   bool AddYParameter(const std::string &paramName);
   //bool AddYParameter(Parameter *param);

   // methods inherited from Subscriber
   virtual bool Initialize();
    
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual bool GetBooleanParameter(const Integer id) const;
   virtual bool GetBooleanParameter(const std::string &label) const;
   virtual bool SetBooleanParameter(const Integer id, const bool value);
   virtual bool SetBooleanParameter(const std::string &label,
                                    const bool value);
    
   UnsignedInt GetUnsignedIntParameter(const Integer id, const std::string &item);
   UnsignedInt GetUnsignedIntParameter(const std::string &label,
                                       const std::string &item);
   UnsignedInt SetUnsignedIntParameter(const Integer id, const std::string &item,
                                       const UnsignedInt value);
   UnsignedInt  SetUnsignedIntParameter(const std::string &label,
                                        const std::string &item,
                                        const UnsignedInt value);
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
    
   virtual const StringArray& GetStringArrayParameter(const Integer id) const;
   virtual const StringArray& GetStringArrayParameter(const std::string &label) const;

protected:

   void BuildPlotTitle();
   void ClearYParameters();
   void DeletePlotCurves();
    
   Parameter *mXParam;
   std::vector<Parameter*> mYParams; //loj: 6/4/04 remove this later
   std::map<std::string, Parameter*> mYParamMap;

   Integer mNumXParams;
   Integer mNumYParams;

   std::string mXParamName;
   StringArray mYParamNames;
    
   std::string mPlotTitle;
   std::string mXAxisTitle;
   std::string mYAxisTitle;
   bool mDrawGrid;
   bool mIsXyPlotWindowSet;
    
   Integer mDataCollectFrequency;
   Integer mUpdatePlotFrequency;
    
   Integer mNumDataPoints;
   Integer mNumCollected;
   
   std::map<std::string, UnsignedInt> mColorMap;
    
   enum
   {
      IND_VAR = SubscriberParamCount,
      ADD,
      DEP_VAR,
      DEP_VAR_LIST,
      CLEAR_DEP_VAR_LIST,
      COLOR,
      PLOT_TITLE,
      X_AXIS_TITLE,
      Y_AXIS_TITLE,
      DRAW_GRID,
      DATA_COLLECT_FREQUENCY,
      UPDATE_PLOT_FREQUENCY,
      XyPlotParamCount
   };
    
   static const Gmat::ParameterType
      PARAMETER_TYPE[XyPlotParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[XyPlotParamCount - SubscriberParamCount];

   // methods inherited from Subscriber
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real * dat, Integer len);

};

#endif
