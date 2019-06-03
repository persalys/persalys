// SWIG file

%{
#include "persalys/Output.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::Output >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, PERSALYS::Output >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::Output *"), 0 ))) {
      PERSALYS::Output * p_it = reinterpret_cast< PERSALYS::Output * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  PERSALYS::Output
  convert< _PyObject_, PERSALYS::Output >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::Output *"), 0))) {
      PERSALYS::Output * p_it = reinterpret_cast< PERSALYS::Output * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Output";
    }
    return PERSALYS::Output();
  }

}
%}

%include Output_doc.i

%template(OutputCollection) OT::Collection<PERSALYS::Output>;

%typemap(in) const OutputCollection & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< PERSALYS::Output >( $input );
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Output");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OutputCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< PERSALYS::Output >( $input );
}

%apply const OutputCollection & { const PERSALYS::OutputCollection & };

%include persalys/Output.hxx
namespace PERSALYS {

%extend Output { Output(const Output & other) { return new PERSALYS::Output(other); } 

} }
