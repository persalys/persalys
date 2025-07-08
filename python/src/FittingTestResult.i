// SWIG file

%{
#include "persalys/FittingTestResult.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::FittingTestResult >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, PERSALYS::FittingTestResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::FittingTestResult *"), 0 ))) {
      PERSALYS::FittingTestResult * p_it = reinterpret_cast< PERSALYS::FittingTestResult * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  PERSALYS::FittingTestResult
  convert< _PyObject_, PERSALYS::FittingTestResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::FittingTestResult *"), 0))) {
      PERSALYS::FittingTestResult * p_it = reinterpret_cast< PERSALYS::FittingTestResult * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a FittingTestResult";
    }
    return PERSALYS::FittingTestResult();
  }

}
%}

%include FittingTestResult_doc.i

%copyctor PERSALYS::FittingTestResult;

%include persalys/FittingTestResult.hxx

%template(FittingTestResultCollection) OT::Collection<PERSALYS::FittingTestResult>;
