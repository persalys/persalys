// SWIG file

%{
#include "otgui/FieldMonteCarloResult.hxx"

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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("KarhunenLoeveResult *"), 0 ))) {
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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("KarhunenLoeveResult *"), 0))) {
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

%template(KarhunenLoeveResultCollection) OT::Collection<OT::KarhunenLoeveResult>;

%include otgui/FieldMonteCarloResult.hxx
namespace OTGUI {

%extend FieldMonteCarloResult { FieldMonteCarloResult(const FieldMonteCarloResult & other) { return new OTGUI::FieldMonteCarloResult(other); } 

} }
