//                                               -*- C++ -*-
/**
 *  @brief Computes Monte Carlo analysis 
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/MonteCarloAnalysis.hxx"
#include "RandomGenerator.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(MonteCarloAnalysis);

MonteCarloAnalysis::MonteCarloAnalysis(const String & name, const PhysicalModel & physicalModel,
                                       const UnsignedInteger nbSimu, bool confidenceInterval, double level)
  : SimulationAnalysis(name, physicalModel, nbSimu)
  , isConfidenceIntervalRequired_(confidenceInterval)
  , levelConfidenceInterval_(level)
{
//TODO ctr with outputNames (pas OutputCollection!) optionnel par défaut prendrait tous les outputs
}


MonteCarloAnalysis* MonteCarloAnalysis::clone() const
{
  return new MonteCarloAnalysis(*this);
}


bool MonteCarloAnalysis::isConfidenceIntervalRequired() const
{
  return isConfidenceIntervalRequired_;
}


void MonteCarloAnalysis::setIsConfidenceIntervalRequired(const bool isConfidenceIntervalRequired)
{
  isConfidenceIntervalRequired_ = isConfidenceIntervalRequired;
}


double MonteCarloAnalysis::getLevelConfidenceInterval() const
{
  return levelConfidenceInterval_;
}


void MonteCarloAnalysis::setLevelConfidenceInterval(const double levelConfidenceInterval)
{
  levelConfidenceInterval_ = levelConfidenceInterval;
}


void MonteCarloAnalysis::run()
{
  RandomGenerator::SetSeed(getSeed());
  NumericalSample inputSample(getInputSample());

  // set results
  result_ = MonteCarloResult(inputSample, getOutputSample(inputSample));

  notify("analysisFinished");
}


MonteCarloResult MonteCarloAnalysis::getResult() const
{
  return result_;
}


String MonteCarloAnalysis::dump() const
{
  OSS oss;
  oss << getName() << " = otguibase.MonteCarloAnalysis('" << getName() << "', " << getPhysicalModel().getName();
  oss << ", " << getNbSimulations() << ")\n";
  oss << getName() << ".setSeed(" << getSeed() << ")\n";

  return oss;
}


bool MonteCarloAnalysis::analysisLaunched() const
{
//   return outputSample_.getSize()!=0;
}
}