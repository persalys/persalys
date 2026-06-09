// SWIG file

%{
#include "persalys/CopulaInferenceSetResult.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::CopulaInferenceSetResult >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, PERSALYS::CopulaInferenceSetResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::CopulaInferenceSetResult *"), SWIG_POINTER_NO_NULL))) {
      PERSALYS::CopulaInferenceSetResult * p_it = reinterpret_cast< PERSALYS::CopulaInferenceSetResult * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  PERSALYS::CopulaInferenceSetResult
  convert< _PyObject_, PERSALYS::CopulaInferenceSetResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::CopulaInferenceSetResult *"), SWIG_POINTER_NO_NULL))) {
      PERSALYS::CopulaInferenceSetResult * p_it = reinterpret_cast< PERSALYS::CopulaInferenceSetResult * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a CopulaInferenceSetResult";
    }
    return PERSALYS::CopulaInferenceSetResult();
  }

}
%}

%include CopulaInferenceSetResult_doc.i

%copyctor PERSALYS::CopulaInferenceSetResult;

%include persalys/CopulaInferenceSetResult.hxx

%template(CopulaInferenceSetResultCollection) OT::Collection<PERSALYS::CopulaInferenceSetResult>;
