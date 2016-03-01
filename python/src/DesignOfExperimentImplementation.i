// SWIG file

%{
#include "otgui/DesignOfExperimentImplementation.hxx"
%}

%include DesignOfExperimentImplementation_doc.i

%ignore OTGUI::DesignOfExperiment::updateParameters;

%include otgui/DesignOfExperimentImplementation.hxx
namespace OTGUI {

%extend DesignOfExperimentImplementation { DesignOfExperimentImplementation(const DesignOfExperimentImplementation & other) { return new OTGUI::DesignOfExperimentImplementation(other); } 

std::string __repr__() {
  return "DesignOfExperimentImplementation";
}

} }
