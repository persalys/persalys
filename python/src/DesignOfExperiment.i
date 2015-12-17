// SWIG file

%{
#include "otgui/DesignOfExperiment.hxx"
%}

OTGUITypedInterfaceObjectHelper(DesignOfExperiment)

%include otgui/DesignOfExperiment.hxx
namespace OTGUI {

%extend DesignOfExperiment { DesignOfExperiment(const DesignOfExperiment & other) { return new OTGUI::DesignOfExperiment(other); } 

std::string __repr__() {
  return "DesignOfExperiment";
}

} }
