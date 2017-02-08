// SWIG file

%{
#include "otgui/FittingTestResult.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::FittingTestResult >
{
  typedef _PyObject_ Type;
};


template <>
  inline
  bool
  canConvert< _PyObject_, OTGUI::FittingTestResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::FittingTestResult *"), 0 ))) {
      OTGUI::FittingTestResult * p_it = reinterpret_cast< OTGUI::FittingTestResult * >( ptr );
      return p_it != NULL;
    }
    return false;
  }


template <>
  inline
  OTGUI::FittingTestResult
  convert< _PyObject_, OTGUI::FittingTestResult >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::FittingTestResult *"), 0))) {
      OTGUI::FittingTestResult * p_it = reinterpret_cast< OTGUI::FittingTestResult * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a FittingTestResult";
    }
    return OTGUI::FittingTestResult();
  }

}
%}

%include FittingTestResult_doc.i

%include otgui/FittingTestResult.hxx

%template(FittingTestResultCollection) OT::Collection<OTGUI::FittingTestResult>;

namespace OTGUI {

%extend FittingTestResult { FittingTestResult(const FittingTestResult & other) { return new OTGUI::FittingTestResult(other); } 

} }
