//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_FIXEDDESIGNOFEXPERIMENT_HXX
#define OTGUI_FIXEDDESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentImplementation.hxx"

namespace OTGUI {
class OTGUI_API FixedDesignOfExperiment : public DesignOfExperimentImplementation
{
  CLASSNAME;

public:
  enum Type {FromBoundsAndLevels, FromBoundsAndDeltas};

  /** Default constructor */
  FixedDesignOfExperiment();
  /** Constructor with parameters */
  FixedDesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel);
  /** Constructor with parameters */
  FixedDesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel,
                     const OT::NumericalPoint & lowerBounds, const OT::NumericalPoint & upperBounds,
                     const OT::Indices & nbValues, const OT::NumericalPoint & values=OT::NumericalPoint(0));

  /** Virtual constructor */
  virtual FixedDesignOfExperiment * clone() const;

  Type getTypeDesignOfExperiment() const;

  OT::NumericalPoint getValues() const;
  void setValues(const OT::NumericalPoint & values);
  OT::NumericalPoint getLowerBounds() const;
  void setLowerBounds(const OT::NumericalPoint & lowerBounds);
  OT::NumericalPoint getUpperBounds() const;
  void setUpperBounds(const OT::NumericalPoint & upperBounds);
  OT::Indices getLevels() const;
  void setLevels(const OT::Indices & nbValues);
  OT::NumericalPoint getDeltas() const;
  void setDeltas(const OT::NumericalPoint & deltas);
  OT::Description getVariableInputNames() const;

  void updateParameters();

  virtual OT::String getPythonScript() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  void initializeParameters();
  void generateInputSample();

private:
  Type type_;
  OT::Description inputNames_;
  OT::NumericalPoint values_;
  OT::NumericalPoint lowerBounds_;
  OT::NumericalPoint upperBounds_;
  OT::Indices levels_;
  OT::NumericalPoint deltas_;
};
}
#endif