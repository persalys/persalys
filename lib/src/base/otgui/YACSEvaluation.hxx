//                                               -*- C++ -*-
/**
 * @brief The class that implements the evaluation of an analytical function.
 *
 * Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#ifndef OTGUI_YACSEVALUATION_HXX
#define OTGUI_YACSEVALUATION_HXX

#include "NumericalMathEvaluationImplementation.hxx"
#include "Pointer.hxx"
#include "OTGuiprivate.hxx"

#include "YACSEvalYFX.hxx"
#include "YACSEvalSession.hxx"

namespace OTGUI {

/**
 * @class YACSEvaluation
 *
 * The class that implement the evaluation of an analytical function.
 */
class OTGUI_API YACSEvaluation
  : public OT::NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  YACSEvaluation(const std::string & xmlFileName = "");

  YACSEvaluation(const YACSEvaluation & other);

  /** Virtual constructor */
  virtual YACSEvaluation * clone() const;

  virtual ~YACSEvaluation();

  /** Comparison operator */
  OT::Bool operator ==(const YACSEvaluation & other) const;

  /** String converter */
  virtual std::string __repr__() const;
  virtual std::string __str__(const std::string & offset = "") const;

protected:
  /** Method loadData() loads the data from the xmlFileName */
  void loadData();

public:
  /** Operator () */
  virtual OT::NumericalPoint operator() (const OT::NumericalPoint & inP) const;
  virtual OT::NumericalSample operator() (const OT::NumericalSample & inS) const;

  /** Accessor for input values */
  OT::NumericalPoint getInputValues() const;
  void setInputValues(const OT::NumericalPoint & inP);
  /** Accessor to the input variables names */
  OT::Description getInputVariablesNames() const;
  /** Accessor for input point dimension */
  OT::UnsignedInteger getInputDimension() const;

  /** Accessor to the output variables names */
  OT::Description getOutputVariablesNames() const;
  void setOutputVariablesNames(const OT::Description & outDescription);
  /** Accessor for output point dimension */
  OT::UnsignedInteger getOutputDimension() const;

  /** Accessor to the xmlFileName */
  std::string getXMLFileName() const;
  void setXMLFileName(const std::string & xmlFileName);

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:

private:
  static YACSEvalSession * session_;
  std::string xmlFileName_;
  YACSEvalYFX * efx_;
  OT::NumericalPoint inputValues_;
  OT::Description inDescription_;
  OT::Description outDescription_;
};
}
#endif
