#ifndef __PVSERVERMANAGERSINGLETON_HXX__
#define __PVSERVERMANAGERSINGLETON_HXX__

#include "persalys/PersalysPrivate.hxx"

#include "persalys/PVServerManagerInterface.hxx"

namespace PERSALYS
{
class PERSALYS_PLOTPV_API PVServerManagerSingleton
{
public:
  static PVServerManagerInterface * Get();
  static void Reset();
  static void Init(PVServerManagerInterface * pvsm);

private:
  PVServerManagerSingleton() {}
  ~PVServerManagerSingleton() {}
  static PVServerManagerInterface * singleton_;
};
}
#endif
