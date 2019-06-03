//                                               -*- C++ -*-
/**
 *  @brief MorrisAnalysis
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
#ifndef PERSALYS_MORRISANALYSIS_HXX
#define PERSALYS_MORRISANALYSIS_HXX

#include "SimulationAnalysis.hxx"
#include "MorrisResult.hxx"

namespace PERSALYS
{
class PERSALYS_API MorrisAnalysis : public SimulationAnalysis
{
  CLASSNAME

public:
  /** Default constructor */
  MorrisAnalysis();
  /** Constructor with parameters */
  MorrisAnalysis(const OT::String & name, const PhysicalModel & physicalModel);

  /** Virtual constructor */
  virtual MorrisAnalysis * clone() const;

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

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;
  virtual bool hasValidResult() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void initializeParameters();
  virtual void initialize();
  virtual void launch();

private:
  OT::UnsignedInteger trajectoriesNumber_;
  OT::Description inputNames_;
  OT::Interval bounds_;
  OT::UnsignedInteger level_;
  MorrisResult result_;
};
}
#endif
