// SWIG file

%{
#include "otgui/AnalyticalPhysicalModel.hxx"
%}

%include AnalyticalPhysicalModel_doc.i

%include otgui/AnalyticalPhysicalModel.hxx
namespace OTGUI {

%extend AnalyticalPhysicalModel { AnalyticalPhysicalModel(const AnalyticalPhysicalModel & other) { return new OTGUI::AnalyticalPhysicalModel(other); } 

} }
