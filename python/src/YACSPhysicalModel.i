// SWIG file
namespace OTGUI {

%extend YACSPhysicalModel {
YACSPhysicalModel(const YACSPhysicalModel & other)
{ return new OTGUI::YACSPhysicalModel(other); }

PyObject* jobParameters()
{ return py2cpp::toPy($self->jobParameters());}

} }

%ignore jobParameters;
%{
#include "otgui/YACSPhysicalModel.hxx"
%}

%include YACSPhysicalModel_doc.i

%include otgui/YACSPhysicalModel.hxx
