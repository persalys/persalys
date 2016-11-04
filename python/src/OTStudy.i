// SWIG file

%{
#include "otgui/OTStudy.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::OTStudy >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  OTGUI::OTStudy
  convert< _PyObject_, OTGUI::OTStudy >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::OTStudy *"), 0))) {
      OTGUI::OTStudy * p_it = reinterpret_cast< OTGUI::OTStudy * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a OTStudy";
    }
    return OTGUI::OTStudy();
  }
}
%}

%include OTStudy_doc.i

%ignore *::SetInstanceObserver;

OTGUITypedInterfaceObjectHelper(OTStudy)
OTGUITypedCollectionInterfaceObjectHelper(OTStudy)

%include otgui/OTStudy.hxx
namespace OTGUI {

%extend OTStudy { OTStudy(const OTStudy & other) { return new OTGUI::OTStudy(other); }

std::string __repr__() {
  return "OTStudy";
}

} }
