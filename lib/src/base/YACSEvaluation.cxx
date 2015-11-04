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
#include <iomanip>

#include "otgui/YACSEvaluation.hxx"
#include "PersistentObjectFactory.hxx"
#include "Os.hxx"
#include "OTconfig.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(YACSEvaluation);

static Factory<YACSEvaluation> RegisteredFactory("YACSEvaluation");

/* Default constructor */
YACSEvaluation::YACSEvaluation(const String & fileName)
  : NumericalMathEvaluationImplementation()
  , xmlFileName_(fileName)
{
  // Call the initialize() method in order to check the parameters w.r.t. muParser
//   setInputDescription(inputVariablesNames_);
//   setOutputDescription(outputVariablesNames_);
} // YACSEvaluation


/* Virtual constructor */
YACSEvaluation * YACSEvaluation::clone() const
{
  YACSEvaluation * result = new YACSEvaluation(*this);
  return result;
}


/* Comparison operator */
Bool YACSEvaluation::operator ==(const YACSEvaluation & other) const
{
  return true;
}

/* String converter */
String YACSEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << YACSEvaluation::GetClassName()
      << " name=" << getName()
      << " xml=" << xmlFileName_;
  return oss;
}

/* String converter */
String YACSEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getInputDescription() << " xml=" << xmlFileName_;
  return oss;
}


/* Operator () */
NumericalPoint YACSEvaluation::operator() (const NumericalPoint & inP) const
{
  NumericalPoint result(getOutputDimension());
  return result;
}


/* Accessor for input point dimension */
UnsignedInteger YACSEvaluation::getInputDimension() const
{
  return 0;
}

/* Accessor for output point dimension */
UnsignedInteger YACSEvaluation::getOutputDimension() const
{
  return 0;
}

/* Accessor to the input variables names */
Description YACSEvaluation::getInputVariablesNames() const
{
  return Description();
}

/* Accessor to the output variables names */
Description YACSEvaluation::getOutputVariablesNames() const
{
  return Description();
}

/* Accessor to the formulas */
String YACSEvaluation::getXMLFilename() const
{
  return xmlFileName_;
}

/* Method save() stores the object through the StorageManager */
void YACSEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute("xmlFileName_", xmlFileName_);
}

/* Method load() reloads the object from the StorageManager */
void YACSEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute("xmlFileName_", xmlFileName_);
  *this = YACSEvaluation(xmlFileName_);
}

}
