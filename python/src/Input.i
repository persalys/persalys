// SWIG file

%{
#include "otgui/Input.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::Input >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  OTGUI::Input
  convert< _PyObject_, OTGUI::Input >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::Input *"), 0))) {
      OTGUI::Input * p_it = reinterpret_cast< OTGUI::Input * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Input";
    }
    return OTGUI::Input();
  }

}
%}

%include Input_doc.i

%template(InputCollection) OT::Collection<OTGUI::Input>;


%include otgui/Input.hxx
namespace OTGUI {

%extend Input { Input(const Input & other) { return new OTGUI::Input(other); } 

std::string __repr__() {
  return "Input";
}

} }
