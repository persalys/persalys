#ifndef __PVSERVERMANAGERINTERFACE_HXX__
#define __PVSERVERMANAGERINTERFACE_HXX__

#include "otgui/OTGuiprivate.hxx"

class pqServer;

class OTGUI_API PVServerManagerInterface
{
public:
  virtual pqServer *fetchServer(bool *isRemote=0) = 0;
};

#endif
