//                                               -*- C++ -*-
/**
 *  @brief Class for Reliability Analysis using the FORM-Importance sampling method
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_FORMIMPORTANCESAMPLINGANALYSIS_HXX
#define PERSALYS_FORMIMPORTANCESAMPLINGANALYSIS_HXX

#include "ImportanceSamplingAnalysis.hxx"
#include "ApproximationAnalysis.hxx"

#include <openturns/FORMResult.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API FORMImportanceSamplingAnalysis : public ImportanceSamplingAnalysis, public ApproximationAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  FORMImportanceSamplingAnalysis();
  /** Constructor with parameters */
  FORMImportanceSamplingAnalysis(const OT::String& name, const LimitState& limitState);

  /** Virtual constructor */
  virtual FORMImportanceSamplingAnalysis * clone() const;

  OT::FORMResult getFORMResult() const;

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual void initialize();
  virtual void launch();

private:
  OT::FORMResult FORMResult_;
};
}
#endif
