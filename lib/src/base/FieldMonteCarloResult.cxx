//                                               -*- C++ -*-
/**
 *  @brief Monte Carlo analysis result
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "otgui/FieldMonteCarloResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(FieldMonteCarloResult)

static Factory<FieldMonteCarloResult> Factory_FieldMonteCarloResult;
static Factory<PersistentCollection<KarhunenLoeveResult> > Factory_PersistentCollectionKarhunenLoeveResult;


/* Default constructor */
FieldMonteCarloResult::FieldMonteCarloResult()
  : EvaluationResult()
  , processSample_()
  , meanSample_()
  , lowerQuantileSample_()
  , upperQuantileSample_()
  , xiSamples_()
  , correlationFunction_()
  , karhunenLoeveResults_()
{
}


FieldMonteCarloResult* FieldMonteCarloResult::clone() const
{
  return new FieldMonteCarloResult(*this);
}


ProcessSample FieldMonteCarloResult::getProcessSample() const
{
  return processSample_;
}


Sample FieldMonteCarloResult::getMeanSample() const
{
  return meanSample_;
}


Sample FieldMonteCarloResult::getLowerQuantileSample() const
{
  return lowerQuantileSample_;
}


Sample FieldMonteCarloResult::getUpperQuantileSample() const
{
  return upperQuantileSample_;
}


Collection<Sample> FieldMonteCarloResult::getXiSamples() const
{
  return xiSamples_;
}


Collection<Function> FieldMonteCarloResult::getCorrelationFunction() const
{
  return correlationFunction_;
}


Collection<KarhunenLoeveResult> FieldMonteCarloResult::getKarhunenLoeveResults() const
{
  return karhunenLoeveResults_;
}


/* String converter */
String FieldMonteCarloResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " processSample_=" << getProcessSample()
      << " meanSample_=" << getMeanSample()
      << " lowerQuantileSample_=" << getLowerQuantileSample()
      << " upperQuantileSample_=" << getUpperQuantileSample()
      << " xiSamples_=" << getXiSamples()
      << " correlationFunction_=" << getCorrelationFunction();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void FieldMonteCarloResult::save(Advocate & adv) const
{
  EvaluationResult::save(adv);
  adv.saveAttribute("processSample_", processSample_);
  adv.saveAttribute("meanSample_", meanSample_);
  adv.saveAttribute("lowerQuantileSample_", lowerQuantileSample_);
  adv.saveAttribute("upperQuantileSample_", upperQuantileSample_);
  adv.saveAttribute("xiSamples_", xiSamples_);
  adv.saveAttribute("correlationFunction_", correlationFunction_);
  adv.saveAttribute("karhunenLoeveResults_", karhunenLoeveResults_);
}


/* Method load() reloads the object from the StorageManager */
void FieldMonteCarloResult::load(Advocate & adv)
{
  EvaluationResult::load(adv);
  adv.loadAttribute("processSample_", processSample_);
  adv.loadAttribute("meanSample_", meanSample_);
  adv.loadAttribute("lowerQuantileSample_", lowerQuantileSample_);
  adv.loadAttribute("upperQuantileSample_", upperQuantileSample_);
  adv.loadAttribute("xiSamples_", xiSamples_);
  adv.loadAttribute("correlationFunction_", correlationFunction_);
  adv.loadAttribute("karhunenLoeveResults_", karhunenLoeveResults_);
}
}
