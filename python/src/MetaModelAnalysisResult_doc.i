%feature("docstring") PERSALYS::MetaModelAnalysisResult
"Create the base class for a metamodel analysis result."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysisResult::getMetaModel
"Metamodel accessor.

Returns
-------
metamodel : :class:`~persalys.MetaModel`
    Metamodel"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysisResult::getOutputSample
"Output sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Output sample"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysisResult::getMetaModelOutputSample
"Output sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Output sample evaluated with the metamodel"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysisResult::getLeaveOneOutValidation
"Leave-one-out validation result accessor.

Returns
-------
validation : :class:`persalys.MetaModelValidationResult`
    Leave-one-out validation result"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysisResult::getKFoldValidation
"K-Fold validation result accessor.

Returns
-------
validation : :class:`persalys.MetaModelValidationResult`
    K-Fold validation result"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysisResult::getTestSampleValidation
"Test sample validation result accessor.

Returns
-------
validation : :class:`persalys.MetaModelValidationResult`
    Test sample validation result"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysisResult::getAnalyticalValidation
"Analytical validation result accessor.

Returns
-------
validation : :class:`persalys.MetaModelValidationResult`
    Analytical validation result"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysisResult::getValidations
"Validation results accessor.

Returns
-------
validations : sequence of :class:`persalys.MetaModelValidationResult`
    All the validation results"

