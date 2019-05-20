// SWIG file

%{
#include "persalys/SobolAnalysis.hxx"
%}

%include SobolAnalysis_doc.i

%include persalys/SobolAnalysis.hxx
namespace PERSALYS {

%extend SobolAnalysis { SobolAnalysis(const SobolAnalysis & other) { return new PERSALYS::SobolAnalysis(other); } 

} }
