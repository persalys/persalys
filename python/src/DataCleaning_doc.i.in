%feature("docstring") PERSALYS::DataCleaning
"DataModel sample manipulation. Allows one to remove/replace irrelevant/erroneous values on-the-fly.

Parameters
----------
sample : openturns.Sample
   Sample instance that requires cleaning. Original sample is copied and will not be modified. Use sample accessor for the modified sample version.

Examples
--------
>>> import math
>>> import openturns as ot
>>> import persalys
>>> sample = ot.Sample(0,3)
>>> sample.add([4,2,4])
>>> sample.add([2,math.nan,4])
>>> sample.add([2,3,7])
>>> cleaner = persalys.DataCleaning(sample)
>>> cleaner.removeAllNans()
>>> cleaned_sample = cleaner.getSample()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::removeAllNans
"Removes Nans/Infs in sample"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::replaceAllNans
"Replaces Nans/Infs in sample point by point values

Parameters
----------
point : openturns.Point
   Replacement values"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::removeNansByColumn
"Removes Nans/Infs in sample column

Parameters
----------
col : int
   Column index to clean"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::replaceNansByColumn
"Replaces Nans/Infs in sample column by value

Parameters
----------
col : int
   Column index to clean
val : openturns.Scalar
   Replacement value"
// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::computeMAD
"Computes sample median absolute deviation"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::computeGeometricMAD
"Computes sample geometric median absolute deviation"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::analyseSample
"Column by column sample analysis.
Allows marginals mean/median computation by ignoring Nans/Infs.
Evaluates number of Nans/Infs for each marginal"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::getSample
"Sample accessor

Returns
-------
sample : :py:class:`openturns.Sample`
   Sample associated to the DataCleaning API. Constructed from a copy of the original sample and edited on the fly."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::getMedian
"Median accessor

Returns
-------
median : :py:class:`openturns.Point`"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::getMean
"Mean accessor

Returns
-------
mean : :py:class:`openturns.Point`"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::getMAD
"MAD accessor
Returns
-------
MAD : :py:class:`openturns.Point`"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::getGeometricMAD
"Geom. MAD accessor
Returns
-------
geomMad : :py:class:`openturns.Scalar`"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataCleaning::getNanNumbers
"Returns number of Nans/Infs in each sample column
Returns
-------
nNans : :py:class:`openturns.Point`"
