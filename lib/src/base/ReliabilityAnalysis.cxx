//                                               -*- C++ -*-
/**
 *  @brief ReliabilityAnalysis is the base class for Reliability Analysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/ReliabilityAnalysis.hxx"

using namespace OT;

namespace PERSALYS
{

/* Default constructor */
ReliabilityAnalysis::ReliabilityAnalysis()
  : PhysicalModelAnalysis()
{
  isReliabilityAnalysis_ = true;
  isDeterministicAnalysis_ = false;
}


/* Constructor with parameters */
ReliabilityAnalysis::ReliabilityAnalysis(const String & name, const LimitState & limitState)
  : PhysicalModelAnalysis(name, limitState.getPhysicalModel())
  , limitState_(limitState)
{
  isReliabilityAnalysis_ = true;
  isDeterministicAnalysis_ = false;
}


ReliabilityAnalysis * ReliabilityAnalysis::clone() const
{
  return new ReliabilityAnalysis(*this);
}

Observer * ReliabilityAnalysis::getParentObserver() const
{
  return limitState_.getImplementation()->getObserver("LimitStateItem");
}


LimitState ReliabilityAnalysis::getLimitState() const
{
  return limitState_;
}


void ReliabilityAnalysis::setInterestVariables(const Description& outputsNames)
{
  if (outputsNames.getSize() == 1)
    if (outputsNames[0] == limitState_.getOutputName())
      return;
  throw InvalidArgumentException(HERE) << "The output of interest " << limitState_.getOutputName() << "is already defined in the limit state";
}


void ReliabilityAnalysis::run()
{
  PhysicalModelAnalysis::run();
  OSS oss;
  oss << "<h3>Limit state</h3>";
  oss << "<p>";
  String limiteState = getLimitState().__str__();
  // write correctly the symbol of the operator
  size_t start_pos = limiteState.find("<=");
  if (start_pos != std::string::npos)
    limiteState.replace(start_pos, 2, "&le;");
  start_pos = limiteState.find(">=");
  if (start_pos != std::string::npos)
    limiteState.replace(start_pos, 2, "&ge;");
  start_pos = limiteState.find("<");
  if (start_pos != std::string::npos)
    limiteState.replace(start_pos, 1, "&lt;");
  oss << limiteState;
  oss << "</p>";
  modelHtmlDescription_ += oss;
}


bool ReliabilityAnalysis::canBeLaunched(String &errorMessage) const
{
  const bool canBeLaunched = PhysicalModelAnalysis::canBeLaunched(errorMessage);
  if (!canBeLaunched)
    return false;
  if (!getLimitState().isValid())
    errorMessage = "The limit state is not valid.";
  return errorMessage.empty();
}


/* String converter */
String ReliabilityAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " name=" << getName()
      << " limitState=" << getLimitState();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void ReliabilityAnalysis::save(Advocate & adv) const
{
  PhysicalModelAnalysis::save(adv);
  adv.saveAttribute("limitState_", limitState_);
}


/* Method load() reloads the object from the StorageManager */
void ReliabilityAnalysis::load(Advocate & adv)
{
  PhysicalModelAnalysis::load(adv);
  adv.loadAttribute("limitState_", limitState_);
}
}
