// SWIG file

%{
#include "otgui/DataModel.hxx"
%}

%include DataModel_doc.i

%include otgui/DataModel.hxx
namespace OTGUI {

%extend DataModel { DataModel(const DataModel & other) { return new OTGUI::DataModel(other); } 

std::string __repr__() {
  return "DataModel";
}

} }