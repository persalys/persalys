%feature("docstring") PERSALYS::QuantileAnalysisResult
"Get the results of a quantile analysis."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysisResult::getPValue
"P-value accessor.

Parameters
----------

varName : str
    Name of the marginal

tailType : enum
    Tail type, either `persalys.QuantileAnalysisResult.Lower` or `persalys.QuantileAnalysisResult.Upper`.

Returns
-------
p-value : double
    P-value for the corresponding marginal and tail."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysisResult::getQuantiles
"Quantile accessor.

Parameters
----------

varName : str
    Name of the marginal

Returns
-------

quantile : :py:class:`openturns.SampleCollection`
    Each item in the collection contains the quantiles for a given tail type, each point in each item contains the quantile and its confidence interval for a given target probability."


// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysisResult::getType
"Type accessor.

Returns
-------

type : enum
    Analysis type, either `persalys.QuantileAnalysisResult.MonteCarlo` or `persalys.QuantileAnalysisResult.GeneralizedPareto`."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysisResult::getWilksValidity
"Validity accessor.

Parameters
----------

varName : str
    Name of the marginal

tailType : enum
    Tail type, either `persalys.QuantileAnalysisResult.Lower`, `persalys.QuantileAnalysisResult.Upper` or `persalys.QuantileAnalysisResult.Bilateral`.

Returns
-------
validity : :py:class:`openturns.Interval.BoolCollection`
    Indicates for each target probability if the quantile is valid or not."

%ignore PERSALYS::QuantileAnalysisResult::getSize;
