// SWIG file

%{
#include "persalys/Study.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::Study >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  PERSALYS::Study
  convert< _PyObject_, PERSALYS::Study >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::Study *"), 0))) {
      PERSALYS::Study * p_it = reinterpret_cast< PERSALYS::Study * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Study";
    }
    return PERSALYS::Study();
  }
}
%}

%include Study_doc.i

%ignore *::SetInstanceObserver;

%pythonappend PERSALYS::Study::add %{
   args[0].thisown = 0
%}

%pythonappend PERSALYS::Study::remove %{
   args[0].thisown = 1
%}

PERSALYSTypedInterfaceObjectHelper(Study)
PERSALYSTypedCollectionInterfaceObjectHelper(Study)

%include persalys/Study.hxx
namespace PERSALYS {

%extend Study { Study(const Study & other) { return new PERSALYS::Study(other); }

std::string __repr__() {
  return "Study";
}

} }
