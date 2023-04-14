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

#include "persalys/AnsysParser.hxx"
#include "persalys/BaseTools.hxx"

#include "openturns/XMLToolbox.hxx"

#include <boost/filesystem.hpp>
#include <regex>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(AnsysParser)

AnsysParser::AnsysParser()
  : Object()
  , inputs_()
  , outputs_()
  , systems_()
{
}

AnsysParser::AnsysParser(const String & fileName)
  : Object()
  , inputs_()
  , outputs_()
  , systems_()
{
  loadData(fileName);
}

void AnsysParser::loadData(const String & fileName)
{
  setModelFileName(fileName);
  Pointer<XMLDoc> doc = new XMLDoc(fileName);

  XML::Node root = XML::GetRootNode(*doc);
  inputs_.clear();
  outputs_.clear();
  systems_.clear();

  // parse version
  XML::Node project = XML::FindElementByName(root, "Project");
  if(project != NULL)
  {
    XML::Node fwVersionString = XML::FindElementByName(project, "framework-build-version");
    if(fwVersionString != NULL)
    {
      std::regex versionRegex("([0-9]+)");
      std::smatch what;
      fullAnsysVersion_ = XML::GetNodeValue(fwVersionString);
      std::string::const_iterator start = fullAnsysVersion_.begin(), end = fullAnsysVersion_.end();
      if (std::regex_search(fullAnsysVersion_, what, versionRegex))
      {
        ansysVersion_ = what[1];
        start = what[0].second;
        std::regex_search(start, end, what, versionRegex);
        ansysVersion_ += what[1];
      }
    }
  }

  XML::Node containers = XML::FindElementByName(root, "Containers");
  XML::Node container = XML::FindElementByName(containers, "Container");
  while (container != NULL)
  {
    XML::Node containerType = XML::FindElementByName(container, "container-type");
    if (containerType == NULL)
      continue;
    if (XML::GetNodeValue(containerType) ==
        "Ansys.ParameterManager.ParameterManagerAddin:parameters")
    {
      XML::Node object = XML::FindElementByName(container, "Object");
      while (object != NULL)
      {
        XML::Node valType = XML::FindElementByName(object, "class-type");
        if (valType == NULL)
          continue;
        // Read Parameter class info
        if (XML::GetNodeValue(valType) == "Parameter")
        {
          XML::Node objectName = XML::FindElementByName(object, "object-name");
          if (objectName == NULL)
            continue;

          String variableName = XML::GetNodeValue(objectName);
          XML::Node memberData = XML::FindElementByName(object, "member-data");
          if (memberData == NULL)
            continue;

          String data = XML::GetNodeValue(memberData);
          std::regex usageRegex("\"Usage\": \"(\\w+)\"");
          std::smatch what;

          String variability;
          if (std::regex_search(data, what, usageRegex))
          {
            variability = what[1];
          }

          std::regex valueUnitRegex("\"value\": \"([\\S]+)(?:\\s*\\[([^\\]]*)\\])?\"");
          String value("0.0");
          String unit;
          String valueUnit;
          if (std::regex_search(data, what, valueUnitRegex))
          {
            value = what[1];
            unit = what[2];
          }

          std::regex expressionUnitRegex("\"Expression\": \"([^\"[]+)(?:.*)?\"");
          String expression;
          if (std::regex_search(data, what, expressionUnitRegex))
          {
            expression = what[1];
          }

          std::regex textRegex("\"DisplayText\": \"([^\"]*)\"");
          String text;
          if (std::regex_search(data, what, textRegex))
          {
            text = what[1];
          }

          // Check wether the expression depends on other parameters
          bool expressionIsValue = false;
          try
          {
            std::stod(expression);
            expressionIsValue = true;
          }
          catch (std::exception & exc)
          {
            // Do nothing
          }

          if(variability == "Input")
          {
            if(expressionIsValue)
              inputs_.add(Input(variableName, std::stod(value), text, unit));
          }
          else
            outputs_.add(Output(variableName, std::stod(value), text, unit));
        }
        object = XML::FindNextElementByName(object, "Object");
      }
    }
    // Read System class info
    else if (XML::GetNodeValue(containerType) ==
             "Ansys.ProjectSchematic.ProjectSchematicAddin:Default")
    {
      XML::Node object = XML::FindElementByName(container, "Object");
      while (object != NULL)
      {
        XML::Node valType = XML::FindElementByName(object, "class-type");
        if (valType == NULL)
          continue;
        if (XML::GetNodeValue(valType) == "System")
        {
          XML::Node objectName = XML::FindElementByName(object, "object-name");
          if (objectName == NULL)
            continue;

          String systemName = XML::GetNodeValue(objectName);
          XML::Node memberData = XML::FindElementByName(object, "member-data");
          if (memberData == NULL)
            continue;

          String data = XML::GetNodeValue(memberData);
          std::pair<String, String> text;

          std::regex displayTextRegex("\"DisplayText\": \"([^\"]*)\"");
          std::smatch whatDisplay;
          if (std::regex_search(data, whatDisplay, displayTextRegex))
            text.first = whatDisplay[1];

          std::regex HeaderRegex("\"HeaderText\": \"([^\"]*)\"");
          std::smatch whatHeader;
          if (std::regex_search(data, whatHeader, HeaderRegex))
            text.second = whatHeader[1];

          systems_[systemName] = text;
        }
        object = XML::FindNextElementByName(object, "Object");
      }
    }
    container = XML::FindNextElementByName(container, "Container");
  }
}

void AnsysParser::setInputVariables(const InputCollection & inputs)
{
  inputs_ = inputs;
}

InputCollection AnsysParser::getInputVariables() const
{
  return inputs_;
}

void AnsysParser::setOutputVariables(const OutputCollection & outputs)
{
  outputs_ = outputs;
}

OutputCollection AnsysParser::getOutputVariables() const
{
  return outputs_;
}

String AnsysParser::getAnsysVersion() const
{
  return ansysVersion_;
}

String AnsysParser::getFullAnsysVersion() const
{
  return fullAnsysVersion_;
}

void AnsysParser::setExecutableFileName(const String & fileName)
{
  executableFileName_ = fileName;
}

String AnsysParser::getExecutableFileName() const
{
  return executableFileName_;
}

void AnsysParser::setModelFileName(const String & fileName)
{
  modelFileName_ = fileName;
}

String AnsysParser::getModelFileName() const
{
  return modelFileName_;
}

void AnsysParser::setTemplateFileName(const String & fileName)
{
  templateFileName_ = fileName;
}

String AnsysParser::getTemplateFileName() const
{
  return templateFileName_;
}

void AnsysParser::setOutputFileName(const String & fileName)
{
  outputFileName_ = fileName;
}

String AnsysParser::getOutputFileName() const
{
  return outputFileName_;
}

String AnsysParser::getProjectDirectory() const
{
  return modelFileName_.substr(0, modelFileName_.find_last_of('.')) + "_files";
}


void AnsysParser::setSystems(const std::map<String, std::pair<String, String> >& sys)
{
  systems_ = sys;
}

std::map<String, std::pair<String, String> > AnsysParser::getSystems() const
{
  return systems_;
}

void AnsysParser::generateTemplate(const int indStep)
{
  String name = "input" + std::to_string(indStep) + ".wbjn.in";
  templateFileName_ = (boost::filesystem::path(modelFileName_).parent_path()
                       / boost::filesystem::path(name)).string();
  outputFileName_ = "output" + std::to_string(indStep) + ".csv";
  generateTemplate(templateFileName_, outputFileName_);
}

void AnsysParser::generateTemplate(const String & templateFileName,
                                   const String & outputFileName)
{
  setTemplateFileName(templateFileName);
  setOutputFileName(outputFileName);

  std::ofstream templateFile;
  templateFile.open(templateFileName.c_str());

  templateFile << "# encoding: utf-8\n";
  templateFile << "# SetScriptVersion(Version=\"" << getFullAnsysVersion() << "\")\n";
  templateFile << "designPoint0 = Parameters.GetDesignPoint(Name=\"0\")\n";
  templateFile << "designPoint1 = Parameters.CreateDesignPoint()\n";
  for (UnsignedInteger i = 0; i < inputs_.getSize(); ++i)
  {
    templateFile << "parameter" << i << " = Parameters.GetParameter(Name=\""
                 << inputs_[i].getName() << "\")\n";
    templateFile << "designPoint1.SetParameterExpression(Parameter=parameter"
                 << i << ", Expression=\"@" << inputs_[i].getName() << "@";
    if(inputs_[i].getUnit() != "")
      templateFile << " [" << inputs_[i].getUnit() << "]";
    templateFile << "\")\n";
  }
  templateFile << "designPoint1.Retained = True\n";
  templateFile << "Parameters.SetBaseDesignPoint(DesignPoint=designPoint1)\n";
  templateFile << "Save(Overwrite=True)\n";
  int i = 1;
  for (auto const& sys : systems_)
  {
    templateFile << "system" << i << " = GetSystem(Name=\"" << sys.first <<"\")\n";
    templateFile << "system" << i << ".Update(AllDependencies=True)\n";
    ++i;
  }

  templateFile << "Parameters.ExportAllDesignPointsData(FilePath=\""
               << outputFileName << "\")\n";
  templateFile << "Parameters.SetBaseDesignPoint(DesignPoint=designPoint0)\n";
  templateFile << "designPoint1.Retained = False\n";
  templateFile << "Save(Overwrite=True)\n";

}

void AnsysParser::populateCouplingStep(CouplingPhysicalModel *model,
                                       const int indStep,
                                       const String& templateFileName)
{
  if(templateFileName == "")
    generateTemplate(indStep);
  else
    generateTemplate(templateFileName, "output0.csv");

  CouplingStepCollection csColl(model->getSteps());
  CouplingStep cs(csColl[indStep]);

  CouplingInputFileCollection inColl;
  CouplingOutputFileCollection outColl;

  CouplingInputFile inFile(getTemplateFileName());
  const InputCollection inputs = getInputVariables();
  Description inNames;
  Description inTokens;
  Description inFormats;
  for (UnsignedInteger i = 0; i < inputs.getSize(); ++i)
  {
    inNames.add(inputs[i].getName());
    inTokens.add("@"+inputs[i].getName()+"@");
    inFormats.add("{}");
    if(!model->hasInputNamed(inputs[i].getName()))
      model->PhysicalModelImplementation::addInput(inputs[i]);
  }

  inFile.setVariables(inNames, inTokens, inFormats);
  inColl.add(inFile);

  for (UnsignedInteger i = 0; i < getOutputVariables().getSize(); ++i)
    if(!model->hasOutputNamed(getOutputVariables()[i].getName()))
      model->PhysicalModelImplementation::addOutput(getOutputVariables()[i]);

  cs.setInputFiles(inColl);

  OSS cmd;
  cmd << getExecutableFileName()
      << " -B -F " << modelFileName_
      << " -R input" << std::to_string(indStep) << ".wbjn";
  cs.setCommand(cmd);
  cs.setEnvironment(Description({"PYTHONPATH", "PYTHONHOME"}), Description(2, ""));

  OSS code;
  if(getOutputVariables().getSize())
  {
    code << "def read_output():\n";
    code << "    import os\n";
    code << "    with open(os.path.join(workdir, '" << getOutputFileName() <<"'), 'r') as f:\n";
    code << "        for line in f.read().splitlines():\n";
    code << "            if line.startswith('N'):# 'Name|Nom' en|fr|de, !jp\n";
    code << "                names = line.split(',')[1:]\n";
    code << "            if line.startswith('DP'):\n";
    code << "                values = line.split(',')[1:]\n";
    code << "    results = dict(zip(names, values))\n";

    for(UnsignedInteger i = 0; i < getOutputVariables().getSize(); ++i)
      code << "    " << getOutputVariables()[i].getName() << " = float(results['"
           << getOutputVariables()[i].getName() << "'])\n";
    code << "    return ";
    for(UnsignedInteger i = 0; i < getOutputVariables().getSize()-1; ++i)
      code << getOutputVariables()[i].getName() << ",";
    code << getOutputVariables()[getOutputVariables().getSize()-1].getName();
  }
  cs.setCode(EscapeQuotes(code));

  csColl[indStep] = cs;
  model->setSteps(csColl);


}

}
