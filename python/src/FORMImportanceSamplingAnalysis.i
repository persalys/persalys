// SWIG file FORMImportanceSamplingAnalysis.i

%{
#include "persalys/FORMImportanceSamplingAnalysis.hxx"
%}

%include FORMImportanceSamplingAnalysis_doc.i

%include persalys/FORMImportanceSamplingAnalysis.hxx
namespace PERSALYS {

%extend FORMImportanceSamplingAnalysis { FORMImportanceSamplingAnalysis(const FORMImportanceSamplingAnalysis & other) { return new PERSALYS::FORMImportanceSamplingAnalysis(other); } 

} }
