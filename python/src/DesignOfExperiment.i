// SWIG file

%{
#include "otgui/DesignOfExperiment.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::DesignOfExperiment >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  OTGUI::DesignOfExperiment
  convert< _PyObject_, OTGUI::DesignOfExperiment >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::DesignOfExperiment *"), 0))) {
      OTGUI::DesignOfExperiment * p_it = reinterpret_cast< OTGUI::DesignOfExperiment * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a DesignOfExperiment";
    }
    return OTGUI::DesignOfExperiment();
  }
}
%}

%include DesignOfExperiment_doc.i

OTGUITypedInterfaceObjectHelper(DesignOfExperiment)
OTGUITypedCollectionInterfaceObjectHelper(DesignOfExperiment)

%include otgui/DesignOfExperiment.hxx
namespace OTGUI {

%extend DesignOfExperiment { DesignOfExperiment(const DesignOfExperiment & other) { return new OTGUI::DesignOfExperiment(other); } 

std::string __repr__() {
  return "DesignOfExperiment";
}

} }
