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
YACSEvaluation::YACSEvaluation(const String & fileName)
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
  efx_ = YACSEvalYFX::BuildFromFile(xmlFileName_);

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
    std::cerr<< "inps.size() != inS.getDimension()\n";
    throw;
  }

  std::vector< YACSEvalOutputPort * > outps0(efx_->getFreeOutputPorts());
  std::vector< YACSEvalOutputPort * > outps;

  if (getOutputDimension() == outps0.size())
    outps = outps0;
  else
    for (int i=0; i<getOutputDimension(); ++i)
      for (std::vector<YACSEvalOutputPort *>::iterator it = outps0.begin(); it != outps0.end(); it++)
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
    inps[i]->setSequenceOfValuesToEval(&ds);
  }

  // launch analysis
  NumericalSample result(inS.getSize(), getOutputDimension());
  result.setDescription(getOutputVariablesNames());

  efx_->lockPortsForEvaluation(inps, outps);
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
String YACSEvaluation::getXMLFileName() const
{
  return xmlFileName_;
}


void YACSEvaluation::setXMLFileName(const String & xmlFileName)
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