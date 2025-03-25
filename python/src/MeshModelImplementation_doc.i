%define PERSALYS_MeshModel_doc
"Create a mesh model base class.

Notes
-----
Can only be used through its derived classes.
See :class:`~persalys.GridMeshModel`, :class:`~persalys.ImportedMeshModel`"
%enddef
%feature("docstring") PERSALYS::MeshModelImplementation
PERSALYS_MeshModel_doc

// ---------------------------------------------------------------------

%define PERSALYS_MeshModel_getMesh_doc
"Mesh accessor.

Returns
-------
mesh : :py:class:`openturns.Mesh`
    Mesh"
%enddef
%feature("docstring") PERSALYS::MeshModelImplementation::getMesh
PERSALYS_MeshModel_getMesh_doc

// ---------------------------------------------------------------------

%define PERSALYS_MeshModel_getBounds_doc
"Get bounds of the mesh nodes accessor.

Returns
-------
bounds : :py:class:`openturns.Interval`
    Bounds of the mesh nodes along each direction"
%enddef
%feature("docstring") PERSALYS::MeshModelImplementation::getBounds
PERSALYS_MeshModel_getBounds_doc

// ---------------------------------------------------------------------

%define PERSALYS_MeshModel_getNumberOfNodes_doc
"Number of nodes accessor.

Returns
-------
number : :py:class:`openturns.Indices`
    Number of nodes along each direction"
%enddef
%feature("docstring") PERSALYS::MeshModelImplementation::getNumberOfNodes
PERSALYS_MeshModel_getNumberOfNodes_doc

// ---------------------------------------------------------------------

%define PERSALYS_MeshModel_getIndexParameters_doc
"Index parameters accessor.

Returns
-------
parameters : sequence of :class:`persalys.Variable`
    Index parameters"
%enddef
%feature("docstring") PERSALYS::MeshModelImplementation::getIndexParameters
PERSALYS_MeshModel_getIndexParameters_doc

// ---------------------------------------------------------------------

%define PERSALYS_MeshModel_setIndexParameters_doc
"Index parameters accessor.

Parameters
----------
parameters : sequence of :class:`persalys.Variable`
    Index parameters"
%enddef
%feature("docstring") PERSALYS::MeshModelImplementation::setIndexParameters
PERSALYS_MeshModel_setIndexParameters_doc

// ---------------------------------------------------------------------

%define PERSALYS_MeshModel_getPythonScript_doc
"Python script accessor.

Returns
-------
script : str
    Python script to replay the analysis"
%enddef
%feature("docstring") PERSALYS::MeshModelImplementation::getPythonScript
PERSALYS_MeshModel_getPythonScript_doc

