// SWIG file

%{
#include "persalys/KrigingAnalysisResult.hxx"

namespace OT {
template <>
struct traitsPythonType< KrigingResult >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, KrigingResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("KrigingResult *"), SWIG_POINTER_NO_NULL))) {
      KrigingResult * p_it = reinterpret_cast< KrigingResult * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  KrigingResult
  convert< _PyObject_, KrigingResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("KrigingResult *"), SWIG_POINTER_NO_NULL))) {
      KrigingResult * p_it = reinterpret_cast< KrigingResult * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a KrigingResult";
    }
    return KrigingResult();
  }

}
%}

%include KrigingAnalysisResult_doc.i

%template(KrigingResultCollection) OT::Collection<OT::KrigingResult>;

%copyctor PERSALYS::KrigingAnalysisResult;

%include persalys/KrigingAnalysisResult.hxx
