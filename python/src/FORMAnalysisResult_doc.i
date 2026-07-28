%feature("docstring") PERSALYS::FORMAnalysisResult
"Class containing results of the FORM analysis.

Available constructors are:
- FORMAnalysisResult(formResult)
- FORMAnalysisResult(multiFormResult)

Parameters
----------
formResult : :py:class:`~openturns.FORMResult`
    FORM result
multiFormResult : :py:class:`~openturns.MultiFORMResult`
    MultiFORM result (for system limit states)"

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::FORMAnalysisResult::getEventProbability
"Event probability accessor.

Returns
-------
eventProbability : float
    Event probability"

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::FORMAnalysisResult::getGeneralisedReliabilityIndex
"Generalised reliability index accessor.

Returns
-------
generalisedReliabilityIndex : float
    Generalised reliability index"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FORMAnalysisResult::getFORMResult
"FORM result accessor.

Returns
-------
formResult : :py:class:`openturns.FORMResult`
    FORM result"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FORMAnalysisResult::getMultiFORMResult
"MultiFORM result accessor.

Returns
-------
multiFormResult : :py:class:`~openturns.MultiFORMResult`
    MultiFORM result (for system limit states)"
