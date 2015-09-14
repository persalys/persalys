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
  YACSEvaluation(const OT::String & xmlFileName = "");

  /** Virtual constructor */
  virtual YACSEvaluation * clone() const;

  /** Comparison operator */
  OT::Bool operator ==(const YACSEvaluation & other) const;

  /** String converter */
  virtual OT::String __repr__() const;
  virtual OT::String __str__(const OT::String & offset = "") const;

  /** Operator () */
  virtual OT::NumericalPoint operator() (const OT::NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  OT::UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  OT::UnsignedInteger getOutputDimension() const;

  /** Accessor to the input variables names */
  OT::Description getInputVariablesNames() const;

  /** Accessor to the output variables names */
  OT::Description getOutputVariablesNames() const;

  /** Accessor to the formulas */
  OT::String getXMLFilename() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:

private:
  OT::String xmlFileName_;

}; /* class YACSEvaluation */


}

#endif /* OTGUI_YACSEVALUATION_HXX */
