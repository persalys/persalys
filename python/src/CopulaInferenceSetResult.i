// SWIG file

%{
#include "otgui/CopulaInferenceSetResult.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::CopulaInferenceSetResult >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, OTGUI::CopulaInferenceSetResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::CopulaInferenceSetResult *"), 0 ))) {
      OTGUI::CopulaInferenceSetResult * p_it = reinterpret_cast< OTGUI::CopulaInferenceSetResult * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  OTGUI::CopulaInferenceSetResult
  convert< _PyObject_, OTGUI::CopulaInferenceSetResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::CopulaInferenceSetResult *"), 0))) {
      OTGUI::CopulaInferenceSetResult * p_it = reinterpret_cast< OTGUI::CopulaInferenceSetResult * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a CopulaInferenceSetResult";
    }
    return OTGUI::CopulaInferenceSetResult();
  }

}
%}

%include CopulaInferenceSetResult_doc.i

%include otgui/CopulaInferenceSetResult.hxx

%template(CopulaInferenceSetResultCollection) OT::Collection<OTGUI::CopulaInferenceSetResult>;

namespace OTGUI {

%extend CopulaInferenceSetResult { CopulaInferenceSetResult(const CopulaInferenceSetResult & other) { return new OTGUI::CopulaInferenceSetResult(other); } 

} }