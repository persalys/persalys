//                                               -*- C++ -*-
/**
 *  @brief Kriging analysis result
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
#include "otgui/KrigingAnalysisResult.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTGUI
{

CLASSNAMEINIT(KrigingAnalysisResult)

static Factory<KrigingAnalysisResult> Factory_KrigingAnalysisResult;
static Factory<PersistentCollection<KrigingResult> > Factory_PersistentCollection_KrigingResult;

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


KrigingAnalysisResult::KrigingResultCollection KrigingAnalysisResult::getKrigingResultCollection() const
{
  return krigingResultCollection_;
}


KrigingResult KrigingAnalysisResult::getResultForVariable(const String& variableName) const
{
  if (!krigingResultCollection_.getSize())
    throw InvalidArgumentException(HERE) << "There is no kriging result";

  for (UnsignedInteger i = 0; i < krigingResultCollection_.getSize(); ++i)
    if (krigingResultCollection_[i].getModel().getOutputDescription()[0] == variableName)
      return krigingResultCollection_[i];

  throw InvalidArgumentException(HERE) << "No result for a variable named " << variableName;
}


/* String converter */
String KrigingAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " krigingResultCollection=" << getKrigingResultCollection();
  return oss;
}


/* Method save() stores the object through the StorageManager */
void KrigingAnalysisResult::save(Advocate& adv) const
{
  MetaModelAnalysisResult::save(adv);
  adv.saveAttribute("krigingResultCollection_", krigingResultCollection_);
}


/* Method load() reloads the object from the StorageManager */
void KrigingAnalysisResult::load(Advocate& adv)
{
  MetaModelAnalysisResult::load(adv);
  adv.loadAttribute("krigingResultCollection_", krigingResultCollection_);
}
}
