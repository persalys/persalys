//                                               -*- C++ -*-
/**
 *  @brief YACSPhysicalModel implements models defined by an xml file
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
#include "persalys/YACSPhysicalModel.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/PersistentObjectFactory.hxx>
#include <openturns/MemoizeFunction.hxx>

#include <regex>

using namespace OT;


namespace PERSALYS
{

CLASSNAMEINIT(YACSPhysicalModel)

static Factory<YACSPhysicalModel> Factory_YACSPhysicalModel;

/* Default constructor */
YACSPhysicalModel::YACSPhysicalModel(const String & name)
  : PythonPhysicalModel(name)
{
  // Nothing to do
}


/* Constructor with parameters */
YACSPhysicalModel::YACSPhysicalModel(const String & name,
                                     const InputCollection & inputs,
                                     const OutputCollection & outputs,
                                     const String & code)
  : PythonPhysicalModel(name, inputs, outputs, "")
{
  setCode(code);
}


/* Virtual constructor */
YACSPhysicalModel* YACSPhysicalModel::clone() const
{
  return new YACSPhysicalModel(*this);
}


void YACSPhysicalModel::setInputs(const InputCollection & inputs)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


void YACSPhysicalModel::addInput(const Input & input)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


void YACSPhysicalModel::removeInput(const String & inputName)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


void YACSPhysicalModel::setOutputs(const OutputCollection & outputs)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


void YACSPhysicalModel::addOutput(const Output & output)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


void YACSPhysicalModel::removeOutput(const String & outputName)
{
  throw NotYetImplementedException(HERE) << "Use setCode to modify a YACS model";
}


ydefx::JobParametersProxy& YACSPhysicalModel::jobParameters()
{
  return evaluation_.jobParameters();
}


const ydefx::JobParametersProxy& YACSPhysicalModel::jobParameters()const
{
  return evaluation_.jobParameters();
}


void YACSPhysicalModel::setCode(const String & code)
{
  PythonPhysicalModel::setCode(code);
  if (useYACS_)
  {
    try
    {
      evaluation_.setCode(code);
    }
    catch (const std::exception & ex)
    {
      throw InvalidArgumentException(HERE) << "Error in the script.\n" << ex.what();
    }
  }
}


Function YACSPhysicalModel::generateFunction(const Description & outputNames) const
{
  if (!useYACS_)
    return PythonPhysicalModel::generateFunction(outputNames);
  if (!functionCache_.getEvaluation().getImplementation()->isActualImplementation())
  {
    YACSEvaluation anEvaluation(evaluation_);
    anEvaluation.setOutputVariablesNames(outputNames);
    functionCache_ = MemoizeFunction(anEvaluation);
  }
  return functionCache_;
}


String YACSPhysicalModel::getPythonScript() const
{
  OSS oss;

  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i < getOutputDimension(); ++ i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = " << Parameters::GetOTDescriptionStr(getInputNames(), false) << "\n";
  oss << "outputs = " << Parameters::GetOTDescriptionStr(getOutputNames(), false) << "\n";

  // replace ''' by """
  std::string code = getCode();
  std::regex_replace(code, std::regex("'''"), "\"\"\"");

  oss << "code = '''" << code << "'''\n";
  oss << getName()
      << " = persalys.YACSPhysicalModel('" << getName() << "'"
      << ", inputs, outputs, code)\n";
  oss << getName() << ".setUseYACS(" << (useYACS_ ? "True" : "False") << ")\n";
  if (useYACS_)
    oss << evaluation_.getJobParamsPythonScript(getName());

  oss << getProbaModelPythonScript();
  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
}


/* Use YACS backend or Python backend */
void YACSPhysicalModel::setUseYACS(const Bool useYACS)
{
  if (useYACS != useYACS_)
  {
    useYACS_ = useYACS;
    functionCache_ = Function();

    if (useYACS)
    {
      try
      {
        evaluation_.setCode(getCode());
      }
      catch (const std::exception & ex)
      {
        // just to keep in sync
      }
    }
  }
}


Bool YACSPhysicalModel::getUseYACS() const
{
  return useYACS_;
}


/** String converter */
String YACSPhysicalModel::__repr__() const
{
  OSS oss;
  oss << PythonPhysicalModel::__repr__()
      << " evaluation=" << evaluation_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void YACSPhysicalModel::save(Advocate & adv) const
{
  PythonPhysicalModel::save(adv);
  adv.saveAttribute("evaluation_", evaluation_);
  adv.saveAttribute("useYACS_", useYACS_);
}


/* Method load() reloads the object from the StorageManager */
void YACSPhysicalModel::load(Advocate & adv)
{
  PythonPhysicalModel::load(adv);
  adv.loadAttribute("evaluation_", evaluation_);
  if (adv.hasAttribute("useYACS_"))
    adv.loadAttribute("useYACS_", useYACS_);
  else
    useYACS_ = true;
}


bool YACSPhysicalModel::canBeDetached() const
{
  return useYACS_;
}


}
