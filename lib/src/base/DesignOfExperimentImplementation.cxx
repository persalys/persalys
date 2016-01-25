// DesignOfExperimentImplementation.cxx

#include "otgui/DesignOfExperimentImplementation.hxx"

#include "Box.hxx"
#include "TruncatedDistribution.hxx"
#include "TruncatedNormal.hxx"

using namespace OT;

namespace OTGUI {

DesignOfExperimentImplementation::DesignOfExperimentImplementation(const std::string & name, const PhysicalModel & physicalModel)
  : PersistentObject()
  , Observable()
  , name_(name)
  , physicalModel_(physicalModel)
  , type_(DesignOfExperimentImplementation::FromBoundsAndLevels)
  , fileName_("")
  , columns_(Indices(0))
//   , experiment_(Experiment())
{
  initializeParameters(physicalModel.getInputs());
}


DesignOfExperimentImplementation::DesignOfExperimentImplementation(const std::string & name,
                                                                   const PhysicalModel & physicalModel,
                                                                   const NumericalPoint & lowerBounds,
                                                                   const NumericalPoint & upperBounds,
                                                                   const Indices & levels,
                                                                   const OT::NumericalPoint & values)
  : PersistentObject()
  , Observable()
  , name_(name)
  , physicalModel_(physicalModel)
  , type_(DesignOfExperimentImplementation::FromBoundsAndLevels)
  , inputNames_(getPhysicalModel().getInputNames())
  , values_(values)
  , lowerBounds_(lowerBounds)
  , upperBounds_(upperBounds)
  , deltas_(NumericalPoint(physicalModel.getInputs().getSize()))
  , fileName_("")
  , columns_(Indices(0))
//   , experiment_(Experiment())
{
  setLevels(levels);
  if (!values_.getSize())
  {
    values_ = NumericalPoint(physicalModel.getInputs().getSize());
    for (UnsignedInteger i=0; i<physicalModel.getInputs().getSize(); ++i)
      values_[i] = physicalModel.getInputs()[i].getValue();
  }
//   TODO: check if lowerBounds[i] <= upperBounds[i]
}


// TODO: CTR with deltas
//   , type_(DesignOfExperimentImplementation::FromBoundsAndDeltas)

DesignOfExperimentImplementation::DesignOfExperimentImplementation(const std::string & name,
                                                                   const PhysicalModel & physicalModel,
                                                                   const std::string & fileName,
                                                                   const OT::Indices & columns)
  : PersistentObject()
  , Observable()
  , name_(name)
  , physicalModel_(physicalModel)
  , type_(DesignOfExperimentImplementation::FromFile)
  , fileName_(fileName)
{
  setColumns(columns);
  initializeParameters(physicalModel.getInputs());
}


// TODO
// DesignOfExperimentImplementation::DesignOfExperimentImplementation(const std::string & name,
//                                                                    const PhysicalModel & physicalModel,
//                                                                    const Experiment & experiment)
//   : PersistentObject()
//   , Observable()
//   , name_(name)
//   , physicalModel_(physicalModel)
//   , type_(DesignOfExperimentImplementation::FromExperiment)
//   , fileName_("")
//   , columns_(Indices(0))
//   , experiment_(experiment)
// {
//   initializeParameters(physicalModel.getInputs());
// }


DesignOfExperimentImplementation * DesignOfExperimentImplementation::clone() const
{
  return new DesignOfExperimentImplementation(*this);
}


void DesignOfExperimentImplementation::initializeParameters(const InputCollection & inputs)
{
  inputSample_.clear();
  inputNames_ = getPhysicalModel().getInputNames();

  int inputSize = inputs.getSize();
  values_ = NumericalPoint(inputSize);
  lowerBounds_ = NumericalPoint(inputSize);
  upperBounds_ = NumericalPoint(inputSize);
  levels_ = Indices(inputSize);
  deltas_ = NumericalPoint(inputSize);

  for (int i=0; i<inputSize; ++i)
  {
    values_[i] = inputs[i].getValue();
    if (!inputs[i].isStochastic())
    {
      lowerBounds_[i] = 0.9 * inputs[i].getValue();
      upperBounds_[i] = 1.1 * inputs[i].getValue();
    }
    else
    {
      Distribution distribution = inputs[i].getDistribution();
      std::string distributionName = distribution.getImplementation()->getClassName();
      
      if (distributionName == "TruncatedDistribution")
      {
        TruncatedDistribution truncatedDist = *dynamic_cast<TruncatedDistribution*>(&*distribution.getImplementation());
        if (truncatedDist.getFiniteLowerBound())
          lowerBounds_[i] = truncatedDist.getLowerBound();
        else
          lowerBounds_[i] = distribution.computeQuantile(0.05)[0];
        if (truncatedDist.getFiniteUpperBound())
          upperBounds_[i] = truncatedDist.getUpperBound();
        else
          upperBounds_[i] = distribution.computeQuantile(0.95)[0];
      }
      else if (distributionName == "TruncatedNormal")
      {
        TruncatedNormal truncatedDist = *dynamic_cast<TruncatedNormal*>(&*distribution.getImplementation());
        lowerBounds_[i] = truncatedDist.getA();
        upperBounds_[i] = truncatedDist.getB();
      }
      else
      {
        lowerBounds_[i] = distribution.computeQuantile(0.05)[0];
        upperBounds_[i] = distribution.computeQuantile(0.95)[0];
      }
    }
    levels_[i] = 2;
    deltas_[i] = upperBounds_[i] - lowerBounds_[i];
  }
}


void DesignOfExperimentImplementation::updateParameters()
{
  Description inputNames(inputNames_);
  NumericalPoint values(values_);
  NumericalPoint infBounds(lowerBounds_);
  NumericalPoint supBounds(upperBounds_);
  Indices levels(levels_);
  NumericalPoint deltas(deltas_);

  initializeParameters(getPhysicalModel().getInputs());

  for (UnsignedInteger i = 0; i < inputNames.getSize(); ++ i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
    {
      values_[i] = values[it - inputNames.begin()];
      lowerBounds_[i] = infBounds[it - inputNames.begin()];
      upperBounds_[i] = supBounds[it - inputNames.begin()];
      levels_[i] = levels[it - inputNames.begin()];
      deltas_[i] = deltas[it - inputNames.begin()];
    }
  }
}


std::string DesignOfExperimentImplementation::getName() const
{
  return name_;
}


void DesignOfExperimentImplementation::setName(const std::string & name)
{
  name_ = name;
}


PhysicalModel DesignOfExperimentImplementation::getPhysicalModel() const
{
  return physicalModel_;
}


void DesignOfExperimentImplementation::setPhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModel_ = physicalModel;
}


DesignOfExperimentImplementation::Type DesignOfExperimentImplementation::getTypeDesignOfExperiment() const
{
  return type_;
}


int DesignOfExperimentImplementation::getNumberOfExperiments() const
{
  int nbExperiments = 1;
  for (UnsignedInteger i=0; i<levels_.getSize(); ++i)
    nbExperiments *= levels_[i];
  return nbExperiments;
}


NumericalPoint DesignOfExperimentImplementation::getValues() const
{
  return values_;
}


void DesignOfExperimentImplementation::setValues(const NumericalPoint & values)
{
  values_ = values;
}


NumericalPoint DesignOfExperimentImplementation::getLowerBounds() const
{
  return lowerBounds_;
}


void DesignOfExperimentImplementation::setLowerBounds(const NumericalPoint & lowerBounds)
{
  lowerBounds_ = lowerBounds;
}


NumericalPoint DesignOfExperimentImplementation::getUpperBounds() const
{
  return upperBounds_;
}


void DesignOfExperimentImplementation::setUpperBounds(const NumericalPoint & upperBounds)
{
  upperBounds_ = upperBounds;
}


Indices DesignOfExperimentImplementation::getLevels() const
{
  return levels_;
}


void DesignOfExperimentImplementation::setLevels(const Indices & levels)
{
  if (levels.getSize() != physicalModel_.getInputs().getSize())
  {
    OSS oss;
    oss << "DesignOfExperimentImplementation::setLevels : The dimension of the list of the levels has to be equal to the number of inputs of the physical model: ";
    oss << physicalModel_.getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  type_ = DesignOfExperimentImplementation::FromBoundsAndLevels;
  levels_ = levels;
  for (UnsignedInteger i=0; i<levels_.getSize(); ++i)
  {
    if (levels_[i] == 1)
      deltas_[i] = 0;
    else
      deltas_[i] = (upperBounds_[i] - lowerBounds_[i])/(levels_[i]-1);
  }
}


NumericalPoint DesignOfExperimentImplementation::getDeltas() const
{
  return deltas_;
}


void DesignOfExperimentImplementation::setDeltas(const NumericalPoint & deltas)
{
  if (deltas.getSize() != physicalModel_.getInputs().getSize())
  {
    OSS oss;
    oss << "DesignOfExperimentImplementation::setDeltas : The dimension of the list of the deltas has to be equal to the number of inputs of the physical model: ";
    oss << physicalModel_.getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  for (UnsignedInteger i=0; i<deltas.getSize(); ++i)
    if (deltas[i] <= 0.)
      throw InvalidArgumentException(HERE) << "DesignOfExperimentImplementation::setDeltas : All the deltas must be superior or equal to 0.";


  type_ = DesignOfExperimentImplementation::FromBoundsAndDeltas;
  deltas_ = deltas;
  for (UnsignedInteger i=0; i<deltas_.getSize(); ++i)
  {
    if (deltas_[i] > 0)
      levels_[i] = (upperBounds_[i] - lowerBounds_[i])/deltas_[i] + 1;
    else
      levels_[i] = 1.;
  }
}


void DesignOfExperimentImplementation::setFileName(const std::string & fileName)
{
  fileName_ = fileName;
}


std::string DesignOfExperimentImplementation::getFileName() const
{
  return fileName_;
}


Indices DesignOfExperimentImplementation::getColumns() const
{
  return columns_;
}


void DesignOfExperimentImplementation::setColumns(Indices columns)
{
  if (columns.getSize() != physicalModel_.getInputs().getSize())
  {
    OSS oss;
    oss << "DesignOfExperimentImplementation::setColumns : The dimension of the list of the column numbers has to be equal to the number of inputs of the physical model: ";
    oss << physicalModel_.getInputs().getSize();
    throw InvalidArgumentException(HERE) << oss.str();
  }

  columns_ = columns;
}


// TODO
// Experiment DesignOfExperimentImplementation::getExperiment() const
// {
//   return experiment_;
// }
// 
// 
// void DesignOfExperimentImplementation::setExperiment(const Experiment & experiment)
// {
//   experiment_ = experiment;
// }


Description DesignOfExperimentImplementation::getVariableInputsNames() const
{
  if (!levels_.__contains__(1))
    return inputNames_;

  Description variableInputsNames;
  for (UnsignedInteger i=0; i<inputNames_.getSize(); ++i)
    if (levels_[i] > 1)
      variableInputsNames.add(inputNames_[i]);
  return variableInputsNames;
}


NumericalSample DesignOfExperimentImplementation::getInputSample()
{
  if (!inputSample_.getSize())
  {
    inputSample_.clear();
    if (fileName_.size() > 0)
    {
      std::vector< std::string > separatorsList(3);
      separatorsList[0] = " ";
      separatorsList[1] = ",";
      separatorsList[2] = ";";
      NumericalSample sampleFromFile;
      for (UnsignedInteger i = 0; i < separatorsList.size(); ++ i)
      {
        // import sample from the file
        sampleFromFile = NumericalSample::ImportFromTextFile(fileName_, separatorsList[i]);
        if (sampleFromFile.getSize())
          break;
      }
      if (!sampleFromFile.getSize())
        throw InvalidArgumentException(HERE) << "In DesignOfExperimentImplementation: impossible to load sample";
      if (!columns_.check(sampleFromFile.getDimension()))
        throw InvalidArgumentException(HERE) << "In DesignOfExperimentImplementation: impossible to load sample marginals";
      inputSample_ = sampleFromFile.getMarginal(columns_);
    }
    else
    {
      NumericalPoint scale;
      NumericalPoint transvec;
      NumericalPoint otLevels;
      Description variableInputsNames;

      for (UnsignedInteger i=0; i<inputNames_.getSize(); ++i)
      {
        if (levels_[i] > 1)
        {
          double inf = lowerBounds_[i];
          double sup = upperBounds_[i];
          scale.add(sup - inf);
          transvec.add(inf);
          variableInputsNames.add(inputNames_[i]);
          otLevels.add(levels_[i]-2);
        }
      }

      if (otLevels.getSize())
      {
        NumericalSample sample = Box(otLevels).generate();
        sample *= scale;
        sample += transvec;

        if (sample.getDimension() == inputNames_.getSize())
          inputSample_ = sample;
        else
        {
          inputSample_ = NumericalSample(sample.getSize(), inputNames_.getSize());
          for (UnsignedInteger i=0; i<inputNames_.getSize(); ++i)
          {
            if (levels_[i] == 1)
              for (UnsignedInteger j=0; j<sample.getSize(); ++j)
                inputSample_[j][i] = values_[i];
            else
            {
              const Description::const_iterator it = std::find(variableInputsNames.begin(), variableInputsNames.end(), inputNames_[i]);
              if (it != variableInputsNames.end())
                for (UnsignedInteger j=0; j<sample.getSize(); ++j)
                  inputSample_[j][i] = sample[j][it - variableInputsNames.begin()];
            }
          }
        }
      }
      else
        inputSample_ = NumericalSample(1, values_);
    }
    inputSample_.setDescription(inputNames_);
  }
  return inputSample_;
}


void DesignOfExperimentImplementation::setInputSample(const OT::NumericalSample & sample)
{
  if (sample.getDimension() != physicalModel_.getInputs().getSize())
    throw InvalidArgumentException(HERE) << "The sample has not a dimension equal to the number of inputs of the physical model.";
  inputSample_ = sample;
}


NumericalSample DesignOfExperimentImplementation::getOutputSample()
{
  return outputSample_;
}


void DesignOfExperimentImplementation::setOutputSample(const NumericalSample & sample)
{
  outputSample_ = sample;
}


ParametricAnalysisResult DesignOfExperimentImplementation::getResult() const
{
  return result_;
}


void DesignOfExperimentImplementation::eval()
{
  getInputSample();
  outputSample_.clear();
  outputSample_ = physicalModel_.getFunction()(inputSample_);
  result_ = ParametricAnalysisResult(inputSample_, outputSample_);
  notify("analysisFinished");
}


std::string DesignOfExperimentImplementation::dump() const
{
  OSS oss;

  if (fileName_.size() > 0)
  {
    oss << "columns = ot.Indices([";
   for (UnsignedInteger i = 0; i < columns_.getSize(); ++ i)
    {
      oss << columns_[i];
      if (i < columns_.getSize()-1)
        oss << ", ";
    }
    oss << "])\n";
  }
  else
  {
    oss << "values = [";
    for (UnsignedInteger i = 0; i < values_.getSize(); ++ i)
    {
      oss << values_[i];
      if (i < values_.getSize()-1)
        oss << ", ";
    }
    oss << "]\n";
    oss << "lowerBounds = [";
    for (UnsignedInteger i = 0; i < lowerBounds_.getSize(); ++ i)
    {
      oss << lowerBounds_[i];
      if (i < lowerBounds_.getSize()-1)
        oss << ", ";
    }
    oss << "]\n";
    oss << "upperBounds = [";
    for (UnsignedInteger i = 0; i < upperBounds_.getSize(); ++ i)
    {
      oss << upperBounds_[i];
      if (i < upperBounds_.getSize()-1)
        oss << ", ";
    }
    oss << "]\n";
    oss << "levels = [";
    for (UnsignedInteger i = 0; i < levels_.getSize(); ++ i)
    {
      oss << levels_[i];
      if (i < levels_.getSize()-1)
        oss << ", ";
    }
    oss << "]\n";
  }
  oss << getName()+ " = otguibase.DesignOfExperiment('" + getName() + "', "+getPhysicalModel().getName()+", ";
  if (fileName_.size() > 0)
  {
    oss << "'"+fileName_+"', columns)\n";
  }
  else
  {
    oss << "lowerBounds, upperBounds, levels, values)\n";
  }
  return oss.str();
}
}