// ModelEvaluation.hxx

#ifndef MODELEVALUATION_H
#define MODELEVALUATION_H

#include "AnalysisImplementation.hxx"
#include "ModelEvaluationResult.hxx"

namespace OTGUI {
class ModelEvaluation : public AnalysisImplementation
{
  CLASSNAME;

public:
  ModelEvaluation(const OT::String & name, const PhysicalModel & physicalModel);
  ModelEvaluation(const OT::String & name, const PhysicalModel & physicalModel,
                  const OT::NumericalPoint & inputsValues);

  virtual ModelEvaluation * clone() const;

  void updateParameters();

  OT::NumericalPoint getInputsValues() const;
  void setInputValue(const int & index, const double & value);

  ModelEvaluationResult getResult() const;

  virtual void run();
  virtual OT::String dump() const;
  virtual bool analysisLaunched() const;

protected:
  void initializeParameters(const InputCollection & inputs);

private:
  OT::Description inputNames_;
  ModelEvaluationResult result_;
  OT::NumericalPoint inputsValues_;
};
}
#endif