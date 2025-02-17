//                                               -*- C++ -*-
/**
 *  @brief Defines field evaluation from external data and mesh
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/DataFieldModel.hxx"

using namespace OT;

namespace PERSALYS
{

  CLASSNAMEINIT(DataFieldModel)

  /* Default constructor */
  DataFieldModel::DataFieldModel()
    : TypedInterfaceObject<DataFieldModelImplementation>(new DataFieldModelImplementation())
  {
  }

  DataFieldModel::DataFieldModel(const String& name,
                                 const MeshModel & meshModel)
    : TypedInterfaceObject<DataFieldModelImplementation>(new DataFieldModelImplementation(name, meshModel))
  {
  }

  /* Constructor with parameters */
  DataFieldModel::DataFieldModel(const String & name,
                                 const MeshModel & meshModel,
                                 const OT::ProcessSample & processSample)
    : TypedInterfaceObject<DataFieldModelImplementation>(new DataFieldModelImplementation(name, meshModel, processSample))
  {
  }


  /* Default constructor */
  DataFieldModel::DataFieldModel(const DataFieldModelImplementation& implementation)
    : TypedInterfaceObject<DataFieldModelImplementation>(implementation.clone())
  {
  }


/* Constructor from implementation */
  DataFieldModel::DataFieldModel(const Implementation& p_implementation)
    : TypedInterfaceObject<DataFieldModelImplementation>(p_implementation)
  {
    // Initialize any other class members here
    // At last, allocate memory space if needed, but go to destructor to free it
  }

/* Constructor from implementation pointer */
  DataFieldModel::DataFieldModel(DataFieldModelImplementation* p_implementation)
    : TypedInterfaceObject<DataFieldModelImplementation>(p_implementation)
  {
    // Initialize any other class members here
    // At last, allocate memory space if needed, but go to destructor to free it
  }

  Bool DataFieldModel::operator==(const DataFieldModel& other) const
  {
    if (this == &other) return true;
    return *getImplementation() == *other.getImplementation();
  }


  Bool DataFieldModel::operator!=(const DataFieldModel& other) const
  {
    return !operator==(other);
  }

  void DataFieldModel::addObserver(Observer* observer)
  {
    getImplementation()->addObserver(observer);
  }

  void DataFieldModel::importProcessSample(const String & fileName,
                                           const Tools::DataOrder order)
  {
    getImplementation()->importProcessSample(fileName, order);
  }

  Sample DataFieldModel::getProcessSampleAsSample() const
  {
    return getImplementation()->getProcessSampleAsSample();
  }

  ProcessSample DataFieldModel::getProcessSample() const
  {
    return getImplementation()->getProcessSample();
  }

  void DataFieldModel::setSampleAsProcessSample(const OT::Sample & sample)
  {
    getImplementation()->setSampleAsProcessSample(sample);
  }

  void DataFieldModel::setProcessSample(const OT::ProcessSample& processSample)
  {
    getImplementation()->setProcessSample(processSample);
  }

  String DataFieldModel::getPythonScript() const
  {
    return getImplementation()->getPythonScript();
  }

  void DataFieldModel::setMeshModel(const MeshModel& meshModel)
  {
    getImplementation()->setMeshModel(meshModel);
  }

  MeshModel DataFieldModel::getMeshModel() const
  {
    return getImplementation()->getMeshModel();
  }

  bool DataFieldModel::isValid() const
  {
    const Sample sample = getProcessSampleAsSample();
    // Check mesh and data are not empty
    if (!getMeshModel().getMesh().getVerticesNumber() ||
        !sample.getSize())
      return false;

    // Check data does not contain invalid values
    for (UnsignedInteger i=0; i<sample.getSize(); ++i)
      for (UnsignedInteger j = 0; j < sample.getDimension(); ++j)
        if (!SpecFunc::IsNormal(sample(i, j)))
          return false;

    // Ensure mesh and data compatibility
    return getMeshModel().getMesh().getVerticesNumber() == sample.getDimension();
  }

  String DataFieldModel::__repr__() const
  {
    return getImplementation()->__repr__();
  }

  void DataFieldModel::save(OT::Advocate & adv) const
  {
    getImplementation()->save(adv);
  }

  void DataFieldModel::load(OT::Advocate & adv)
  {
    getImplementation()->load(adv);
  }

  void DataFieldModel::setImplementationAsPersistentObject(const ImplementationAsPersistentObject& obj)
  {
    TypedInterfaceObject< DataFieldModelImplementation >::setImplementationAsPersistentObject(obj);
    getImplementation()->notify("implementationModified");
  }
}
