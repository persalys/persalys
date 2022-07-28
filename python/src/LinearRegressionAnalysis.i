// SWIG file

%{
#include "persalys/LinearRegressionAnalysis.hxx"
%}

%include LinearRegressionAnalysis_doc.i

%include persalys/LinearRegressionAnalysis.hxx
namespace PERSALYS {

%extend LinearRegressionAnalysis { LinearRegressionAnalysis(const LinearRegressionAnalysis & other) { return new PERSALYS::LinearRegressionAnalysis(other); } 

} }
