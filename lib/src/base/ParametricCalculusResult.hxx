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
  ParametricCalculusResult(OT::NumericalSample resultSample, OT::NumericalSample inputSample);

  virtual ~ParametricCalculusResult();

  OT::NumericalSample getResultSample() const;
  NumericalSampleCollection getListXMin() const;
  OT::NumericalPoint getListMin() const;
  NumericalSampleCollection getListXMax() const;
  OT::NumericalPoint getListMax() const;
  OT::Description getOutputNames() const;

protected:
  void searchMinMax(OT::NumericalSample inputSample);

private:
  OT::NumericalSample resultSample_;
  NumericalSampleCollection listXMin_;
  OT::NumericalPoint listMin_;
  NumericalSampleCollection listXMax_;
  OT::NumericalPoint listMax_;
};
}
#endif