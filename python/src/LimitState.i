// SWIG file

%{
#include "persalys/LimitState.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::LimitState >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  PERSALYS::LimitState
  convert< _PyObject_, PERSALYS::LimitState >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::LimitState *"), 0))) {
      PERSALYS::LimitState * p_it = reinterpret_cast< PERSALYS::LimitState * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a LimitState";
    }
    return PERSALYS::LimitState();
  }
}
%}

%include LimitState_doc.i

PERSALYSTypedInterfaceObjectHelper(LimitState)
PERSALYSTypedCollectionInterfaceObjectHelper(LimitState)

%include persalys/LimitState.hxx
namespace PERSALYS {

%extend LimitState { LimitState(const LimitState & other) { return new PERSALYS::LimitState(other); } 

std::string __repr__() {
  return "LimitState";
}

} }
