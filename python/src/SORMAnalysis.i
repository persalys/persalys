// SWIG file

%{
#include "persalys/SORMAnalysis.hxx"
%}

%include SORMAnalysis_doc.i

%include persalys/SORMAnalysis.hxx
namespace PERSALYS {

%extend SORMAnalysis { SORMAnalysis(const SORMAnalysis & other) { return new PERSALYS::SORMAnalysis(other); } 

} }
