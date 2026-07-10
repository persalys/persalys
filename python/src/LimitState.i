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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::LimitState *"), SWIG_POINTER_NO_NULL))) {
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

%copyctor PERSALYS::LimitState;

%include persalys/LimitState.hxx
namespace PERSALYS {

%extend LimitState {

std::string __repr__() {
  return "LimitState";
}

%pythoncode %{
    Union = LimitStateImplementation.Union
    Intersection = LimitStateImplementation.Intersection
%}

} }
