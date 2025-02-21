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

#ifndef PERSALYS_DATAFIELDMODEL_HXX
#define PERSALYS_DATAFIELDMODEL_HXX

#include "persalys/DataFieldModelImplementation.hxx"

namespace PERSALYS
{
  class PERSALYS_BASE_API DataFieldModel : public OT::TypedInterfaceObject<DataFieldModelImplementation>
  {
    CLASSNAME

  public:
    typedef OT::Pointer<DataFieldModelImplementation>       Implementation;

    /** Default constructor */
    DataFieldModel();
    DataFieldModel(const OT::String & name,
                   const MeshModel & meshModel);
    DataFieldModel(const OT::String & name,
                   const MeshModel & meshModel,
                   const OT::ProcessSample & processSample);

    /** Default constructor */
    DataFieldModel(const DataFieldModelImplementation& implementation);
    /** Constructor from implementation */
    DataFieldModel(const Implementation & p_implementation);
    /** Constructor from implementation pointer */
    DataFieldModel(DataFieldModelImplementation * p_implementation);

    /** Comparison operator */
    OT::Bool operator ==(const DataFieldModel & other) const;
    OT::Bool operator !=(const DataFieldModel & other) const;

    void addObserver(Observer * observer);
    void blockNotification(const OT::String& blockedObserverType = "") {getImplementation()->blockNotification(blockedObserverType);};

    void importProcessSample(const OT::String & fileName,
                             const Tools::DataOrder order=Tools::Columns);

    void setMeshModel(const MeshModel& meshModel);
    MeshModel getMeshModel() const;

    /** String converter **/
    OT::String __repr__() const;

    OT::ProcessSample getProcessSample() const;
    OT::Sample getProcessSampleAsSample() const;

    void setSampleAsProcessSample(const OT::Sample & sample);
    void setProcessSample(const OT::ProcessSample& processSample);

    OT::String getPythonScript() const;

    bool isValid() const;

    /** Method save() stores the object through the StorageManager */
    void save(OT::Advocate & adv) const;

    /** Method load() reloads the object from the StorageManager */
    void load(OT::Advocate & adv);

    /** override this method in order to emit a notification */
    void setImplementationAsPersistentObject(const ImplementationAsPersistentObject& obj) override;
  };
}
#endif
