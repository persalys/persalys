// SWIG file CouplingPhysicalModel.i

%{
#include "persalys/CouplingPhysicalModel.hxx"

namespace OT {

  template <>
  struct traitsPythonType< PERSALYS::CouplingStep >
  {
    typedef _PyObject_ Type;
  };


  template <>
  inline
  bool
  canConvert< _PyObject_, PERSALYS::CouplingStep >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::CouplingStep *"), 0 ))) {
      PERSALYS::CouplingStep * p_it = reinterpret_cast< PERSALYS::CouplingStep * >( ptr );
      return p_it != NULL;
    }
    return false;
  }

  template <>
  inline
  PERSALYS::CouplingStep
  convert< _PyObject_, PERSALYS::CouplingStep >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::CouplingStep *"), 0))) {
      PERSALYS::CouplingStep * p_it = reinterpret_cast< PERSALYS::CouplingStep * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a PERSALYS::CouplingStep";
    }
    return PERSALYS::CouplingStep();
  }
}
%}

%include CouplingPhysicalModel_doc.i

%template(CouplingStepCollection) OT::Collection<PERSALYS::CouplingStep>;

%typemap(in) const PERSALYS::CouplingStepCollection & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< PERSALYS::CouplingStep >( $input );
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of CouplingStep");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const PERSALYS::CouplingStepCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< PERSALYS::CouplingStep >( $input );
}


%include persalys/CouplingPhysicalModel.hxx
namespace PERSALYS {

%extend CouplingPhysicalModel { CouplingPhysicalModel(const CouplingPhysicalModel & other) { return new PERSALYS::CouplingPhysicalModel(other); } 

} }
