// SWIG file

%{
#include "Output.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::Output >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  OTGUI::Output
  convert< _PyObject_, OTGUI::Output >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::Output *"), 0))) {
      OTGUI::Output * p_it = reinterpret_cast< OTGUI::Output * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Output";
    }
    return OTGUI::Output();
  }

}
%}

%template(OutputCollection) OT::Collection<OTGUI::Output>;




%include Output.hxx



namespace OTGUI {

%extend Output { Output(const Output & other) { return new OTGUI::Output(other); } 

std::string __repr__() {
  return "Output";
}

} }
