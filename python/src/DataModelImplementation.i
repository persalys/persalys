// SWIG file DataModelImplementation.i

%{
#include "otgui/DataModelImplementation.hxx"
%}

%include DataModelImplementation_doc.i

%include otgui/DataModelImplementation.hxx
namespace OTGUI {

%extend DataModelImplementation { DataModelImplementation(const DataModelImplementation & other) { return new OTGUI::DataModelImplementation(other); } 

std::string __repr__() {
  return "DataModelImplementation";
}

} }
