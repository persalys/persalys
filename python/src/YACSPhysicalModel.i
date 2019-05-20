// SWIG file
namespace PERSALYS {

%extend YACSPhysicalModel {
YACSPhysicalModel(const YACSPhysicalModel & other)
{ return new PERSALYS::YACSPhysicalModel(other); }

PyObject* jobParameters()
{ return py2cpp::toPy($self->jobParameters());}

} }

%ignore jobParameters;
%{
#include "persalys/YACSPhysicalModel.hxx"
%}

%include YACSPhysicalModel_doc.i

%include persalys/YACSPhysicalModel.hxx
