%feature("docstring") PERSALYS::FileMemoizeFunction
"Function proxy with a CSV file cache.

When this function is evaluated, it calls the
:class:`~openturns.Function` passed as argument, and caches
values with CSV files.

Parameters
----------
function : :class:`~openturns.Function`
    Delegate function
input_file : str
    Input CSV file name
output_file : str
    Output CSV file name

See also
--------
Function

Examples
--------
>>> import openturns as ot
>>> import persalys as psl
>>> f = ot.SymbolicFunction('x', 'x^2')
>>> inputSample = ot.Sample([[1], [2], [3], [4]])
>>> f = psl.FileMemoizeFunction(f, 'input.csv', 'output.csv')
>>> outputSample = f(inputSample)"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::enableCache
"Enable the cache mechanism."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::disableCache
"Disable the cache mechanism."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::clearCache
"Empty the content of the cache."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::isCacheEnabled
"Test whether the cache mechanism is enabled or not.

Returns
-------
isCacheEnabled : bool
    Flag telling whether the cache mechanism is enabled.
    It is disabled by default."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::getCacheHits
"Accessor to the number of computations saved thanks to the cache mechanism.

Returns
-------
cacheHits : int
    Integer that counts the number of computations saved thanks to the cache
    mechanism."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::getCacheInput
"Accessor to all the input numerical points stored in the cache mechanism.

Returns
-------
cacheInput : :class:`~openturns.Sample`
    All the input numerical points stored in the cache mechanism."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::getCacheOutput
"Accessor to all the output numerical points stored in the cache mechanism.

Returns
-------
cacheInput : :class:`~openturns.Sample`
    All the output numerical points stored in the cache mechanism."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::addCacheContent
"Add input numerical points and associated output to the cache.

Parameters
----------
input_sample : 2-d sequence of float
    Input numerical points to be added to the cache.
output_sample : 2-d sequence of float
    Output numerical points associated with the input_sample to be added to the
    cache."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::getCacheInputFile
"Cache input file accessor.

Returns
-------
input_file : str
    Cache input file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::getCacheOutputFile
"Cache output file accessor.

Returns
-------
output_file : str
    Cache output file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FileMemoizeFunction::setCacheFiles
"Cache files accessor.

Parameters
----------
input_file : str
    Cache input file name.
output_file : str
    Cache output file name."
