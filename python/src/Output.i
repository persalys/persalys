// SWIG file

%{
#include "otgui/Output.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::Output >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, OTGUI::Output >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::Output *"), 0 ))) {
      OTGUI::Output * p_it = reinterpret_cast< OTGUI::Output * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


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

%include Output_doc.i

%template(OutputCollection) OT::Collection<OTGUI::Output>;

%typemap(in) const OutputCollection & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< OTGUI::Output >( $input );
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Output");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OutputCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< OTGUI::Output >( $input );
}

%apply const OutputCollection & { const OTGUI::OutputCollection & };

%include otgui/Output.hxx
namespace OTGUI {

%extend Output { Output(const Output & other) { return new OTGUI::Output(other); } 

std::string __repr__() {
  return "Output";
}

} }
