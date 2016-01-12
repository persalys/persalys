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
  , experiment_(Experiment())
{
  initializeParameters(physicalModel.getInputs());
}


DesignOfExperimentImplementation::DesignOfExperimentImplementation(const std::string & name,
                                                                   const PhysicalModel & physicalModel,
                                                                   const NumericalPoint & lowerBounds,
                                                                   const NumericalPoint & upperBounds,
                                                                   const Indices & levels)
  : PersistentObject()
  , Observable()
  , name_(name)
  , physicalModel_(physicalModel)
  , type_(DesignOfExperimentImplementation::FromBoundsAndLevels)
  , inputNames_(getPhysicalModel().getInputNames())
  , lowerBounds_(lowerBounds)
  , upperBounds_(upperBounds)
  , levels_(levels)
  , deltas_(NumericalPoint(levels.getSize()))
  , fileName_("")
  , columns_(Indices(0))
  , experiment_(Experiment())
{
//   TODO: check if lowerBounds[i] <= upperBounds[i]
}


// TODO: CTR with deltas
//   , type_(DesignOfExperimentImplementation::FromBoundsAndDeltas)

DesignOfExperimentImplementation::DesignOfExperimentImplementation(const std::string & name,
                                                                   const PhysicalModel & physicalModel,
                                                                   const std::string & fileName,
                                                                   OT::Indices columns)
  : PersistentObject()
  , Observable()
  , name_(name)
  , physicalModel_(physicalModel)
  , type_(DesignOfExperimentImplementation::FromFile)
  , inputNames_(getPhysicalModel().getInputNames())
  , lowerBounds_(NumericalPoint(0))
  , upperBounds_(NumericalPoint(0))
  , levels_(Indices(0))
  , deltas_(NumericalPoint(0))
  , experiment_(Experiment())
{
  setFileName(fileName, columns);
}


DesignOfExperimentImplementation::DesignOfExperimentImplementation(const std::string & name,
                                                                   const PhysicalModel & physicalModel,
                                                                   const Experiment & experiment)
  : PersistentObject()
  , Observable()
  , name_(name)
  , physicalModel_(physicalModel)
  , type_(DesignOfExperimentImplementation::FromExperiment)
  , inputNames_(getPhysicalModel().getInputNames())
  , lowerBounds_(NumericalPoint(0))
  , upperBounds_(NumericalPoint(0))
  , levels_(Indices(0))
  , deltas_(NumericalPoint(0))
  , fileName_("")
  , columns_(Indices(0))
  , experiment_(experiment)
{
}


DesignOfExperimentImplementation::DesignOfExperimentImplementation(const DesignOfExperimentImplementation & other)
  : PersistentObject()
  , Observable()
  , name_(other.name_)
  , physicalModel_(other.physicalModel_)
  , type_(other.type_)
  , inputNames_(other.inputNames_)
  , lowerBounds_(other.lowerBounds_)
  , upperBounds_(other.upperBounds_)
  , levels_(other.levels_)
  , deltas_(other.deltas_)
  , fileName_(other.fileName_)
  , columns_(other.columns_)
  , experiment_(other.experiment_)
{
}


DesignOfExperimentImplementation * DesignOfExperimentImplementation::clone() const
{
  return new DesignOfExperimentImplementation(*this);
}


void DesignOfExperimentImplementation::initializeParameters(const InputCollection & inputs)
{
  inputSample_.clear();
  inputNames_ = getPhysicalModel().getInputNames();

  int inputSize = inputs.getSize();
  lowerBounds_ = NumericalPoint(inputSize);
  upperBounds_ = NumericalPoint(inputSize);
  levels_ = Indices(inputSize);
  deltas_ = NumericalPoint(inputSize);

  for (int i=0; i<inputSize; ++i)
  {
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
  NumericalPoint infBounds(lowerBounds_);
  NumericalPoint supBounds(upperBounds_);
  Indices levels(levels_);
  NumericalPoint deltas(deltas_);

  initializeParameters(getPhysicalModel().getInputs());

  for (UnsignedInteger i=0; i<inputNames.getSize(); ++i)
  {
    const Description::const_iterator it = std::find(inputNames.begin(), inputNames.end(), inputNames_[i]);
    if (it != inputNames.end())
    {
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
//   TODO
//   if (type_ == DesignOfExperimentImplementation::FromBoundsAndDeltas)
//   for (int i=0; i<deltas_.getSize(); ++i)
//     levels_[i] = (lowerBounds[i] - upperBounds[i])/deltas_[i];
  return levels_;
}


void DesignOfExperimentImplementation::setLevels(const Indices & levels)
{
  type_ = DesignOfExperimentImplementation::FromBoundsAndLevels;
  levels_ = levels;
}


NumericalPoint DesignOfExperimentImplementation::getDeltas() const
{
//   TODO
//   if (type_ == DesignOfExperimentImplementation::FromBoundsAndLevels)
//   for (int i=0; i<levels.getSize(); ++i)
//     deltas_[i] = (lowerBounds[i] - upperBounds[i])/levels_[i];
  return deltas_;
}


void DesignOfExperimentImplementation::setDeltas(const NumericalPoint & deltas)
{
  type_ = DesignOfExperimentImplementation::FromBoundsAndDeltas;
  deltas_ = deltas;
}


std::string DesignOfExperimentImplementation::getFileName() const
{
  return fileName_;
}


void DesignOfExperimentImplementation::setFileName(const std::string & fileName, OT::Indices columns)
{
  try
  {
    importSampleFromFile(fileName);

    // check columns list
    if (!columns.getSize())
    {
      columns = Indices(physicalModel_.getInputs().getSize());
      columns.fill();
    }
    setColumns(columns);

    // try to fill inputSample_
    // TODO: put it in getInputSample
    inputSample_ = sampleFromFile_.getMarginal(columns_);
    inputSample_.setDescription(inputNames_);
  }
  catch(std::exception & ex)
  {
    std::cerr<<"Impossible to create a design of experiment from the file "<<fileName<<std::endl;
    throw &ex;
  }

  // set fileName_
  fileName_ = fileName;
}


void DesignOfExperimentImplementation::importSampleFromFile(const std::string & fileName)
{
  std::vector< std::string > separatorsList(3);
  separatorsList[0] = " ";
  separatorsList[1] = ",";
  separatorsList[2] = ";";
  sampleFromFile_.clear();
  for (int i=0; i<3; ++i)
  {
    // import sample from the file
    sampleFromFile_ = NumericalSample::ImportFromTextFile(fileName, separatorsList[i]);
    if (sampleFromFile_.getSize())
      break;
  }
  if (!sampleFromFile_.getSize())
    throw InvalidArgumentException(HERE) << "In DesignOfExperimentImplementation: impossible to load sample\n";
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

  std::set<UnsignedInteger> columnsSet;
  for (UnsignedInteger i=0; i<columns.getSize(); ++i)
    columnsSet.insert(columns[i]);
  if (columnsSet.size() != columns.getSize())
    throw InvalidArgumentException(HERE) << "An input must be associated with only one column.";

  columns_ = columns;
  // TODO: put it in getInputSample
  inputSample_ = sampleFromFile_.getMarginal(columns_);
  inputSample_.setDescription(inputNames_);
}


Experiment DesignOfExperimentImplementation::getExperiment() const
{
  return experiment_;
}


void DesignOfExperimentImplementation::setExperiment(const Experiment & experiment)
{
  experiment_ = experiment;
}


NumericalSample DesignOfExperimentImplementation::getInputSample()
{
  if (!inputSample_.getSize())
  {
    inputSample_.clear();
    NumericalPoint scale(0);
    NumericalPoint transvec(0);
    NumericalPoint otLevels(0);

    for (UnsignedInteger i=0; i<lowerBounds_.getSize(); ++i)
    {
      //TODO: improve this part if a variable is constant
      double inf = lowerBounds_[i];
      double sup = upperBounds_[i];
      scale.add(sup - inf);
      transvec.add(inf);
      if (levels_[i]>1)
        otLevels.add(levels_[i]-2);
      else
        otLevels.add(0);
    }

    if (otLevels.getSize())
    {
      Box box = Box(otLevels);

      inputSample_ = box.generate();
      inputSample_*=scale;
      inputSample_+=transvec;
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
  outputSample_.clear();
  outputSample_ = physicalModel_.getFunction()(inputSample_);
  result_ = ParametricAnalysisResult(inputSample_, outputSample_);
}


std::string DesignOfExperimentImplementation::dump() const
{
  OSS oss;

  oss << physicalModel_.dump();
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
  oss << getName()+ " = otguibase.DesignOfExperiment('" + getName() + "', "+getPhysicalModel().getName()+", lowerBounds, upperBounds, levels)\n";
  return oss.str();
}


}