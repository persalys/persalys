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
  bool
  canConvert< _PyObject_, OTGUI::Input >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::Input *"), 0 ))) {
      OTGUI::Input * p_it = reinterpret_cast< OTGUI::Input * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


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

%typemap(in) const InputCollection & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< OTGUI::Input >( $input );
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Input");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const InputCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< OTGUI::Input >( $input );
}

%apply const InputCollection & { const OTGUI::InputCollection & };

%include otgui/Input.hxx
namespace OTGUI {

%extend Input { Input(const Input & other) { return new OTGUI::Input(other); } 

} }
