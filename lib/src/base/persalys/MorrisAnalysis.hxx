//                                               -*- C++ -*-
/**
 *  @brief MorrisAnalysis
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
#ifndef PERSALYS_MORRISANALYSIS_HXX
#define PERSALYS_MORRISANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "MorrisResult.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API MorrisAnalysis : public SimulationAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  MorrisAnalysis();
  /** Constructor with parameters */
  MorrisAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  MorrisAnalysis * clone() const override;

  OT::UnsignedInteger getTrajectoriesNumber() const;
  void setTrajectoriesNumber(const OT::UnsignedInteger number);

  OT::Interval getBounds() const;
  void setBounds(const OT::Interval & bounds);
  OT::UnsignedInteger getLevel() const;
  void setLevel(const OT::UnsignedInteger & value);

  void updateParameters();

  MorrisResult getResult() const;
#ifndef SWIG
  MorrisResult& getResult();
#endif

  Parameters getParameters() const override;
  OT::String getPythonScript() const override;
  bool hasValidResult() const override;
  bool canBeLaunched(OT::String &errorMessage) const override;

  /** String converter */
  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

  void setBlockSize(const OT::UnsignedInteger blockSize) override;
  OT::UnsignedInteger getBlockSize() const;

protected:
  void initializeParameters();
  void initialize() override;
  void launch() override;

private:
  OT::UnsignedInteger trajectoriesNumber_ = 10;
  OT::Description inputNames_;
  OT::Interval bounds_;
  OT::UnsignedInteger level_ = 4;
  MorrisResult result_;
  OT::UnsignedInteger blockSize_ = 1;
};
}
#endif
