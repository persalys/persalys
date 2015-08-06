//g++ -c -I/local00/home/H87074/salome/DEV/modules/install/YACS_V7_6_BR/include/salome main.cxx
//g++ main.o -L${YACS_ROOT_DIR}/lib/salome -lYACSevalYFX 

#include "YACSEvalYFX.hxx"
#include "YACSEvalPort.hxx"
#include "YACSEvalSeqAny.hxx"
#include "YACSEvalSession.hxx"
#include "YACSEvalResource.hxx"

#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

int main(int argc, char *argv[])
{
  static const char fileName[]="example.xml";
  YACSEvalSession session;
  session.launch();
  YACSEvalYFX *efx=YACSEvalYFX::BuildFromFile(fileName);
  std::list< YACSEvalInputPort * > inps(efx->getFreeInputPorts());
  std::vector< YACSEvalInputPort * > inps2(inps.begin(),inps.end());
  std::list< YACSEvalOutputPort * > outps(efx->getFreeOutputPorts());
  //
  {
    YACSEvalAnyDouble d(3.4);
    inps2[0]->setDefaultValue(&d);
  }
  {
    static const double tab[]={10.1,10.2,10.3};
    std::vector<double> tab2(tab,tab+sizeof(tab)/sizeof(double));
    YACSEvalSeqAnyDouble ds(tab2);
    inps2[1]->setSequenceOfValuesToEval(&ds);
  }
  //
  efx->lockPortsForEvaluation(outps);
  efx->getUndergroundGeneratedGraph();
  YACSEvalListOfResources *rss(efx->giveResources());
  rss->setWantedMachine("localhost");
  int b;
  bool a(efx->run(&session,b));
  std::vector<YACSEvalSeqAny *> res(efx->getResults());
  YACSEvalSeqAnyDouble *res0(dynamic_cast<YACSEvalSeqAnyDouble *>(res[0]));
  std::copy(res0->getInternal()->begin(),res0->getInternal()->end(),std::ostream_iterator<double>(std::cerr," "));
  std::cerr << "********************" << std::endl;
  delete efx;
  return 0;
}
