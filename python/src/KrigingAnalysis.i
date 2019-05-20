// SWIG file

%{
#include "persalys/KrigingAnalysis.hxx"
%}

%include KrigingAnalysis_doc.i

%include persalys/KrigingAnalysis.hxx
namespace PERSALYS {

%extend KrigingAnalysis { KrigingAnalysis(const KrigingAnalysis & other) { return new PERSALYS::KrigingAnalysis(other); } 

} }
