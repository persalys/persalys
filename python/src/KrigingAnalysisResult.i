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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("KrigingResult *"), 0 ))) {
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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("KrigingResult *"), 0))) {
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

%include persalys/KrigingAnalysisResult.hxx
namespace PERSALYS {

%extend KrigingAnalysisResult { KrigingAnalysisResult(const KrigingAnalysisResult & other) { return new PERSALYS::KrigingAnalysisResult(other); } 

} }
