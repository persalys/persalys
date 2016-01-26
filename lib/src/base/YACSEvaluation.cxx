// YACSEvaluation.cxx

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
YACSEvaluation::YACSEvaluation(const std::string & fileName)
  : NumericalMathEvaluationImplementation()
  , xmlFileName_(fileName)
  , efx_(0)
{
  if (!session_)
  {
    session_ = new YACSEvalSession;
    session_->launch();
  }
}


YACSEvaluation::YACSEvaluation(const YACSEvaluation & other)
  : xmlFileName_(other.xmlFileName_)
  , inputValues_(other.inputValues_)
  , inDescription_(other.inDescription_)
  , outDescription_(other.outDescription_)
  , efx_(0)
{
  if (!session_)
  {
    session_ = new YACSEvalSession;
    session_->launch();
  }
  efx_ = YACSEvalYFX::BuildFromFile(xmlFileName_);
}


/* Virtual constructor */
YACSEvaluation* YACSEvaluation::clone() const
{
  YACSEvaluation * result = new YACSEvaluation(*this);
  return result;
}


YACSEvaluation::~YACSEvaluation()
{
  delete efx_;
}


/* Comparison operator */
Bool YACSEvaluation::operator ==(const YACSEvaluation & other) const
{
  return (xmlFileName_ == other.xmlFileName_);
}


/* std::string converter */
std::string YACSEvaluation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << YACSEvaluation::GetClassName()
      << " name=" << getName()
      << " xml=" << xmlFileName_;
  return oss;
}


/* std::string converter */
std::string YACSEvaluation::__str__(const std::string & offset) const
{
  OSS oss(false);
  oss << offset << getInputDescription() << " xml=" << xmlFileName_;
  return oss;
}


/* Method loadData() loads the data from the xmlFileName */
void YACSEvaluation::loadData()
{
  efx_ = YACSEvalYFX::BuildFromFile(xmlFileName_);

  std::list< YACSEvalInputPort * > inps(efx_->getFreeInputPorts());
  std::list< YACSEvalOutputPort * > outps(efx_->getFreeOutputPorts());

  inputValues_ = NumericalPoint(inps.size());
  inDescription_ = Description(inps.size());
  outDescription_ = Description(outps.size());

  int i = 0;
  for (std::list<YACSEvalInputPort *>::iterator it=inps.begin(); it!=inps.end(); ++it, i++)
  {
    inputValues_[i] = (*it)->getDefaultValueDefined()->toDouble();
    inDescription_[i] = (*it)->getName();
  }

  i = 0;
  for (std::list<YACSEvalOutputPort *>::iterator it=outps.begin(); it!=outps.end(); ++it, i++)
    outDescription_[i] = (*it)->getName();
}


/* Operator () */
NumericalPoint YACSEvaluation::operator() (const NumericalPoint & inP) const
{
  return operator()(NumericalSample(1, inP))[0];
}


/* Operator () */
NumericalSample YACSEvaluation::operator() (const NumericalSample & inS) const
{
  std::list< YACSEvalInputPort * > inps(efx_->getFreeInputPorts());
  std::vector< YACSEvalInputPort * > inps2(inps.begin(),inps.end());

  if (inps2.size() != inS.getDimension())
  {
    std::cerr<< "inps2.size() != inS.getDimension()\n";
    throw;
  }

  std::list< YACSEvalOutputPort * > outps0(efx_->getFreeOutputPorts());
  std::list< YACSEvalOutputPort * > outps;

  if (getOutputDimension() == outps0.size())
    outps = outps0;
  else
    for (int i=0; i<getOutputDimension(); ++i)
      for (std::list<YACSEvalOutputPort *>::iterator it = outps0.begin(); it != outps0.end(); it++)
        if (getOutputVariablesNames()[i] == (*it)->getName())
        {
          outps.push_back(*it);
          break;
        }

  if (outps.size() != getOutputDimension())
  {
    std::cerr<< "outps.size() != getOutputDimension()\n";
    throw;
  }

  for (int i=0; i<inS.getDimension(); ++i)
  {
    std::vector<double> tab(inS.getSize());
    for (int j=0; j<inS.getSize(); ++j)
      tab[j] = inS[j][i];
    
    YACSEvalSeqAnyDouble ds(tab);
    inps2[i]->setSequenceOfValuesToEval(&ds);
  }

  // launch analysis
  NumericalSample result(inS.getSize(), getOutputDimension());
  result.setDescription(getOutputVariablesNames());

  efx_->lockPortsForEvaluation(outps);
  efx_->getUndergroundGeneratedGraph();
  YACSEvalListOfResources *rss(efx_->giveResources());
  rss->setWantedMachine("localhost");
  int b;
  bool a(efx_->run(session_, b));
  if (!a)
    return result;

  // get results
  std::vector<YACSEvalSeqAny *> res(efx_->getResults());

  for (int k=0; k<outps.size(); ++k)
  {
    YACSEvalSeqAnyDouble *res_k(dynamic_cast<YACSEvalSeqAnyDouble *>(res[k]));
    for (int h=0; h<res_k->size(); ++h)
      result[h][k] = res_k->getInternal()->at(h);
  }

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


/* Accessor to the formulas */
std::string YACSEvaluation::getXMLFileName() const
{
  return xmlFileName_;
}


void YACSEvaluation::setXMLFileName(const std::string & xmlFileName)
{
  xmlFileName_ = xmlFileName;
  try
  {
    loadData();
  }
  catch(std::exception)
  {
    std::cerr<<"Impossible to load data from the xml file "<<xmlFileName<<std::endl;
    throw;
  }
}


/* Method save() stores the object through the StorageManager */
void YACSEvaluation::save(Advocate & adv) const
{
  NumericalMathEvaluationImplementation::save(adv);
  adv.saveAttribute("xmlFileName_", xmlFileName_);
}


/* Method load() reloads the object from the StorageManager */
void YACSEvaluation::load(Advocate & adv)
{
  NumericalMathEvaluationImplementation::load(adv);
  adv.loadAttribute("xmlFileName_", xmlFileName_);
  *this = YACSEvaluation(xmlFileName_);
}
}