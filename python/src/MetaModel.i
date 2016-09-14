// SWIG file

%{
#include "otgui/MetaModel.hxx"
%}

%include MetaModel_doc.i

%include otgui/MetaModel.hxx
namespace OTGUI {

%extend MetaModel { MetaModel(const MetaModel & other) { return new OTGUI::MetaModel(other); } 

std::string __repr__() {
  return "MetaModel";
}

} }
