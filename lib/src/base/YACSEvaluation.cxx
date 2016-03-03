//                                               -*- C++ -*-
/**
 *  @brief YACSEvaluation implements the evaluation of the models defined by an xml file
 *
 *  Copyright 2015-2016 EDF
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/YACSEvaluation.hxx"
#include "PersistentObjectFactory.hxx"

#include "YACSEvalPort.hxx"
#include "YACSEvalSeqAny.hxx"
#include "YACSEvalResource.hxx"

using namespace OT;

namespace OTGUI {

CLASSNAMEINIT(YACSEvaluation);

static Factory<YACSEvaluation> RegisteredFactory("YACSEvaluation");

YACSEvalSession * YACSEvaluation::session_ = 0;

/* Default constructor */
YACSEvaluation::YACSEvaluation(const String & fileName)
  : NumericalMathEvaluationImplementation()
  , xmlFileName_(fileName)
  , efx_(0)
  , parallelizeStatus_(true)
  , wantedMachine_("localhost")
{
  if (!session_)
  {
    session_ = new YACSEvalSession;
    session_->launch();
  }
  if (!xmlFileName_.empty())
    loadData();
}


/* Virtual constructor */
YACSEvaluation* YACSEvaluation::clone() const
{
  return new YACSEvaluation(*this);
}


/* Comparison operator */
Bool YACSEvaluation::operator ==(const YACSEvaluation & other) const
{
  return (xmlFileName_ == other.xmlFileName_);
}


/* String converter */
String YACSEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << YACSEvaluation::GetClassName()
      << " name=" << getName()
      << " xml=" << xmlFileName_;
  return oss;
}


/* String converter */
String YACSEvaluation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getInputDescription() << " xml=" << xmlFileName_;
  return oss;
}


/* Method loadData() loads the data from the xmlFileName */
void YACSEvaluation::loadData()
{
  // read file
  efx_ = YACSEvalYFX::BuildFromFile(xmlFileName_);

  // get variables information
  std::vector< YACSEvalInputPort * > inps(efx_->getFreeInputPorts());
  std::vector< YACSEvalOutputPort * > outps(efx_->getFreeOutputPorts());

  inputValues_ = NumericalPoint(inps.size());
  inDescription_ = Description(inps.size());
  outDescription_ = Description(outps.size());

  for (int i=0; i<inps.size(); ++i)
  {
    inputValues_[i] = inps[i]->getDefaultValueDefined()->toDouble();
    inDescription_[i] = inps[i]->getName();
  }

  for (int i=0; i<outps.size(); ++i)
    outDescription_[i] = outps[i]->getName();

  // get parameters
  parallelizeStatus_ = efx_->getParallelizeStatus();

  efx_->lockPortsForEvaluation(inps, outps);
  YACSEvalListOfResources *rss(efx_->giveResources());
  fittingMachines_ = Description(rss->getAllFittingMachines().size());
  for (int i=0; i<rss->getAllFittingMachines().size(); ++i)
    fittingMachines_[i] = rss->getAllFittingMachines()[i];

  efx_->unlockAll();
}


/* Operator () */
NumericalPoint YACSEvaluation::operator() (const NumericalPoint & inP) const
{
  return operator()(NumericalSample(1, inP))[0];
}


/* Operator () */
NumericalSample YACSEvaluation::operator() (const NumericalSample & inS) const
{
  std::vector< YACSEvalInputPort * > inps(efx_->getFreeInputPorts());

  if (inps.size() != inS.getDimension())
  {
    std::cerr<< "In YACSEvaluation::operator() inps.size() != inS.getDimension()\n";
    throw InvalidArgumentException(HERE) << "The dimension of the input sample " << inS.getDimension() << " is not valid";
  }

  std::vector< YACSEvalOutputPort * > outps0(efx_->getFreeOutputPorts());
  std::vector< YACSEvalOutputPort * > outps;

  if (getOutputDimension() == outps0.size())
    outps = outps0;
  else
    for (UnsignedInteger i=0; i<getOutputDimension(); ++i)
      for (std::vector<YACSEvalOutputPort *>::iterator it = outps0.begin(); it != outps0.end(); it++)
        if (getOutputVariablesNames()[i] == (*it)->getName())
        {
          outps.push_back(*it);
          break;
        }

  if (outps.size() != getOutputDimension())
  {
    std::cerr<< "outps.size() != getOutputDimension()\n";
    throw InvalidArgumentException(HERE) << "The dimension of the output sample " << getOutputDimension() << " is not valid";
  }

  for (UnsignedInteger i=0; i<inS.getDimension(); ++i)
  {
    std::vector<double> tab(inS.getSize());
    for (UnsignedInteger j=0; j<inS.getSize(); ++j)
      tab[j] = inS[j][i];
    
    YACSEvalSeqAnyDouble ds(tab);
    inps[i]->setSequenceOfValuesToEval(&ds);
  }

  // launch analysis
  NumericalSample result(inS.getSize(), getOutputDimension());
  result.setDescription(getOutputVariablesNames());

  efx_.get()->lockPortsForEvaluation(inps, outps);
  efx_->getUndergroundGeneratedGraph();

  int b = 0;
  bool a(efx_.get()->run(session_, b));
  if (!a)
  {
    efx_.get()->unlockAll();
    throw NotDefinedException(HERE) << "Error when executing YACS scheme. ";
  }

  // get results
  std::vector<YACSEvalSeqAny *> res(efx_->getResults());

  for (int k=0; k<outps.size(); ++k)
  {
    YACSEvalSeqAnyDouble *res_k(dynamic_cast<YACSEvalSeqAnyDouble *>(res[k]));
    for (int h=0; h<res_k->size(); ++h)
      result[h][k] = res_k->getInternal()->at(h);
  }
  efx_.get()->unlockAll();
  return result;
}


/* Accessor for input values */
NumericalPoint YACSEvaluation::getInputValues() const
{
  return inputValues_;
}


/* Accessor for input values */
void YACSEvaluation::setInputValues(const NumericalPoint & inP)
{
  inputValues_ = inP;
}


Description YACSEvaluation::getInputVariablesNames() const
{
  return inDescription_;
}


/* Accessor for input point dimension */
UnsignedInteger YACSEvaluation::getInputDimension() const
{
  return getInputVariablesNames().getSize();
}


Description YACSEvaluation::getOutputVariablesNames() const
{
  return outDescription_;
}


void YACSEvaluation::setOutputVariablesNames(const Description & outDescription)
{
  outDescription_ = outDescription;
}


/* Accessor for output point dimension */
UnsignedInteger YACSEvaluation::getOutputDimension() const
{
  return getOutputVariablesNames().getSize();
}


/* Accessor to the parallelize status */
bool YACSEvaluation::getParallelizeStatus() const
{
  return parallelizeStatus_;
}


/* Accessor to the parallelize status */
void YACSEvaluation::setParallelizeStatus(const bool & status)
{
  efx_.get()->setParallelizeStatus(parallelizeStatus_);
  parallelizeStatus_ = status;
}


/* Accessor to the fitting machines */
Description YACSEvaluation::getFittingMachines() const
{
  return fittingMachines_;
}


/* Accessor to the wanted machine */
String YACSEvaluation::getWantedMachine() const
{
  return wantedMachine_;
}


/* Accessor to the wanted machine */
void YACSEvaluation::setWantedMachine(const String & machine)
{
  efx_.get()->giveResources()->setWantedMachine(wantedMachine_);
  wantedMachine_ = machine;
}


/* Accessor to the formulas */
String YACSEvaluation::getXMLFileName() const
{
  return xmlFileName_;
}


void YACSEvaluation::setXMLFileName(const String & xmlFileName)
{
  xmlFileName_ = xmlFileName;
}


/* Method save() stores the object through the StorageManager */
void YACSEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute("xmlFileName_", xmlFileName_);
  adv.saveAttribute("parallelizeStatus_", parallelizeStatus_);
  adv.saveAttribute("wantedMachine_", wantedMachine_);
}


/* Method load() reloads the object from the StorageManager */
void YACSEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute("xmlFileName_", xmlFileName_);
  if (!xmlFileName_.empty())
    loadData();
  adv.loadAttribute("parallelizeStatus_", parallelizeStatus_);
  adv.loadAttribute("wantedMachine_", wantedMachine_);
}
}
