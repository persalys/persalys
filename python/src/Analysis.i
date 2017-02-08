// SWIG file Analysis.i

%{
#include "otgui/Analysis.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::Analysis >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  OTGUI::Analysis
  convert< _PyObject_, OTGUI::Analysis >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::Analysis *"), 0))) {
      OTGUI::Analysis * p_it = reinterpret_cast< OTGUI::Analysis * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a Analysis";
    }
    return OTGUI::Analysis();
  }
}
%}

%include Analysis_doc.i

OTGUITypedInterfaceObjectHelper(Analysis)
OTGUITypedCollectionInterfaceObjectHelper(Analysis)

%include otgui/Analysis.hxx
namespace OTGUI {

%extend Analysis { Analysis(const Analysis & other) { return new OTGUI::Analysis(other); } 

std::string __repr__() {
  return "Analysis";
}

} }
