// SWIG file

%{
#include "persalys/FieldMonteCarloResult.hxx"

namespace OT {
template <>
struct traitsPythonType< KarhunenLoeveResult >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, KarhunenLoeveResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("KarhunenLoeveResult *"), SWIG_POINTER_NO_NULL))) {
      KarhunenLoeveResult * p_it = reinterpret_cast< KarhunenLoeveResult * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  KarhunenLoeveResult
  convert< _PyObject_, KarhunenLoeveResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("KarhunenLoeveResult *"), SWIG_POINTER_NO_NULL))) {
      KarhunenLoeveResult * p_it = reinterpret_cast< KarhunenLoeveResult * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a KarhunenLoeveResult";
    }
    return KarhunenLoeveResult();
  }

}
%}

%include FieldMonteCarloResult_doc.i

%copyctor PERSALYS::FieldMonteCarloResult;

%include persalys/FieldMonteCarloResult.hxx
