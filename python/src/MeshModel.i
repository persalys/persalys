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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("PERSALYS::MeshModel *"), SWIG_POINTER_NO_NULL))) {
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

%copyctor PERSALYS::MeshModel;

%include persalys/MeshModel.hxx
