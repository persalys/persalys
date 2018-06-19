// SWIG file

%{
#include "otgui/MeshModel.hxx"

namespace OT {
template <>
struct traitsPythonType< OTGUI::MeshModel >
{
  typedef _PyObject_ Type;
};

template <>
  inline
  OTGUI::MeshModel
  convert< _PyObject_, OTGUI::MeshModel >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OTGUI::MeshModel *"), 0))) {
      OTGUI::MeshModel * p_it = reinterpret_cast< OTGUI::MeshModel * >( ptr );
      return *p_it;
    }
    else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not convertible to a MeshModel";
    }
    return OTGUI::MeshModel();
  }
}
%}

%include MeshModel_doc.i

OTGUITypedInterfaceObjectHelper(MeshModel)
OTGUITypedCollectionInterfaceObjectHelper(MeshModel)

%include otgui/MeshModel.hxx
namespace OTGUI {

%extend MeshModel { MeshModel(const MeshModel & other) { return new OTGUI::MeshModel(other); } 

} }
