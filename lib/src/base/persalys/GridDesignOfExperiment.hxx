//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#ifndef PERSALYS_GRIDDESIGNOFEXPERIMENT_HXX
#define PERSALYS_GRIDDESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentEvaluation.hxx"

namespace PERSALYS
{
class PERSALYS_API GridDesignOfExperiment : public DesignOfExperimentEvaluation
{
  CLASSNAME

public:
  enum Type {FromBoundsAndLevels, FromBoundsAndDeltas};

  /** Default constructor */
  GridDesignOfExperiment();
  /** Constructor with parameters */
  GridDesignOfExperiment(const OT::String &name, const PhysicalModel &physicalModel);
  /** Constructor with parameters */
  GridDesignOfExperiment(const OT::String &name,
                         const PhysicalModel &physicalModel,
                         const OT::Collection<OT::Point> &values);

  /** Virtual constructor */
  virtual GridDesignOfExperiment * clone() const;

  OT::Collection<OT::Point> getValues() const;
  void setValues(const OT::Collection<OT::Point> &values);
  static OT::Interval GetDefaultBounds(const PhysicalModel &model);

  void updateParameters();

  virtual Parameters getParameters() const;
  virtual OT::String getPythonScript() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void initializeParameters();
  virtual OT::Sample generateInputSample(const OT::UnsignedInteger nbSimu) const;

protected:
  OT::Description inputNames_;
  OT::PersistentCollection<OT::Point> values_;
};
}
#endif
