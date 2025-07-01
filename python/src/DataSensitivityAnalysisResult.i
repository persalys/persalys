// SWIG file

%{
#include "persalys/DataSensitivityAnalysisResult.hxx"

namespace OT {
template <>
struct traitsPythonType< OT::Interval >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, OT::Interval >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Interval *"), 0 ))) {
      OT::Interval * p_it = reinterpret_cast< OT::Interval * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  OT::Interval
  convert< _PyObject_, OT::Interval >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Interval *"), 0))) {
      OT::Interval * p_it = reinterpret_cast< OT::Interval * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Point";
    }
    return OT::Interval();
  }

}
%}

%include DataSensitivityAnalysisResult_doc.i

%copyctor PERSALYS::DataSensitivityAnalysisResult;

%include persalys/DataSensitivityAnalysisResult.hxx

%template(_IntervalCollection) OT::Collection<OT::Interval>;