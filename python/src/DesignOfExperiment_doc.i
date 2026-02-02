// DesignOfExperiment docstrings are defined in DataSample_doc.i and DataModel_doc.i

%feature("docstring") PERSALYS::DesignOfExperiment
"Create a Design of Experiments

The interface allows the user to load data samples and analyse them. They are considered as data models.

    DesignOfExperiment(*name, physicalModel, importedDataset, inputNames, outputNames*)

    DesignOfExperiment(*name, fileName, inputColumns, outputColumns, inputNames, outputNames*)

    DesignOfExperiment(*name, inSample, outSample*)

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
fileName : str
    Name of a data file (.txt ot .csv) to load
inputColumns : sequence of int
    Indices of columns of the input variables in file to consider
outputColumns : sequence of int (optional)
    Indices of columns of the output variables in file to consider (optional)
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
>>> model = persalys.DataModel('dataModel', fileName, [0, 2], [1], persalys.DataModel.MC, ['var1', 'var2'], ['var3'])"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DesignOfExperiment::setName
PERSALYS_DesignOfExperiment_setName_doc

%feature("docstring") PERSALYS::DesignOfExperiment::hasPhysicalModel
PERSALYS_DesignOfExperiment_hasPhysicalModel_doc

%feature("docstring") PERSALYS::DesignOfExperiment::getPhysicalModel
PERSALYS_DesignOfExperiment_getPhysicalModel_doc

%feature("docstring") PERSALYS::DesignOfExperiment::setPhysicalModel
PERSALYS_DesignOfExperiment_setPhysicalModel_doc

%feature("docstring") PERSALYS::DesignOfExperiment::getInputSample
PERSALYS_DesignOfExperiment_getInputSample_doc

%feature("docstring") PERSALYS::DesignOfExperiment::getOutputSample
PERSALYS_DesignOfExperiment_getOutputSample_doc

%feature("docstring") PERSALYS::DesignOfExperiment::getSample
PERSALYS_DesignOfExperiment_getSample_doc

%feature("docstring") PERSALYS::DesignOfExperiment::getMarginalWithoutNaN
PERSALYS_DesignOfExperiment_getMarginalWithoutNaN_doc

%feature("docstring") PERSALYS::DesignOfExperiment::setInputSample
PERSALYS_DesignOfExperiment_setInputSample_doc

%feature("docstring") PERSALYS::DesignOfExperiment::setOutputSample
PERSALYS_DesignOfExperiment_setOutputSample_doc

%feature("docstring") PERSALYS::DesignOfExperiment::isValid
PERSALYS_DesignOfExperiment_isValid_doc

%feature("docstring") PERSALYS::DesignOfExperiment::getPythonScript
PERSALYS_DesignOfExperiment_getPythonScript_doc

%feature("docstring") PERSALYS::DesignOfExperiment::getEffectiveInputIndices
PERSALYS_DesignOfExperiment_getEffectiveInputIndices_doc

%feature("docstring") PERSALYS::DesignOfExperiment::setType
PERSALYS_DesignOfExperiment_setType_doc

%feature("docstring") PERSALYS::DesignOfExperiment::getType
PERSALYS_DesignOfExperiment_getType_doc