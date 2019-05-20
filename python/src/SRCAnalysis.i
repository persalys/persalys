// SWIG file

%{
#include "persalys/SRCAnalysis.hxx"
%}

%include SRCAnalysis_doc.i

%include persalys/SRCAnalysis.hxx
namespace PERSALYS {

%extend SRCAnalysis { SRCAnalysis(const SRCAnalysis & other) { return new PERSALYS::SRCAnalysis(other); } 

} }
