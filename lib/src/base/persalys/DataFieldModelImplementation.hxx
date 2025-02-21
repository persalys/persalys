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

#ifndef PERSALYS_DATAFIELDMODELIMPLEMENTATION_HXX
#define PERSALYS_DATAFIELDMODELIMPLEMENTATION_HXX

#include "persalys/PersalysPrivate.hxx"
#include "persalys/Observable.hxx"
#include "persalys/BaseTools.hxx"
#include "persalys/MeshModel.hxx"

#include <openturns/ProcessSample.hxx>

namespace PERSALYS
{

  class PERSALYS_BASE_API DataFieldModelImplementation : public OT::PersistentObject, public Observable
  {
    CLASSNAME

  public:
    /** Constructor with parameters */
    DataFieldModelImplementation(const OT::String & name = "Unnamed");
    DataFieldModelImplementation(const OT::String & name,
                                 const MeshModel & meshModel);
    DataFieldModelImplementation(const OT::String & name,
                                 const MeshModel & meshModel,
                                 const OT::ProcessSample & processSample);

    /** Virtual constructor */
    DataFieldModelImplementation* clone() const;

    virtual void setName(const OT::String & name);

    void removeAllObservers() override;

    /** Comparison operators */
    OT::Bool operator ==(const DataFieldModelImplementation & other) const;
    OT::Bool operator !=(const DataFieldModelImplementation & other) const;

    void importProcessSample(const OT::String & fileName,
                             const Tools::DataOrder order=Tools::Columns);

    void setMeshModel(const MeshModel& meshModel);
    MeshModel getMeshModel() const;

    /** String converter **/
    OT::String __repr__() const;

    /** Method save() stores the object through the StorageManager */
    void save(OT::Advocate & adv) const override;

    /** Method load() reloads the object from the StorageManager */
    void load(OT::Advocate & adv) override;

    OT::ProcessSample getProcessSample() const {return processSample_;};
    void setProcessSample(const OT::ProcessSample& processSample);

    OT::Sample getProcessSampleAsSample() const;
    void setSampleAsProcessSample(const OT::Sample & sample);

    OT::String getPythonScript() const;

  private:
    MeshModel meshModel_;
    OT::ProcessSample processSample_;

  };
}
#endif
