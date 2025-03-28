//                                               -*- C++ -*-
/**
 *  @brief Defines field evaluation from external data and mesh
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
#include "persalys/DataFieldModelImplementation.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{
  CLASSNAMEINIT(DataFieldModelImplementation)

  static Factory<DataFieldModelImplementation> Factory_DataFieldModelImplementation;
  DataFieldModelImplementation::DataFieldModelImplementation(const String& name)
    : PersistentObject()
    , Observable()
  {
    setName(name);
  }

  DataFieldModelImplementation::DataFieldModelImplementation(const String& name,
                                                             const MeshModel & meshModel)
    : PersistentObject()
    , Observable()
    , meshModel_(meshModel)
  {
    setName(name);
  }

  DataFieldModelImplementation::DataFieldModelImplementation(const OT::String & name,
                                                             const MeshModel & meshModel,
                                                             const ProcessSample & processSample)
    : PersistentObject()
    , Observable()
    , meshModel_(meshModel)
    , processSample_(processSample)
  {
    setName(name);
  }


  /* Virtual constructor */
  DataFieldModelImplementation* DataFieldModelImplementation::clone() const
  {
    return new DataFieldModelImplementation(*this);
  }

  void DataFieldModelImplementation::setName(const String & name)
  {
    PersistentObject::setName(name);
    notify("nameChanged");
  }

  void DataFieldModelImplementation::removeAllObservers()
  {
    notifyAndRemove("DataFieldModelDefinitionItem");
    notifyAndRemove("DataMeshItem");
    notifyAndRemove("DataFieldModelDiagramItem");
    notifyAndRemove("Study");
  }

  Bool DataFieldModelImplementation::operator==(const DataFieldModelImplementation& other) const
  {
    if (this == &other)
      return true;
    return false;
  }


  Bool DataFieldModelImplementation::operator!=(const DataFieldModelImplementation& other) const
  {
    return !operator==(other);
  }

  void DataFieldModelImplementation::setMeshModel(const MeshModel& meshModel)
  {
    meshModel_ = meshModel;
    notify("meshChanged");
  }

  MeshModel DataFieldModelImplementation::getMeshModel()const
  {
    return meshModel_;
  }

  void DataFieldModelImplementation::importProcessSample(const String & fileName,
                                                         const Tools::DataOrder order)
  {
    const Sample sample = Tools::ImportSample(fileName, order);
    if (sample.getDimension() != meshModel_.getMesh().getVerticesNumber())
      throw InvalidArgumentException(HERE) << "Mesh vertices number must match field size. Expected "
                                           << sample.getDimension() << " got "
                                           << meshModel_.getMesh().getVerticesNumber();
    setSampleAsProcessSample(sample);
  }

  Sample DataFieldModelImplementation::getProcessSampleAsSample() const
  {
    if (!getProcessSample().getSize())
      return Sample();
    if (getProcessSample()[0].getDimension() != 1)
      throw InvalidArgumentException(HERE) << "Can only convert ProcessSample containing one output.";
    Sample sample(0, getProcessSample()[0].getSize());
    for(UnsignedInteger i=0; i<getProcessSample().getSize(); ++i)
    {
      sample.add(getProcessSample()[i].asPoint());
    }
    return sample;
  }

  void DataFieldModelImplementation::setSampleAsProcessSample(const OT::Sample& sample)
  {
    Collection<Sample> collection;
    for(UnsignedInteger i=0; i<sample.getSize(); ++i)
      collection.add(Sample::BuildFromPoint(sample[i]));
    processSample_ = ProcessSample(getMeshModel().getMesh(), collection);
    notify("processSampleChanged");
  }

  void DataFieldModelImplementation::setProcessSample(const OT::ProcessSample& processSample)
  {
    processSample_ = processSample;
    notify("processSampleChanged");
  }

  String DataFieldModelImplementation::getPythonScript() const
  {
    OSS oss;
    // mesh
    oss << getMeshModel().getPythonScript();
    oss << "collection = " << Parameters::GetOTSampleCollectionStr(getProcessSample());
    oss << "mesh = " << getMeshModel().getName() << ".getMesh()\n";
    oss << "pSample = ot.ProcessSample(mesh, collection)\n";
    oss << getName() << " = persalys.DataFieldModel('" << getName() << "', "
        << getMeshModel().getName() << ", pSample)\n";

    return oss;
  }


  String DataFieldModelImplementation::__repr__() const
  {
    OSS oss;
    oss << PersistentObject::__repr__()
        << " meshModel_=" << getMeshModel()
        << " processSample_=" << getProcessSample();

    return oss;
  }

  void DataFieldModelImplementation::save(OT::Advocate & adv) const
  {
    PersistentObject::save(adv);
    adv.saveAttribute("meshModel_", meshModel_);
    adv.saveAttribute("processSample_", processSample_);
  }

  void DataFieldModelImplementation::load(OT::Advocate & adv)
  {
    PersistentObject::load(adv);
    adv.loadAttribute("meshModel_", meshModel_);
    adv.loadAttribute("processSample_", processSample_);
  }
}
