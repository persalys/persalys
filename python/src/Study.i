// SWIG file

%{
#include "otgui/Study.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::Study >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  OTGUI::Study
  convert< _PyObject_, OTGUI::Study >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::Study *"), 0))) {
      OTGUI::Study * p_it = reinterpret_cast< OTGUI::Study * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Study";
    }
    return OTGUI::Study();
  }
}
%}

%include Study_doc.i

%ignore *::SetInstanceObserver;

%pythonappend OTGUI::Study::add %{
   args[0].thisown = 0
%}

%pythonappend OTGUI::Study::remove %{
   args[0].thisown = 1
%}

OTGUITypedInterfaceObjectHelper(Study)
OTGUITypedCollectionInterfaceObjectHelper(Study)

%include otgui/Study.hxx
namespace OTGUI {

%extend Study { Study(const Study & other) { return new OTGUI::Study(other); }

std::string __repr__() {
  return "Study";
}

} }
