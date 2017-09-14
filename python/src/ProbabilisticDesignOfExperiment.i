// SWIG file

%{
#include "otgui/ProbabilisticDesignOfExperiment.hxx"
%}

%ignore OTGUI::ProbabilisticDesignOfExperiment::setDesignOfExperiment;

%include ProbabilisticDesignOfExperiment_doc.i

%include otgui/ProbabilisticDesignOfExperiment.hxx
namespace OTGUI {

%extend ProbabilisticDesignOfExperiment { ProbabilisticDesignOfExperiment(const ProbabilisticDesignOfExperiment & other) { return new OTGUI::ProbabilisticDesignOfExperiment(other); } 

} }
