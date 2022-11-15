//                                               -*- C++ -*-
/**
 *  @brief Kriging analysis result
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/PolynomialRegressionAnalysisResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(PolynomialRegressionAnalysisResult)

static Factory<PolynomialRegressionAnalysisResult> Factory_PolynomialRegressionAnalysisResult;
static Factory<PersistentCollection<LinearModelResult> > Factory_PersistentCollection_LinearModelResult;


/* Default constructor */
PolynomialRegressionAnalysisResult::PolynomialRegressionAnalysisResult()
  : MetaModelAnalysisResult()
{
}


/* Virtual constructor */
PolynomialRegressionAnalysisResult* PolynomialRegressionAnalysisResult::clone() const
{
  return new PolynomialRegressionAnalysisResult(*this);
}


PolynomialRegressionAnalysisResult::LinearModelResultCollection PolynomialRegressionAnalysisResult::getLinearModelResultCollection() const
{
  return linearModelResultCollection_;
}


LinearModelResult PolynomialRegressionAnalysisResult::getResultForVariable(const String& variableName) const
{
  if (!linearModelResultCollection_.getSize())
    throw InvalidArgumentException(HERE) << "There is no result";

  for (UnsignedInteger i = 0; i < linearModelResultCollection_.getSize(); ++ i)
    if (linearModelResultCollection_[i].getOutputSample().getDescription()[0] == variableName)
      return linearModelResultCollection_[i];

  throw InvalidArgumentException(HERE) << "No result for a variable named " << variableName;
}

Description PolynomialRegressionAnalysisResult::getFormulas() const
{
  return formulas_;
}

/* String converter */
String PolynomialRegressionAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " linearModelResultCollection=" << getLinearModelResultCollection();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void PolynomialRegressionAnalysisResult::save(Advocate& adv) const
{
  MetaModelAnalysisResult::save(adv);
  adv.saveAttribute("linearModelResultCollection_", linearModelResultCollection_);
  adv.saveAttribute("formulas_", formulas_);
}


/* Method load() reloads the object from the StorageManager */
void PolynomialRegressionAnalysisResult::load(Advocate& adv)
{
  MetaModelAnalysisResult::load(adv);
  adv.loadAttribute("linearModelResultCollection_", linearModelResultCollection_);
  adv.loadAttribute("formulas_", formulas_);
}
}
