// SWIG file Analysis.i

%{
#include "persalys/Analysis.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::Analysis >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  PERSALYS::Analysis
  convert< _PyObject_, PERSALYS::Analysis >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::Analysis *"), 0))) {
      PERSALYS::Analysis * p_it = reinterpret_cast< PERSALYS::Analysis * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Analysis";
    }
    return PERSALYS::Analysis();
  }
}
%}

%ignore PERSALYS::Analysis::getParameters;

%include Analysis_doc.i

PERSALYSTypedInterfaceObjectHelper(Analysis)
PERSALYSTypedCollectionInterfaceObjectHelper(Analysis)

%include persalys/Analysis.hxx
namespace PERSALYS {

%extend Analysis { Analysis(const Analysis & other) { return new PERSALYS::Analysis(other); } 

std::string __repr__() {
  return "Analysis";
}

} }
