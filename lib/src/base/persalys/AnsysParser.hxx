//                                               -*- C++ -*-
/**
 *  @brief Parser for Ansys files
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

#ifndef PERSALYS_ANSYSPARSER_HXX
#define PERSALYS_ANSYSPARSER_HXX

#include "persalys/Input.hxx"
#include "persalys/Output.hxx"
#include "persalys/CouplingPhysicalModel.hxx"

namespace PERSALYS
{
class PERSALYS_BASE_API AnsysParser : public OT::Object
{
  CLASSNAME
public:
  /** Default constructor */
  AnsysParser();

  AnsysParser(const OT::String & fileName);

  /** Load data from ansys file */
  void loadData(const OT::String & fileName);

  /** Inputs accessor */
  void setInputVariables(const InputCollection & inputs);
  InputCollection getInputVariables() const;

  /** Outputs accessor*/
  void setOutputVariables(const OutputCollection & outputs);
  OutputCollection getOutputVariables() const;

  /** Ansys version accessor*/
  OT::String getAnsysVersion() const;
  OT::String getFullAnsysVersion() const;

  /** Ansys executable accessor*/
  void setExecutableFileName(const OT::String & fileName);
  OT::String getExecutableFileName() const;

  /** Ansys model file accessor*/
  void setModelFileName(const OT::String & fileName);
  OT::String getModelFileName() const;

  void setTemplateFileName(const OT::String & fileName);
  OT::String getTemplateFileName() const;

  void setOutputFileName(const OT::String & fileName);
  OT::String getOutputFileName() const;

  OT::String getProjectDirectory() const;

  void setSystems(const std::map<OT::String, std::pair<OT::String, OT::String> >& sys);
  std::map<OT::String, std::pair<OT::String, OT::String> > getSystems() const;

  void generateTemplate(const int indStep);
  void generateTemplate(const OT::String & templateFileName,
                        const OT::String & outputFileName);

  void populateCouplingStep(CouplingPhysicalModel *model,
                            const int indStep,
                            const OT::String& templateFileName = "");

protected:
  InputCollection inputs_;
  OutputCollection outputs_;
  std::map<OT::String, std::pair<OT::String, OT::String> > systems_;
  OT::String ansysVersion_;
  OT::String fullAnsysVersion_;
  OT::String executableFileName_;
  OT::String modelFileName_;
  OT::String templateFileName_;
  OT::String outputFileName_;
};
}
#endif
