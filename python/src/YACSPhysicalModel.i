// SWIG file YACSPhysicalModel.i

%{
#include "persalys/YACSPhysicalModel.hxx"
%}

%include YACSPhysicalModel_doc.i


namespace PERSALYS {

%extend YACSPhysicalModel {
YACSPhysicalModel(const YACSPhysicalModel & other)
{ return new PERSALYS::YACSPhysicalModel(other); }

PyObject* jobParameters()
{ return py2cpp::toPy($self->jobParameters());}

void setJobModel(PyObject* po)
{$self->setJobModel(py2cpp::PyPtr(po));}
} }

%ignore PERSALYS::YACSPhysicalModel::jobParameters;
%ignore PERSALYS::YACSPhysicalModel::setJobModel;

%include persalys/YACSPhysicalModel.hxx
