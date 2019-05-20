// SWIG file

%{
#include "persalys/MeshModel.hxx"

namespace OT {
template <>
struct traitsPythonType< PERSALYS::MeshModel >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  PERSALYS::MeshModel
  convert< _PyObject_, PERSALYS::MeshModel >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::MeshModel *"), 0))) {
      PERSALYS::MeshModel * p_it = reinterpret_cast< PERSALYS::MeshModel * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a MeshModel";
    }
    return PERSALYS::MeshModel();
  }
}
%}

%include MeshModel_doc.i

PERSALYSTypedInterfaceObjectHelper(MeshModel)
PERSALYSTypedCollectionInterfaceObjectHelper(MeshModel)

%include persalys/MeshModel.hxx
namespace PERSALYS {

%extend MeshModel { MeshModel(const MeshModel & other) { return new PERSALYS::MeshModel(other); } 

} }
