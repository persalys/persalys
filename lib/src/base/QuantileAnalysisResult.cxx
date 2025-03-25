//                                               -*- C++ -*-
/**
 *  @brief Results of a quantile analysis
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
#include "persalys/QuantileAnalysisResult.hxx"
#include "persalys/QuantileAnalysis.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OT
{

TEMPLATE_CLASSNAMEINIT(OT::PersistentCollection<OT::PersistentCollection<Sample> >)
static const Factory<OT::PersistentCollection<OT::PersistentCollection<Sample>>> Factory_PersistentCollection_PersistentCollection_Sample;

}

namespace PERSALYS
{

CLASSNAMEINIT(QuantileAnalysisResult)

static Factory<QuantileAnalysisResult> Factory_QuantileAnalysisResult;

/* Default constructor */
QuantileAnalysisResult::QuantileAnalysisResult()
  : EvaluationResult()
{

}


/* Constructor with parameters */
QuantileAnalysisResult::QuantileAnalysisResult(const DesignOfExperiment& design)
  : EvaluationResult(design)
{
}


QuantileAnalysisResult* QuantileAnalysisResult::clone() const
{
  return new QuantileAnalysisResult(*this);
}

/* String converter */
String QuantileAnalysisResult::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
//      << " type=" << getType()
      // << " tail type=" << getTailTypes()
      << " quantiles=";
  for (auto const& qqq : quantiles_)
    oss << qqq.first << ": " << qqq.second;
  oss << " validity=";
  if (type_ == QuantileAnalysisResult::MonteCarlo)
  {
    for (auto const& val : wilksValidity_)
      oss << " " << val.first << ": " << val.second;
  }
  if (type_ == QuantileAnalysisResult::GeneralizedPareto)
  {
    for (auto const& val : pValue_)
      oss << " " << val.first << ": " << val.second;
  }
  return oss;
}


/* Method save() stores the object through the StorageManager */
void QuantileAnalysisResult::save(Advocate & adv) const
{
  EvaluationResult::save(adv);
  adv.saveAttribute("type_", type_);
  OT::Description keys;
  OT::PersistentCollection<OT::PersistentCollection<OT::Sample>> quantiles;
  for (auto const& qqq : quantiles_)
  {
    keys.add(qqq.first);
    quantiles.add(qqq.second);
  }
  OT::Description keysWilks1;
  OT::Indices keysWilks2;
  OT::PersistentCollection<OT::Indices> wilksValidity;
  for (auto const& val : wilksValidity_)
  {
    keysWilks1.add(val.first.first);
    keysWilks2.add(val.first.second);
    wilksValidity.add(val.second);
  }
  OT::Description keysPval1;
  OT::Indices keysPval2;
  OT::PersistentCollection<OT::Scalar> pValue;
  for (auto const& val : pValue_)
  {
    keysPval1.add(val.first.first);
    keysPval2.add(val.first.second);
    pValue.add(val.second);
  }

  adv.saveAttribute("keys_", keys);
  adv.saveAttribute("quantiles_", quantiles);
  adv.saveAttribute("keysWilks1_", keysWilks1);
  adv.saveAttribute("keysWilks2_", keysWilks2);
  adv.saveAttribute("wilksValidity_", wilksValidity);
  adv.saveAttribute("keysPval1_", keysPval1);
  adv.saveAttribute("keysPval2_", keysPval2);
  adv.saveAttribute("pValue_", pValue);
}


/* Method load() reloads the object from the StorageManager */
void QuantileAnalysisResult::load(Advocate & adv)
{
  EvaluationResult::load(adv);
  adv.loadAttribute("type_", type_);
  OT::Description keys;
  OT::PersistentCollection<OT::PersistentCollection<OT::Sample>> quantiles;
  adv.loadAttribute("keys_", keys);
  adv.loadAttribute("quantiles_", quantiles);
  for (UnsignedInteger i=0; i<keys.getSize(); ++i)
    quantiles_[keys[i]] = quantiles[i];

  OT::Description keysWilks1;
  OT::Indices keysWilks2;
  OT::PersistentCollection<OT::Indices> wilksValidity;
  adv.loadAttribute("keysWilks1_", keysWilks1);
  adv.loadAttribute("keysWilks2_", keysWilks2);
  adv.loadAttribute("wilksValidity_", wilksValidity);
  for (UnsignedInteger i=0; i<keysWilks1.getSize(); ++i)
    wilksValidity_[std::make_pair(keysWilks1[i], static_cast<TailType>(keysWilks2[i]))] = wilksValidity[i];

  OT::PersistentCollection<OT::Scalar> pValue;
  OT::Description keysPval1;
  OT::Indices keysPval2;
  adv.loadAttribute("keysPval1_", keysPval1);
  adv.loadAttribute("keysPval2_", keysPval2);
  adv.loadAttribute("pValue_", pValue);
  for (UnsignedInteger i=0; i<keysPval1.getSize(); ++i)
    pValue_[std::make_pair(keysPval1[i], static_cast<TailType>(keysPval2[i]))] = pValue[i];

}
}
