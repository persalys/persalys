// SWIG file

%{
#include "otgui/DataSample.hxx"
%}

%include DataSample_doc.i

%include otgui/DataSample.hxx
namespace OTGUI {

%extend DataSample { DataSample(const DataSample & other) { return new OTGUI::DataSample(other); } 

} }
