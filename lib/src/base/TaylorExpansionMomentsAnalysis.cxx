//                                               -*- C++ -*-
/**
 *  @brief TaylorExpansionMomentsAnalysis computes the moments with the Taylor Expansion method
 *
 *  Copyright 2015-2021 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "persalys/TaylorExpansionMomentsAnalysis.hxx"

#include <openturns/TaylorExpansionMoments.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(TaylorExpansionMomentsAnalysis)

static Factory<TaylorExpansionMomentsAnalysis> Factory_TaylorExpansionMomentsAnalysis;

/* Default constructor */
TaylorExpansionMomentsAnalysis::TaylorExpansionMomentsAnalysis()
  : PhysicalModelAnalysis()
{
  isDeterministicAnalysis_ = false;
}


/* Constructor with parameters */
TaylorExpansionMomentsAnalysis::TaylorExpansionMomentsAnalysis(const String & name, const PhysicalModel & physicalModel)
  : PhysicalModelAnalysis(name, physicalModel)
{
  isDeterministicAnalysis_ = false;
}


/* Virtual constructor */
TaylorExpansionMomentsAnalysis* TaylorExpansionMomentsAnalysis::clone() const
{
  return new TaylorExpansionMomentsAnalysis(*this);
}


void TaylorExpansionMomentsAnalysis::initialize()
{
  AnalysisImplementation::initialize();
  result_ = TaylorExpansionMomentsResult();
}


void TaylorExpansionMomentsAnalysis::launch()
{
  if (!getPhysicalModel().getDistribution().isContinuous())
    throw InvalidArgumentException(HERE) << "The model distribution must have continuous marginals.";

  // set analysis
  TaylorExpansionMoments algoTaylorExpansionMoments(getPhysicalModel().getOutputRandomVector(getInterestVariables()));

  // set results
  Point meanFirstOrder = algoTaylorExpansionMoments.getMeanFirstOrder();
  Point meanSecondOrder = algoTaylorExpansionMoments.getMeanSecondOrder();
  Point variance = Point(algoTaylorExpansionMoments.getCovariance().getDimension());
  for (UnsignedInteger i = 0; i < variance.getDimension(); ++i)
    variance[i] = algoTaylorExpansionMoments.getCovariance()(i, i);
  Point standardDeviation(variance.getDimension());
  for (UnsignedInteger i = 0; i < variance.getDimension(); ++i)
    standardDeviation[i] = sqrt(variance[i]);

  result_ = TaylorExpansionMomentsResult(getInterestVariables(), meanFirstOrder, meanSecondOrder, standardDeviation, variance);
}


TaylorExpansionMomentsResult TaylorExpansionMomentsAnalysis::getResult() const
{
  return result_;
}


Parameters TaylorExpansionMomentsAnalysis::getParameters() const
{
  Parameters param;

  param.add("Algorithm", "Taylor expansion moments");
  param.add("Outputs of interest", getInterestVariables().__str__());

  return param;
}


String TaylorExpansionMomentsAnalysis::getPythonScript() const
{
  OSS oss;
  oss << getName() << " = persalys.TaylorExpansionMomentsAnalysis('" << getName() << "', " << getPhysicalModel().getName() << ")\n";
  if (getInterestVariables().getSize() < getPhysicalModel().getSelectedOutputsNames().getSize())
  {
    oss << "interestVariables = " << Parameters::GetOTDescriptionStr(getInterestVariables()) << "\n";
    oss << getName() << ".setInterestVariables(interestVariables)\n";
  }

  return oss;
}


bool TaylorExpansionMomentsAnalysis::hasValidResult() const
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
