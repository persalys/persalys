// SWIG file

%{
#include "persalys/GridDesignOfExperiment.hxx"
%}

%include GridDesignOfExperiment_doc.i

%ignore PERSALYS::GridDesignOfExperiment::updateParameters;
%ignore PERSALYS::GridDesignOfExperiment::setDesignOfExperiment;

%include persalys/GridDesignOfExperiment.hxx
namespace PERSALYS {

%extend GridDesignOfExperiment { GridDesignOfExperiment(const GridDesignOfExperiment & other) { return new PERSALYS::GridDesignOfExperiment(other); } 

} }
