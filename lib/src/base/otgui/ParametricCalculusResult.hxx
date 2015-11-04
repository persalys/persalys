// ParametricCalculusResult.hxx

#ifndef PARAMETRICCALCULUSRESULT_H
#define PARAMETRICCALCULUSRESULT_H

#include "NumericalSample.hxx"

namespace OTGUI {
typedef OT::Collection<OT::NumericalSample> NumericalSampleCollection;
class ParametricCalculusResult
{
  CLASSNAME;

public:
  ParametricCalculusResult();
  ParametricCalculusResult(OT::NumericalSample inputSample, OT::NumericalSample resultSample);

  virtual ~ParametricCalculusResult();

  OT::NumericalSample getResultSample() const;
  OT::NumericalSample getInputSample() const;
  NumericalSampleCollection getListXMin() const;
  OT::NumericalPoint getListMin() const;
  NumericalSampleCollection getListXMax() const;
  OT::NumericalPoint getListMax() const;
  OT::Description getOutputNames() const;
  OT::Description getInputNames() const;

protected:
  void searchMinMax();

private:
  OT::NumericalSample resultSample_;
  OT::NumericalSample inputSample_;
  NumericalSampleCollection listXMin_;
  OT::NumericalPoint listMin_;
  NumericalSampleCollection listXMax_;
  OT::NumericalPoint listMax_;
};
}
#endif