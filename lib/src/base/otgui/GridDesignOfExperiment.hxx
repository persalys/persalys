//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_GRIDDESIGNOFEXPERIMENT_HXX
#define OTGUI_GRIDDESIGNOFEXPERIMENT_HXX

#include "DesignOfExperimentEvaluation.hxx"

namespace OTGUI
{
class OTGUI_API GridDesignOfExperiment : public DesignOfExperimentEvaluation
{
  CLASSNAME

public:
  enum Type {FromBoundsAndLevels, FromBoundsAndDeltas};

  /** Default constructor */
  GridDesignOfExperiment();
  /** Constructor with parameters */
  GridDesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel);
  /** Constructor with parameters */
  GridDesignOfExperiment(const OT::String & name,
                         const PhysicalModel & physicalModel,
                         const OT::Point & lowerBounds,
                         const OT::Point & upperBounds,
                         const OT::Indices & nbValues,
                         const OT::Point & values = OT::Point(0));

  /** Virtual constructor */
  virtual GridDesignOfExperiment * clone() const;

  Type getTypeDesignOfExperiment() const;

  OT::Point getValues() const;
  void setValues(const OT::Point & values);
  OT::Point getLowerBounds() const;
  void setLowerBounds(const OT::Point & lowerBounds);
  OT::Point getUpperBounds() const;
  void setUpperBounds(const OT::Point & upperBounds);
  OT::Indices getLevels() const;
  void setLevels(const OT::Indices & nbValues);
  OT::Point getDeltas() const;
  void setDeltas(const OT::Point & deltas);
  OT::Description getVariableInputNames() const;

  void updateParameters();

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

private:
  Type type_;
  OT::Description inputNames_;
  OT::Point values_;
  OT::Point lowerBounds_;
  OT::Point upperBounds_;
  OT::Indices levels_;
  OT::Point deltas_;
};
}
#endif
