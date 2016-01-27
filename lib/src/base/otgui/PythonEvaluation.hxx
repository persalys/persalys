//                                               -*- C++ -*-

#ifndef OTGUI_PYTHONEVALUATION_HXX
#define OTGUI_PYTHONEVALUATION_HXX

#include "NumericalMathEvaluationImplementation.hxx"
#include "Pointer.hxx"
#include "OTGuiprivate.hxx"

namespace OTGUI {

/**
 * @class PythonEvaluation
 *
 * The class that implement the evaluation of an analytical function.
 */
class OTGUI_API PythonEvaluation
  : public OT::NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  PythonEvaluation();

  PythonEvaluation(const OT::UnsignedInteger & inputDimension,
                   const OT::UnsignedInteger & ouputDimension,
                   const OT::String & code);

  /** Virtual constructor */
  virtual PythonEvaluation * clone() const;

  virtual ~PythonEvaluation();

  /** Comparison operator */
  OT::Bool operator ==(const PythonEvaluation & other) const;

  /** String converter */
  virtual OT::String __repr__() const;
  virtual OT::String __str__(const OT::String & offset = "") const;

protected:

  void initialize();

public:
  /** Operator () */
  virtual OT::NumericalPoint operator() (const OT::NumericalPoint & inP) const;
//   virtual OT::NumericalSample operator() (const OT::NumericalSample & inS) const;

   /** Accessor for input point dimension */
  OT::UnsignedInteger getInputDimension() const;
  OT::UnsignedInteger getOutputDimension() const;
  
  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:

private:
  OT::UnsignedInteger inputDimension_;
  OT::UnsignedInteger outputDimension_;
  OT::String code_;
 
};
}
#endif
