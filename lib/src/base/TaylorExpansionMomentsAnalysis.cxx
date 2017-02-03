//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionMomentsAnalysis computes the moments with the Taylor Expansion method
 *
 *  Copyright 2015-2016 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/TaylorExpansionMomentsAnalysis.hxx"

#include "openturns/TaylorExpansionMoments.hxx"
#include "openturns/PersistentObjectFactory.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(TaylorExpansionMomentsAnalysis);

static Factory<TaylorExpansionMomentsAnalysis> RegisteredFactory;

/* Default constructor */
TaylorExpansionMomentsAnalysis::TaylorExpansionMomentsAnalysis()
  : PhysicalModelAnalysis()
{

}


/* Constructor with parameters */
TaylorExpansionMomentsAnalysis::TaylorExpansionMomentsAnalysis(const String & name, const PhysicalModel & physicalModel)
  : PhysicalModelAnalysis(name, physicalModel)
{
}


/* Virtual constructor */
TaylorExpansionMomentsAnalysis* TaylorExpansionMomentsAnalysis::clone() const
{
  return new TaylorExpansionMomentsAnalysis(*this);
}


void TaylorExpansionMomentsAnalysis::run()
{
  try
  {
    // clear result
    result_ = TaylorExpansionMomentsResult();

    // set analysis
    TaylorExpansionMoments algoTaylorExpansionMoments(getPhysicalModel().getOutputRandomVector(getInterestVariables()));

    // set results
    NumericalPoint meanFirstOrder = algoTaylorExpansionMoments.getMeanFirstOrder();
    NumericalPoint meanSecondOrder = algoTaylorExpansionMoments.getMeanSecondOrder();
    NumericalPoint variance = NumericalPoint(algoTaylorExpansionMoments.getCovariance().getDimension());
    for (UnsignedInteger i=0; i<variance.getDimension(); ++i)
      variance[i] = algoTaylorExpansionMoments.getCovariance()(i,i);
    NumericalPoint standardDeviation(variance.getDimension());
    for (UnsignedInteger i=0; i<variance.getDimension(); ++i)
      standardDeviation[i] = sqrt(variance[i]);

    result_ = TaylorExpansionMomentsResult(getInterestVariables(), meanFirstOrder, meanSecondOrder, standardDeviation, variance);

    notify("analysisFinished");
  }
  catch (std::exception & ex)
  {
    setErrorMessage(ex.what());
    notify("analysisBadlyFinished");
  }
}


TaylorExpansionMomentsResult TaylorExpansionMomentsAnalysis::getResult() const
{
  return result_;
}


String TaylorExpansionMomentsAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = otguibase.TaylorExpansionMomentsAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = [";
    for (UnsignedInteger i=0; i<getInterestVariables().getSize(); ++i)
    {
      oss << "'" << getInterestVariables()[i] << "'";
      if (i < getInterestVariables().getSize()-1)
        oss << ", ";
    }
    oss << "]\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }

  return oss;
}


bool TaylorExpansionMomentsAnalysis::analysisLaunched() const
{
  return getResult().getOutputNames().getSize() != 0;
}


/* String converter */
String TaylorExpansionMomentsAnalysis::__repr__() const
{
  return PhysicalModelAnalysis::__repr__();
}


/* Method save() stores the object through the StorageManager */
void TaylorExpansionMomentsAnalysis::save(Advocate & adv) const
{
  PhysicalModelAnalysis::save(adv);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void TaylorExpansionMomentsAnalysis::load(Advocate & adv)
{
  PhysicalModelAnalysis::load(adv);
  adv.loadAttribute("result_", result_);
}
}