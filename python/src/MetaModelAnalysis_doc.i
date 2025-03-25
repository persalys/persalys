%feature("docstring") PERSALYS::MetaModelAnalysis
"Create a base class for the creation of meta models.

Notes
-----
Can only be used through its derived classes.
See :class:`~persalys.FunctionalChaosAnalysis`,
:class:`~persalys.KrigingAnalysis`"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::leaveOneOutValidation
"Whether a validation by leave-one-out is requested.

Returns
-------
validation : bool
    Whether a validation by leave-one-out is requested"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::setLeaveOneOutValidation
"Whether it is sparse.

Parameters
----------
validation : bool
    Whether a validation by leave-one-out is requested"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::kFoldValidation
"Whether a k-Fold cross-validation is requested.

Returns
-------
validation : bool
    Whether a k-Fold cross-validation is requested"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::setKFoldValidation
"Whether a k-Fold cross-validation is requested.

Parameters
----------
validation : bool
    Whether a k-Fold cross-validation is requested"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::analyticalValidation
"Whether an analytical validation is requested.

Returns
-------
validation : bool
    Whether an analytical validation is requested.
    This method corresponds to an approximation of the Leave-one-out method result."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::setAnalyticalValidation
"Whether an analytical validation is requested.

Parameters
----------
validation : bool
    Whether an analytical validation is requested.
    This method corresponds to an approximation of the Leave-one-out method result."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::testSampleValidation
"Whether a validation with a test sample is requested.

Returns
-------
validation : bool
    Whether a validation with a test sample is requested.
    The data sample is dividing into two sub-samples: a training sample
    (default: 80% of the sample points) and a test sample (default:
    20% of the sample points). A new metamodel is built
    with the training sample and is validated with the test sample.
    The points are randomly picked in the data sample (by default the seed is 1)."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::setTestSampleValidation
"Whether a validation with a test sample is requested.

Parameters
----------
validation : bool
    Whether a validation with a test sample is requested.
    The data sample is dividing into two sub-samples: a training sample
    (default: 80% of the sample points) and a test sample (default:
    20% of the sample points). A new metamodel is built
    with the training sample and is validated with the test sample.
    The points are randomly picked in the data sample (by default the seed is 1)."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::getKFoldValidationNumberOfFolds
"Number of folds accessor.

Returns
-------
folds : int
    Number of folds. By default it is 3."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::setKFoldValidationNumberOfFolds
"Number of folds accessor.

Parameters
----------
folds : int
    Number of folds. By default it is 3."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::getKFoldValidationSeed
"Seed accessor.

Returns
-------
seed : int
    Seed value for k-Fold cross-validation"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::setKFoldValidationSeed
"Seed accessor.

Parameters
----------
seed : int
    Seed value for k-Fold cross-validation"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::getTestSampleValidationSeed
"Seed accessor.

Returns
-------
seed : int
    Seed value for the validation with a test sample"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::setTestSampleValidationSeed
"Seed accessor.

Parameters
----------
seed : int
    Seed value for the validation with a test sample"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::getTestSampleValidationPercentageOfPoints
"Percentage of points accessor.

Returns
-------
percentage : int
    Percentage of points used to validate the metamodel. By default it is 20%."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::setTestSampleValidationPercentageOfPoints
"Percentage of points accessor.

Parameters
----------
percentage : int
    Percentage of points used to validate the metamodel. By default it is 20%."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::getEffectiveInputSample
"Effective input sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Sample of all the input variables if all of them are deterministic.
    Otherwise, sample of the stochastic input variables."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::getEffectiveOutputSample
"Effective output sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Sample of the interest output variables."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelAnalysis::getDistribution
"Input distribution accessor.

Returns
-------
distribution : :py:class:`openturns.JointDistribution`
    The distribution defined in the probabilistic model or a distribution composed
    of Uniform laws if there is no stochastic input variable."
