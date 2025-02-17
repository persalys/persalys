// SWIG file

%{
#include "persalys/DataFieldModel.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::DataFieldModel >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  PERSALYS::DataFieldModel
  convert< _PyObject_, PERSALYS::DataFieldModel >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::DataFieldModel *"), 0))) {
      PERSALYS::DataFieldModel * p_it = reinterpret_cast< PERSALYS::DataFieldModel * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a DataFieldModel";
    }
    return PERSALYS::DataFieldModel();
  }
}
%}

%ignore PERSALYS::DataFieldModel::setImplementationAsPersistentObject;

%include DataFieldModel_doc.i

PERSALYSTypedInterfaceObjectHelper(DataFieldModel)
PERSALYSTypedCollectionInterfaceObjectHelper(DataFieldModel)

%include persalys/DataFieldModel.hxx
namespace PERSALYS {

%extend DataFieldModel { DataFieldModel(const DataFieldModel & other) { return new PERSALYS::DataFieldModel(other); }

std::string __repr__() {
  return "DataFieldModel";
}

} }
