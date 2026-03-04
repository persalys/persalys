%feature("docstring") PERSALYS::DataSample
"Class containing an input and an output sample.

The base class containing an input and an output sample"

// ----------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getInputSample_doc
"Input sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Input sample"
%enddef

%feature("docstring") PERSALYS::DataSample::getInputSample
PERSALYS_DesignOfExperiment_getInputSample_doc

// ----------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_setInputSample_doc
"Input sample accessor.

Parameters
----------
sample : :py:class:`openturns.Sample`
    Input sample"
%enddef

%feature("docstring") PERSALYS::DataSample::setInputSample
PERSALYS_DesignOfExperiment_setInputSample_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getOutputSample_doc
"Output sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Output sample"
%enddef

%feature("docstring") PERSALYS::DataSample::getOutputSample
PERSALYS_DesignOfExperiment_getOutputSample_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_setOutputSample_doc
"Output sample accessor.

Parameters
----------
sample : :py:class:`openturns.Sample`
    Output sample"
%enddef

%feature("docstring") PERSALYS::DataSample::setOutputSample
PERSALYS_DesignOfExperiment_setOutputSample_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getSample_doc
"Sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Input sample and output sample"
%enddef

%feature("docstring") PERSALYS::DataSample::getSample
PERSALYS_DesignOfExperiment_getSample_doc

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_getMarginalWithoutNaN_doc
"Returns a marginal sample with NaN values removed.

Parameters
----------
index : int
    Index of the wanted marginal

Returns
-------
sample : :py:class:`openturns.Sample`
    A subsample of the present sample with the requested marginal
    with NaN values removed."
%enddef

%feature("docstring") PERSALYS::DataSample::getMarginalWithoutNaN
PERSALYS_DesignOfExperiment_getMarginalWithoutNaN_doc


// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSample::getListXMax
"List of input values.

Returns
-------
list : SampleCollection
    List of input values"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSample::getListXMin
"List of input values.

Returns
-------
list : SampleCollection
    List of input values"

// ---------------------------------------------------------------------

%define PERSALYS_DesignOfExperiment_isValid_doc
"Whether the model is valid.

Returns
-------
isValid : bool
   Whether the model is valid"
%enddef

%feature("docstring") PERSALYS::DataSample::isValid
PERSALYS_DesignOfExperiment_isValid_doc
