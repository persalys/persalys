// SWIG file

%{
#include "persalys/PolynomialRegressionAnalysisResult.hxx"

namespace OT {
template <>
struct traitsPythonType< LinearModelResult >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, LinearModelResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("LinearModelResult *"), SWIG_POINTER_NO_NULL))) {
      LinearModelResult * p_it = reinterpret_cast< LinearModelResult * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  LinearModelResult
  convert< _PyObject_, LinearModelResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("LinearModelResult *"), SWIG_POINTER_NO_NULL))) {
      LinearModelResult * p_it = reinterpret_cast< LinearModelResult * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a LinearModelResult";
    }
    return LinearModelResult();
  }

}
%}

%include PolynomialRegressionAnalysisResult_doc.i

%copyctor PERSALYS::PolynomialRegressionAnalysisResult;

%include persalys/PolynomialRegressionAnalysisResult.hxx
