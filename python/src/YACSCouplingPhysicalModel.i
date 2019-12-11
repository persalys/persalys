// SWIG file YACSCouplingPhysicalModel.i

%{
#include "persalys/YACSCouplingPhysicalModel.hxx"
%}

%include YACSCouplingPhysicalModel_doc.i

namespace PERSALYS {

%extend YACSCouplingPhysicalModel {
YACSCouplingPhysicalModel(const YACSCouplingPhysicalModel & other)
{ return new PERSALYS::YACSCouplingPhysicalModel(other); }

PyObject* jobParameters()
{ return py2cpp::toPy($self->jobParameters());}

} }

%ignore PERSALYS::YACSCouplingPhysicalModel::jobParameters;

%include persalys/YACSCouplingPhysicalModel.hxx
