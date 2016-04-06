// SWIG file

%{
#include "otgui/LimitState.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::LimitState >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  OTGUI::LimitState
  convert< _PyObject_, OTGUI::LimitState >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::LimitState *"), 0))) {
      OTGUI::LimitState * p_it = reinterpret_cast< OTGUI::LimitState * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a LimitState";
    }
    return OTGUI::LimitState();
  }
}
%}

%include LimitState_doc.i

OTGUITypedInterfaceObjectHelper(LimitState)
OTGUITypedCollectionInterfaceObjectHelper(LimitState)

%include otgui/LimitState.hxx
namespace OTGUI {

%extend LimitState { LimitState(const LimitState & other) { return new OTGUI::LimitState(other); } 

std::string __repr__() {
  return "LimitState";
}

} }
