// SWIG file

%{
#include "persalys/Input.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::Input >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, PERSALYS::Input >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::Input *"), 0 ))) {
      PERSALYS::Input * p_it = reinterpret_cast< PERSALYS::Input * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  PERSALYS::Input
  convert< _PyObject_, PERSALYS::Input >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::Input *"), 0))) {
      PERSALYS::Input * p_it = reinterpret_cast< PERSALYS::Input * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Input";
    }
    return PERSALYS::Input();
  }

}
%}

%include Input_doc.i

%template(InputCollection) OT::Collection<PERSALYS::Input>;

%typemap(in) const InputCollection & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< PERSALYS::Input >( $input );
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Input");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const InputCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< PERSALYS::Input >( $input );
}

%apply const InputCollection & { const PERSALYS::InputCollection & };

%include persalys/Input.hxx
namespace PERSALYS {

%extend Input { Input(const Input & other) { return new PERSALYS::Input(other); } 

} }
