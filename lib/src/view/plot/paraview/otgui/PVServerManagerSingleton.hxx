#ifndef __PVSERVERMANAGERSINGLETON_HXX__
#define __PVSERVERMANAGERSINGLETON_HXX__
#include "otgui/OTGuiprivate.hxx"

#include "otgui/PVServerManagerInterface.hxx"

namespace OTGUI {
class OTGUI_API PVServerManagerSingleton
{
public:
  static PVServerManagerInterface * Get();
  static void Reset();
  static void Init(PVServerManagerInterface * pvsm);

private:
  PVServerManagerSingleton(){}
  static PVServerManagerInterface * singleton_;
};
}
#endif // __PVSERVERMANAGERSINGLETON_HXX__
