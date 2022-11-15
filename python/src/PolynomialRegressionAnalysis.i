// SWIG file

%{
#include "persalys/PolynomialRegressionAnalysis.hxx"
%}

%include PolynomialRegressionAnalysis_doc.i

%include persalys/PolynomialRegressionAnalysis.hxx
namespace PERSALYS {

%extend PolynomialRegressionAnalysis { PolynomialRegressionAnalysis(const PolynomialRegressionAnalysis & other) { return new PERSALYS::PolynomialRegressionAnalysis(other); } 

} }
