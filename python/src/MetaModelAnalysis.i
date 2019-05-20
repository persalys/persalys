// SWIG file

%{
#include "persalys/MetaModelAnalysis.hxx"
%}

%include MetaModelAnalysis_doc.i

%include persalys/MetaModelAnalysis.hxx
namespace PERSALYS {

%extend MetaModelAnalysis { MetaModelAnalysis(const MetaModelAnalysis & other) { return new PERSALYS::MetaModelAnalysis(other); } 

} }
