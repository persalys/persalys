// SWIG file

%{
#include "persalys/DataSensitivityAnalysis.hxx"

#include "openturns/CovarianceModel.hxx"

namespace OT {
template <>
struct traitsPythonType< OT::CovarianceModel >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  OT::CovarianceModel
  convert< _PyObject_, OT::CovarianceModel >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::CovarianceModel *"), 0))) {
      OT::CovarianceModel * p_it = reinterpret_cast< OT::CovarianceModel * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a CovarianceModel";
    }
    return OT::CovarianceModel();
  }

template <>
  inline
  bool
  canConvert< _PyObject_, OT::CovarianceModel >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::CovarianceModel *"), 0 ))) {
      OT::CovarianceModel * p_it = reinterpret_cast< OT::CovarianceModel * >( ptr );
      return p_it != NULL;
    }
    return false;
  }
}
%}


%include DataSensitivityAnalysis_doc.i

%copyctor PERSALYS::DataSensitivityAnalysis;

%include persalys/DataSensitivityAnalysis.hxx