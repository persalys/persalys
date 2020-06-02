//                                               -*- C++ -*-
/**
 *  @brief Intermediate coupling step
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
#ifndef PERSALYS_COUPLINGSTEP_HXX
#define PERSALYS_COUPLINGSTEP_HXX

#include "persalys/CouplingInputFile.hxx"
#include "persalys/CouplingResourceFile.hxx"
#include "persalys/CouplingOutputFile.hxx"

namespace PERSALYS
{
  typedef OT::Collection<CouplingInputFile> CouplingInputFileCollection;
  typedef OT::Collection<CouplingResourceFile> CouplingResourceFileCollection;
  typedef OT::Collection<CouplingOutputFile> CouplingOutputFileCollection;

class PERSALYS_API CouplingStep : public OT::PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  explicit CouplingStep(const OT::String & command = "");

  CouplingStep(const OT::String & command,
               const CouplingInputFileCollection & inputFiles,
               const CouplingResourceFileCollection & resourceFiles,
               const CouplingOutputFileCollection & outputFiles);

  /** Virtual constructor */
  virtual CouplingStep * clone() const;

  /** Command accessor */
  void setCommand(const OT::String & command);
  OT::String getCommand() const;

  /* Whether the command is to be interpreted by the shell */
  void setIsShell(const OT::Bool isShell);
  OT::Bool getIsShell() const;

  /** Files accessor */
  void setInputFiles(const CouplingInputFileCollection & inputFiles);
  CouplingInputFileCollection getInputFiles() const;

  void setResourceFiles(const CouplingResourceFileCollection & inputFiles);
  CouplingResourceFileCollection getResourceFiles() const;

  void setOutputFiles(const CouplingOutputFileCollection & outputFiles);
  CouplingOutputFileCollection getOutputFiles() const;

  /** TimeOut accessors **/
  void setTimeOut(const double & timeOut);
  double getTimeOut() const;

  /** Encoding accessors **/
  void setEncoding(const OT::String & encoding);
  OT::String getEncoding() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  OT::String command_;
  OT::Bool isShell_;
  OT::PersistentCollection<CouplingInputFile> inputFiles_;
  OT::PersistentCollection<CouplingResourceFile> resourceFiles_;
  OT::PersistentCollection<CouplingOutputFile> outputFiles_;
  double timeOut_=-1;
  OT::String encoding_="utf-8";
};

}
#endif
