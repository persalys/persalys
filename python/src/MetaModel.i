// SWIG file

%{
#include "persalys/MetaModel.hxx"
%}

%include MetaModel_doc.i

%include persalys/MetaModel.hxx
namespace PERSALYS {

%extend MetaModel { MetaModel(const MetaModel & other) { return new PERSALYS::MetaModel(other); } 

} }
