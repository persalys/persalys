// SWIG file

%{
#include "otgui/GridDesignOfExperiment.hxx"
%}

%include GridDesignOfExperiment_doc.i

%ignore OTGUI::GridDesignOfExperiment::updateParameters;
%ignore OTGUI::GridDesignOfExperiment::setDesignOfExperiment;

%include otgui/GridDesignOfExperiment.hxx
namespace OTGUI {

%extend GridDesignOfExperiment { GridDesignOfExperiment(const GridDesignOfExperiment & other) { return new OTGUI::GridDesignOfExperiment(other); } 

} }
