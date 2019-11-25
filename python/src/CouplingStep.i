// SWIG file

%{
#include "persalys/CouplingStep.hxx"

namespace OT {

  template <>
  struct traitsPythonType< PERSALYS::CouplingInputFile >
  {
    typedef _PyObject_ Type;
  };


  template <>
  inline
  bool
  canConvert< _PyObject_, PERSALYS::CouplingInputFile >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::CouplingInputFile *"), 0 ))) {
      PERSALYS::CouplingInputFile * p_it = reinterpret_cast< PERSALYS::CouplingInputFile * >( ptr );
      return p_it != NULL;
    }
    return false;
  }

  template <>
  inline
  PERSALYS::CouplingInputFile
  convert< _PyObject_, PERSALYS::CouplingInputFile >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::CouplingInputFile *"), 0))) {
      PERSALYS::CouplingInputFile * p_it = reinterpret_cast< PERSALYS::CouplingInputFile * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a PERSALYS::CouplingInputFile";
    }
    return PERSALYS::CouplingInputFile();
  }

  template <>
  struct traitsPythonType< PERSALYS::CouplingOutputFile >
  {
    typedef _PyObject_ Type;
  };


  template <>
  inline
  bool
  canConvert< _PyObject_, PERSALYS::CouplingOutputFile >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::CouplingOutputFile *"), 0 ))) {
      PERSALYS::CouplingOutputFile * p_it = reinterpret_cast< PERSALYS::CouplingOutputFile * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


  template <>
  inline
  PERSALYS::CouplingOutputFile
  convert< _PyObject_, PERSALYS::CouplingOutputFile >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::CouplingOutputFile *"), 0))) {
      PERSALYS::CouplingOutputFile * p_it = reinterpret_cast< PERSALYS::CouplingOutputFile * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a PERSALYS::CouplingOutputFile";
    }
    return PERSALYS::CouplingOutputFile();
  }
}

%}

%include CouplingStep_doc.i

%template(CouplingInputFileCollection) OT::Collection<PERSALYS::CouplingInputFile>;

%typemap(in) const PERSALYS::CouplingInputFileCollection & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< PERSALYS::CouplingInputFile >( $input );
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of CouplingInputFile");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const PERSALYS::CouplingInputFileCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< PERSALYS::CouplingInputFile >( $input );
}


%template(CouplingOutputFileCollection) OT::Collection<PERSALYS::CouplingOutputFile>;

%typemap(in) const PERSALYS::CouplingOutputFileCollection & {
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< PERSALYS::CouplingOutputFile >( $input );
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of CouplingOutputFile");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const PERSALYS::CouplingOutputFileCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< PERSALYS::CouplingOutputFile >( $input );
}



%include persalys/CouplingStep.hxx
namespace PERSALYS {

%extend CouplingStep { CouplingStep(const CouplingStep & other) { return new PERSALYS::CouplingStep(other); } 

} }
