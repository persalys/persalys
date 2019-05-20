#include "persalys/PVServerManagerSingleton.hxx"

namespace PERSALYS
{
PVServerManagerInterface * PVServerManagerSingleton::singleton_ = 0;

PVServerManagerInterface * PVServerManagerSingleton::Get()
{
  return singleton_;
}


void PVServerManagerSingleton::Reset()
{
  if(singleton_)
    delete singleton_;
  singleton_ = 0;
}


void PVServerManagerSingleton::Init(PVServerManagerInterface * pvsm)
{
  singleton_ = pvsm;
}
}
