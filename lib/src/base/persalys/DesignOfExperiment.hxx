//                                               -*- C++ -*-
/**
 *  @brief Class to define designs of experiments
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
#ifndef PERSALYS_DESIGNOFEXPERIMENT_HXX
#define PERSALYS_DESIGNOFEXPERIMENT_HXX

#include "persalys/DataModel.hxx"

namespace PERSALYS
{
class DesignOfExperimentEvaluation;

class PERSALYS_BASE_API DesignOfExperiment : public OT::TypedInterfaceObject<DataModel>
{
  CLASSNAME

public:
  using Implementation = OT::Pointer<DataModel>;
  using Type = DataModel::Type;

  /** Constructor with parameters */
  explicit DesignOfExperiment(const OT::String & name = "");

  /** Constructor with parameters */
  DesignOfExperiment(const OT::String & name, const PhysicalModel & physicalModel);

  /** Constructor with parameters */
  DesignOfExperiment(
    const OT::String & name,
    const ImportedDataset & importedDataset,
    const OT::Description & inputNames = OT::Description(),
    const OT::Description & outputNames = OT::Description());

  /** Constructor with parameters */
  DesignOfExperiment(
    const OT::String & name,
    const OT::Sample & inSample,
    const OT::Sample & outSample);

  /** Default constructor */
  DesignOfExperiment(const DataModel & implementation);

  /** Constructor from implementation */
  DesignOfExperiment(const Implementation & p_implementation);
#ifndef SWIG
  /** Constructor from implementation pointer */
  DesignOfExperiment(DataModel * p_implementation);
#endif
  explicit DesignOfExperiment(const DesignOfExperimentEvaluation & eval);

  /** Comparison operator */
  OT::Bool operator ==(const DesignOfExperiment & other) const;
  OT::Bool operator !=(const DesignOfExperiment & other) const;

  void addObserver(Observer * observer);

  void setName(const OT::String & name) override;

  bool hasPhysicalModel() const;
  PhysicalModel getPhysicalModel() const;
  void setPhysicalModel(const PhysicalModel & physicalModel);
  void removePhysicalModel();

  OT::Sample getInputSample() const;
  OT::Sample getOutputSample() const;
  OT::Sample getSample() const;
  OT::Sample getMarginalWithoutNaN(const OT::UnsignedInteger index) const;
  void setInputSample(const OT::Sample & sample);
  void setOutputSample(const OT::Sample & sample);
  bool isValid() const;

  OT::String getPythonScript() const;

  OT::Indices getEffectiveInputIndices() const;

  void setType(Type type);
  Type getType() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

  /** override this method in order to emit a notification */
  void setImplementationAsPersistentObject(const ImplementationAsPersistentObject& obj) override;
};
}
#endif
