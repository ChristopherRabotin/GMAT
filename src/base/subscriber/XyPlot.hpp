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

class XyPlot : public Subscriber
{
public:
   XyPlot(const std::string &name, Parameter *xParam = NULL,
          Parameter *firstYParam = NULL, const std::string &plotTitle = "",
          const std::string &xAxisTitle = "", const std::string &yAxisTitle = "",
          bool drawGrid = false);
   XyPlot(const XyPlot &copy);
   virtual ~XyPlot(void);

   // methods inherited from Subscriber
   virtual bool Initialize();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   virtual bool SetName(const std::string &who); //loj: 11/19/04 - added
   
   virtual bool TakeAction(const std::string &action,  
                           const std::string &actionData = "");
   
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual Integer GetIntegerParameter(const Integer id) const;
   virtual Integer GetIntegerParameter(const std::string &label) const;
   virtual Integer SetIntegerParameter(const Integer id, const Integer value);
   virtual Integer SetIntegerParameter(const std::string &label,
                                       const Integer value);
      
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

   //loj: 11/9/04 added
   virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name = "");

   virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);

protected:

   bool SetXParameter(const std::string &paramName);
   bool AddYParameter(const std::string &paramName, Integer index);
   void BuildPlotTitle();
   bool ClearYParameters();
   bool RemoveYParameter(const std::string &name);
   void DeletePlotCurves();
    
   Parameter *mXParam;
   std::vector<Parameter*> mYParams;

   Integer mNumXParams;
   Integer mNumYParams;

   std::string mXParamName;
   StringArray mYParamNames;
   StringArray mAllParamNames;

   std::string mOldName;
   std::string mPlotTitle;
   std::string mXAxisTitle;
   std::string mYAxisTitle;
   bool mDrawGrid;
   bool mDrawTarget;
   bool mIsXyPlotWindowSet;
    
   Integer mDataCollectFrequency;
   Integer mUpdatePlotFrequency;
    
   Integer mNumDataPoints;
   Integer mNumCollected;
   
   enum
   {
      IND_VAR = SubscriberParamCount,
      ADD,
      PLOT_TITLE,
      X_AXIS_TITLE,
      Y_AXIS_TITLE,
      DRAW_GRID,
      TARGET_STATUS,
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
