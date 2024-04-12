//                                               -*- C++ -*-
/**
 *  @brief SimulationAnalysis evaluates the models
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_SIMULATIONANALYSIS_HXX
#define PERSALYS_SIMULATIONANALYSIS_HXX

#include "PhysicalModelAnalysis.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API SimulationAnalysis : public PhysicalModelAnalysis
{
protected:
  /** Default constructor */
  SimulationAnalysis();
  /** Constructor with parameters */
  SimulationAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

public:
  /** Virtual constructor */
  SimulationAnalysis * clone() const override;

  OT::Sample getFailedInputSample() const;
  OT::Description getErrorDescription() const
  {
    return errorDescription_;
  };

  OT::UnsignedInteger getBlockSize() const;
  virtual void setBlockSize(const OT::UnsignedInteger size);

  OT::UnsignedInteger getSeed() const;
  virtual void setSeed(const OT::UnsignedInteger seed);

  Parameters getParameters() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  void initialize() override;
  virtual OT::Sample generateInputSample(const OT::UnsignedInteger nbSimu) const;

protected:
  OT::Sample failedInputSample_;
  OT::Description errorDescription_;
private:
  OT::UnsignedInteger blockSize_;
  OT::UnsignedInteger seed_;
};
}
#endif
