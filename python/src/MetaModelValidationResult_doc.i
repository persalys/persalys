%feature("docstring") PERSALYS::MetaModelValidationResult
"Create the base class for the results of a metamodel validation."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelValidationResult::getName
"Validation method name accessor.

Returns
-------
name : str
    Validation method name"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelValidationResult::getMetaModelOutputSample
"Output sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Output sample evaluated with the metamodel"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelValidationResult::getResiduals
R"RAW(Residuals accessor.

Returns
-------
error : :py:class:`openturns.Point`
    The errors are defined as follows for each output of the model:
    :math:`\displaystyle \frac{\sqrt{\sum_{i=1}^N (y_i - \hat{y_i})^2}}{N}`
    with :math:`y_i` the :math:`N` model's values and :math:`\hat{y_i}` the
    prediction.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MetaModelValidationResult::getQ2
R"RAW(Q2 accessor.

Returns
-------
q2 : :py:class:`openturns.Point`
    The predictive squared correlation coefficient Q2 is defined as follows for
    each output of the model:
    :math:`1 - \displaystyle \frac{\sum_{i=1}^N (y_i - \hat{y_i})^2}{(N-1) {\rm Var}\left[ \underline{Y} \right]}`
    with :math:`\underline{Y}` the vector of the :math:`N` model's values
    :math:`y_i` and :math:`\hat{y_i}` the prediction.)RAW"
