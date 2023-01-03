//                                               -*- C++ -*-
/**
 *  @brief File-exchange coupling model
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
#ifndef PERSALYS_COUPLINGPHYSICALMODEL_HXX
#define PERSALYS_COUPLINGPHYSICALMODEL_HXX

#include "persalys/PythonPhysicalModel.hxx"
#include "persalys/CouplingStep.hxx"

namespace PERSALYS
{
typedef OT::Collection<CouplingStep> CouplingStepCollection;

class PERSALYS_BASE_API CouplingPhysicalModel : public PythonPhysicalModel
{
  CLASSNAME

public:

  /* Default constructor */
  explicit CouplingPhysicalModel(const OT::String & name = "Unnamed",
                                 const CouplingStepCollection & steps = CouplingStepCollection());

  CouplingPhysicalModel(const OT::String & name,
                        const InputCollection & inputs,
                        const OutputCollection & outputs,
                        const CouplingStepCollection & steps);

  /** Virtual constructor */
  CouplingPhysicalModel * clone() const override;

  /** Accessor to the steps */
  void setSteps(const CouplingStepCollection & steps);
  CouplingStepCollection getSteps() const;

  OT::String getHtmlDescription(const bool deterministic) const override;
  OT::String getPythonScript() const override;

  /** Update python code if advanced config has changed **/
  void updateCode();

  /** Whether the work dir is discarded */
  void setCleanupWorkDirectory(const OT::Bool cleanupWorkDirectory);
  OT::Bool getCleanupWorkDirectory() const;

  /** Working directory accessors **/
  void setWorkDir(const OT::FileName & workDir);
  OT::FileName getWorkDir() const;

  /** Cache files accessors */
  void setCacheFiles(const OT::FileName & inputFile, const OT::FileName & outputFile);
  OT::FileName getCacheInputFile() const;
  OT::FileName getCacheOutputFile() const;

  OT::String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv) override;

protected:
  OT::Function generateFunction(const OT::Description & outputNames) const override;

  OT::String getStepsMacro(const OT::String & offset = "") const;
private:
  // list of steps
  OT::PersistentCollection<CouplingStep> steps_;
  OT::Bool cleanupWorkDirectory_;
  OT::FileName cacheInputFile_;
  OT::FileName cacheOutputFile_;
  OT::FileName workDir_;
};
}
#endif
