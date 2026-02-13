%feature("docstring") PERSALYS::DataModel
"Create a data model from an imported dataset.

The interface allows the user to load data samples and analyse them. They are considered as data models.

    DataModel(*name, physicalModel*)

    DataModel(*name, importedDataset, inputNames, outputNames*)

    DataModel(*name, inSample, outSample*)

Parameters
----------
name : str
    Name
physicalModel : :py:class:`persalys.PhysicalModel`
    Physical model associated to the data model (optional)
importedDataset : :py:class:`persalys.ImportedDataset`
    Imported dataset associated to the data model (optional)
inputNames : sequence of str 
    Names of the input variables (optional)
outputNames : sequence of str
    Names of the output variables (optional)
inSample : :py:class:`openturns.Sample`
    Input sample (its description must be a list of input variable names)
outSample : :py:class:`openturns.Sample`
    Output sample (its description must be a list of output variable names)

Examples
--------
>>> import openturns as ot
>>> import persalys
>>> ot.RandomGenerator.SetSeed(0)
>>> fileName = 'sample.csv'
>>> sample = ot.Normal(3).getSample(30)
>>> sample.exportToCSVFile(fileName)
>>> importedDataset = persalys.ImportedDataset(fileName, [0, 2], [1])
>>> model = persalys.DataModel('dataModel', importedDataset, ['var1', 'var2'], ['var3'])
>>> model.setType(persalys.DataModel.MC)"

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_setName_doc
"Name setter.

Parameters
----------
name : str
    Name of the design of experiment"
%enddef

%feature("docstring") PERSALYS::DesignOfExperiment::setName
PERSALYS_DesignOfExperiment_setName_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_hasPhysicalModel_doc
"Whether it contains a physical model.

Returns
-------
hasPhysicalModel : bool
    Whether it contains a physical model"
%enddef

%feature("docstring") PERSALYS::DataModel::hasPhysicalModel
PERSALYS_DesignOfExperiment_hasPhysicalModel_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getPhysicalModel_doc
"Physical model accessor.

Returns
-------
model : :class:`~persalys.PhysicalModel`
    Physical model"
%enddef

%feature("docstring") PERSALYS::DataModel::getPhysicalModel
PERSALYS_DesignOfExperiment_getPhysicalModel_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_setPhysicalModel_doc
"Physical model setter.

Parameters
----------
model : :class:`~persalys.PhysicalModel`
    Physical model to set"
%enddef

%feature("docstring") PERSALYS::DataModel::setPhysicalModel
PERSALYS_DesignOfExperiment_setPhysicalModel_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_removePhysicalModel_doc
"Remove the physical model from the design of experiment."
%enddef

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataModel::initialize
"Empty the input and output samples."

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getEffectiveInputIndices_doc
"Effective indices accessor.

Indices of non-const variables in the design.

Returns
-------
indices : :py:class:`openturns.Indices`
    Input sample and output sample"
%enddef

%feature("docstring") PERSALYS::DataModel::getEffectiveInputIndices
PERSALYS_DesignOfExperiment_getEffectiveInputIndices_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_setType_doc
"Type setter.

Parameters
----------
type : int
    possible values are 
        persalys.DataModel.GENERIC,
        persalys.DataModel.MC,
        persalys.DataModel.QMC,
        persalys.DataModel.LHS,
        persalys.DataModel.GRID,
        persalys.DataModel.MORRIS"
%enddef

%feature("docstring") PERSALYS::DataModel::setType
PERSALYS_DesignOfExperiment_setType_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getType_doc
"Type getter.

Returns
-------
type : int
    possible values are 
        persalys.DataModel.GENERIC,
        persalys.DataModel.MC,
        persalys.DataModel.QMC,
        persalys.DataModel.LHS,
        persalys.DataModel.GRID,
        persalys.DataModel.MORRIS"
%enddef

%feature("docstring") PERSALYS::DataModel::getType
PERSALYS_DesignOfExperiment_getType_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getInputNames_doc
"Names of the input variables accessor.

Returns
-------
names : :py:class:`openturns.Description`
   Names of the input variables"
%enddef

%feature("docstring") PERSALYS::DataModel::getInputNames
PERSALYS_DesignOfExperiment_getInputNames_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getOutputNames_doc
"Names of the output variables accessor.

Returns
-------
names : :py:class:`openturns.Description`
   Names of the output variables"
%enddef

%feature("docstring") PERSALYS::DataModel::getOutputNames
PERSALYS_DesignOfExperiment_getOutputNames_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_setColumns_doc
"Columns and names of variables accessor.

Parameters
----------
inputColumns : sequence of int
    Columns of input variables
inNames : sequence of str
   Names of input variables
outputColumns : sequence of int
    Columns of output variables
outNames : sequence of str
   Names of output variables"
%enddef

%feature("docstring") PERSALYS::DataModel::setColumns
PERSALYS_DesignOfExperiment_setColumns_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_setSample_doc
"Changes the sample contained inside the data model.

Parameters
----------
sample : :py:class:`openturns.Sample`
    Sample to set inside the data model"
%enddef

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getSampleFromFile_doc
"Returns the sample contained inside the imported dataset.

Returns
-------
sample : :py:class:`openturns.Sample`
   Sample contained inside the imported dataset"
%enddef

%feature("docstring") PERSALYS::DataModel::getSampleFromFile
PERSALYS_DesignOfExperiment_getSampleFromFile_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getInputColumns_doc
"Indices of the input variables accessor.

Returns
-------
columns : :py:class:`openturns.Indices`
   Indices of the input variables"
%enddef

%feature("docstring") PERSALYS::DataModel::getInputColumns
PERSALYS_DesignOfExperiment_getInputColumns_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getOutputColumns_doc
"Indices of the output variables accessor.

Returns
-------
columns : :py:class:`openturns.Indices`
   Indices of the output variables"
%enddef

%feature("docstring") PERSALYS::DataModel::getInputColumns
PERSALYS_DesignOfExperiment_getInputColumns_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getFileName_doc
"File name of the imported dataset accessor.

Returns
-------
fileName : str
   File name of the imported dataset"
%enddef

%feature("docstring") PERSALYS::DataModel::getFileName
PERSALYS_DesignOfExperiment_getFileName_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_setFileName_doc
"Sets the file name of the imported dataset.

Parameters
----------
fileName : str
    File name of the imported dataset"
%enddef

%feature("docstring") PERSALYS::DataModel::setFileName
PERSALYS_DesignOfExperiment_setFileName_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getPythonScript_doc
"Python script accessor.

Returns
-------
script : str
    Python script to rebuild the design of experiments"
%enddef

%feature("docstring") PERSALYS::DataModel::getPythonScript
PERSALYS_DesignOfExperiment_getPythonScript_doc