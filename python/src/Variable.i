// SWIG file

%{
#include "otgui/Variable.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::Variable >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, OTGUI::Variable >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::Variable *"), 0 ))) {
      OTGUI::Variable * p_it = reinterpret_cast< OTGUI::Variable * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  OTGUI::Variable
  convert< _PyObject_, OTGUI::Variable >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::Variable *"), 0))) {
      OTGUI::Variable * p_it = reinterpret_cast< OTGUI::Variable * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Variable";
    }
    return OTGUI::Variable();
  }

}
%}

%include Variable_doc.i

%template(VariableCollection) OT::Collection<OTGUI::Variable>;

%typemap(in) const VariableCollection & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< OTGUI::Variable >( $input );
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Variable");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const VariableCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< OTGUI::Variable >( $input );
}

%apply const VariableCollection & { const OTGUI::VariableCollection & };

%include otgui/Variable.hxx
namespace OTGUI {

%extend Variable { Variable(const Variable & other) { return new OTGUI::Variable(other); } 

std::string __repr__() {
  return "Variable";
}

} }
