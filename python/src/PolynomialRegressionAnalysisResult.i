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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("LinearModelResult *"), 0 ))) {
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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("LinearModelResult *"), 0))) {
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

%include persalys/PolynomialRegressionAnalysisResult.hxx
namespace PERSALYS {

%extend PolynomialRegressionAnalysisResult { PolynomialRegressionAnalysisResult(const PolynomialRegressionAnalysisResult & other) { return new PERSALYS::PolynomialRegressionAnalysisResult(other); } 

} }
