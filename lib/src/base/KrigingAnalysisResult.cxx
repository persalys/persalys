//                                               -*- C++ -*-
/**
 *  @brief Kriging analysis result
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/KrigingAnalysisResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/KrigingResult.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(KrigingAnalysisResult)

static Factory<KrigingAnalysisResult> Factory_KrigingAnalysisResult;
static Factory<PersistentCollection<GaussianProcessRegressionResult> > Factory_PersistentCollection_GaussianProcessRegressionResult;
static Factory<PersistentCollection<KrigingResult> > Factory_PersistentCollection_KrigingResult;// for retro compatibility

/* Default constructor */
KrigingAnalysisResult::KrigingAnalysisResult()
  : MetaModelAnalysisResult()
{
}


/* Virtual constructor */
KrigingAnalysisResult* KrigingAnalysisResult::clone() const
{
  return new KrigingAnalysisResult(*this);
}


KrigingAnalysisResult::GaussianProcessRegressionResultCollection KrigingAnalysisResult::getGPRResultCollection() const
{
  return gprResultCollection_;
}


GaussianProcessRegressionResult KrigingAnalysisResult::getResultForVariable(const String& variableName) const
{
  if (!gprResultCollection_.getSize())
    throw InvalidArgumentException(HERE) << "There is no kriging result";

  for (UnsignedInteger i = 0; i < gprResultCollection_.getSize(); ++ i)
    if (gprResultCollection_[i].getOutputSample().getDescription()[0] == variableName)
      return gprResultCollection_[i];

  throw InvalidArgumentException(HERE) << "No result for a variable named " << variableName;
}


/* String converter */
String KrigingAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " gprResultCollection=" << getGPRResultCollection();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void KrigingAnalysisResult::save(Advocate& adv) const
{
  MetaModelAnalysisResult::save(adv);
  adv.saveAttribute("gprResultCollection_", gprResultCollection_);
}


/* Method load() reloads the object from the StorageManager */
void KrigingAnalysisResult::load(Advocate& adv)
{
  MetaModelAnalysisResult::load(adv);
  if (adv.hasAttribute("gprResultCollection_"))
    adv.loadAttribute("gprResultCollection_", gprResultCollection_);
  else
  {
    // persalys <19.1
    OT::PersistentCollection< OT::KrigingResult > krigingResultCollection;
    adv.loadAttribute("krigingResultCollection_", krigingResultCollection);
    const UnsignedInteger size = krigingResultCollection.getSize();
    gprResultCollection_.resize(size);
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      const KrigingResult krigingResult(krigingResultCollection[i]);
      const GaussianProcessFitterResult fitterResult(krigingResult.getInputSample(),
                                                     krigingResult.getOutputSample(),
                                                     krigingResult.getMetaModel(),
                                                     Matrix(),// regression matrix
                                                     krigingResult.getBasis(),
                                                     krigingResult.getTrendCoefficients(),
                                                     krigingResult.getCovarianceModel(),
                                                     0.0, // optimal likelihood
                                                     GaussianProcessFitterResult::LAPACK);
      const GaussianProcessRegressionResult gprResult(fitterResult, krigingResult.getCovarianceCoefficients());
      gprResultCollection_[i] = gprResult;
    }
  }
}

}
