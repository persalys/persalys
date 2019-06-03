// SWIG file

%{
#include "persalys/Variable.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::Variable >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, PERSALYS::Variable >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::Variable *"), 0 ))) {
      PERSALYS::Variable * p_it = reinterpret_cast< PERSALYS::Variable * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  PERSALYS::Variable
  convert< _PyObject_, PERSALYS::Variable >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::Variable *"), 0))) {
      PERSALYS::Variable * p_it = reinterpret_cast< PERSALYS::Variable * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Variable";
    }
    return PERSALYS::Variable();
  }

}
%}

%include Variable_doc.i

%template(VariableCollection) OT::Collection<PERSALYS::Variable>;

%typemap(in) const VariableCollection & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< PERSALYS::Variable >( $input );
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Variable");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const VariableCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< PERSALYS::Variable >( $input );
}

%apply const VariableCollection & { const PERSALYS::VariableCollection & };

%include persalys/Variable.hxx
namespace PERSALYS {

%extend Variable { Variable(const Variable & other) { return new PERSALYS::Variable(other); } 

std::string __repr__() {
  return "Variable";
}

} }
