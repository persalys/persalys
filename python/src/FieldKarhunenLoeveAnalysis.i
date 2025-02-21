// SWIG file

%{
#include "persalys/FieldKarhunenLoeveAnalysis.hxx"
%}

%include FieldKarhunenLoeveAnalysis_doc.i

%include persalys/FieldKarhunenLoeveAnalysis.hxx
namespace PERSALYS {

%extend FieldKarhunenLoeveAnalysis { FieldKarhunenLoeveAnalysis(const FieldKarhunenLoeveAnalysis & other) { return new PERSALYS::FieldKarhunenLoeveAnalysis(other); }

} }
