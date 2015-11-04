#include "otgui/YACSPhysicalModel.hxx"

#ifdef OTGUI_HAVE_YACS
#include "YACSEvalYFX.hxx"
#include "YACSEvalPort.hxx"
#include "YACSEvalSeqAny.hxx"
#include "YACSEvalSession.hxx"
#include "YACSEvalResource.hxx"
#endif

namespace OTGUI {

YACSPhysicalModel::YACSPhysicalModel(const std::string& name)
  : PhysicalModelImplementation(name)
  , XMLFile_("")
{
}

YACSPhysicalModel::YACSPhysicalModel(const std::string& name, const std::string & fileName)
  : PhysicalModelImplementation(name)
  , XMLFile_(fileName)
{
//check filename?
}


YACSPhysicalModel::YACSPhysicalModel(const YACSPhysicalModel & other)
 : PhysicalModelImplementation(other)
 , XMLFile_(other.XMLFile_)
{
}


YACSPhysicalModel* YACSPhysicalModel::clone() const
{
  return new YACSPhysicalModel(*this);
}


YACSPhysicalModel::~YACSPhysicalModel()
{

}


std::string YACSPhysicalModel::getXMLFile() const
{
  return XMLFile_;
}


void YACSPhysicalModel::setXMLFile(const std::string & fileName)
{
  XMLFile_ = fileName;
}


void YACSPhysicalModel::loadDataWithYACS()
{
  YACSEvalSession session;
  session.launch();

//   TODO: check if fileName not empty
  YACSEvalYFX *efx = YACSEvalYFX::BuildFromFile(XMLFile_);

  std::list< YACSEvalInputPort * > inps(efx->getFreeInputPorts());
  std::list< YACSEvalOutputPort * > outps(efx->getFreeOutputPorts());

  for (std::list<YACSEvalInputPort *>::iterator it=inps.begin(); it!=inps.end(); ++it)
  {
    Input newInput = Input((*it)->getName(), (*it)->getDefaultValueDefined()->toDouble());
    addInput(newInput);
  }
  notify("inputChanged");

  for (std::list<YACSEvalOutputPort *>::iterator it=outps.begin(); it!=outps.end(); ++it)
  {
    Output newOutput = Output((*it)->getName());
    addOutput(newOutput);
  }
  notify("outputChanged");

  delete efx;
}


void YACSPhysicalModel::loadDataWithYACS(const std::string & fileName)
{
  setXMLFile(fileName);
  loadDataWithYACS();
}


}