#ifndef __PVSERVERMANAGERINTERFACE_HXX__
#define __PVSERVERMANAGERINTERFACE_HXX__

#include "persalys/PersalysPrivate.hxx"

#include <openturns/Exception.hxx>

#include <pqActiveObjects.h>

class pqServer;

namespace PERSALYS
{
class PERSALYS_API PVServerManagerInterface
{
public:
  virtual ~PVServerManagerInterface() {}

  virtual pqServer * fetchServer(bool * isRemote = 0)
  {
    pqServer * ret(pqActiveObjects::instance().activeServer());
    if(!ret)
      throw OT::InvalidArgumentException(HERE) << "Ooops no server !";
    if(isRemote)
    {
      if(!ret)
        *isRemote = false;
      else
        *isRemote = ret->isRemote();
    }
    return ret;
  }
};
}
#endif
