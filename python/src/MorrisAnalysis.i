// SWIG file

%{
#include "persalys/MorrisAnalysis.hxx"
%}

%include MorrisAnalysis_doc.i

%ignore PERSALYS::MorrisAnalysis::updateParameters;

%include persalys/MorrisAnalysis.hxx
namespace PERSALYS {

%extend MorrisAnalysis { MorrisAnalysis(const MorrisAnalysis & other) { return new PERSALYS::MorrisAnalysis(other); } 

} }
