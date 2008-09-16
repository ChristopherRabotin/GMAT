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
#ifndef TsPlot_hpp
#define TsPlot_hpp

#include "Subscriber.hpp"
#include "Parameter.hpp"

class TsPlot : public Subscriber 
{
public:
   TsPlot(const std::string &name, Parameter *xParam = NULL,
          Parameter *firstYParam = NULL, const std::string &plotTitle = "",
          const std::string &xAxisTitle = "", const std::string &yAxisTitle = "");
   TsPlot(const TsPlot &orig);
   TsPlot& operator=(const TsPlot& orig);
   virtual ~TsPlot(void);
   
   // methods inherited from Subscriber
   virtual bool         Initialize();
   
   // methods inherited from GmatBase
   virtual GmatBase*    Clone(void) const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         SetName(const std::string &who,
                                const std::string &oldName = "");
   
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   
   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id, 
                                          const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label, 
                                          const std::string &value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   
protected:

   bool SetXParameter(const std::string &paramName);
   bool AddYParameter(const std::string &paramName, Integer index);
   void BuildPlotTitle();
   bool ClearYParameters();
   bool RemoveYParameter(const std::string &name);
   bool ResetYParameters();
   bool PenUp();
   bool PenDown();
   
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
   std::string mDrawGrid;
   bool mIsTsPlotWindowSet;
   
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
      DATA_COLLECT_FREQUENCY,
      UPDATE_PLOT_FREQUENCY,
      SHOW_PLOT,
      TsPlotParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[TsPlotParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[TsPlotParamCount - SubscriberParamCount];

   // methods inherited from Subscriber
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real * dat, Integer len);

};

#endif
