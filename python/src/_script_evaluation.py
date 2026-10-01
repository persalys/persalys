"""
    Persalys --- parallel evaluation of a model defined by a Python script
    ======================================================================

    Everything concerning the worker processes used to evaluate a Python model
    in parallel lives here rather than in C++.  The C++ layer only hands over
    the code and the points to evaluate, so this file is the single place to
    look at to understand how the parallel evaluation works, and it can be
    exercised without building anything.

    Two things make it worth its own module:

    * a pool of worker processes is started once and reused by every batch
      evaluating the same code, because starting processes is orders of
      magnitude more expensive than evaluating a single point;
    * the points of a batch are sent to the workers by chunks rather than one
      by one, because each chunk costs one inter-process message whatever the
      number of points it holds;
    * the pool holds no more workers than the points of the batch it was
      started for, because every worker is started eagerly when the pool is
      created and idle ones would only cost fork time and memory.

    The model code is sent to the workers as *data* (through the ``initargs``
    of the pool) instead of being made importable on disk, so no temporary
    file is needed and the workers need not import anything but this module.
"""

import multiprocessing
import os
import sys
from concurrent.futures import ProcessPoolExecutor
from math import ceil

# The pool of worker processes, and the code, the number of processes and the
# number of workers it was started for. A single slot is kept: there is no
# point in keeping the workers of a model the user has just edited alive.
_POOL = None
_POOL_CODE = None
_POOL_PROCESS_NUMBER = None
_POOL_WORKERS = None

# The model function inside a worker process, filled in by initialize().
_EXEC = None


def initialize(code):
    """Build the model inside a worker process.

    Called once per worker process, when it starts, not for each evaluation.

    The code is evaluated in a namespace of its own so that the objects it
    defines, including the imports it does, cannot collide with anything.
    ``__name__`` is deliberately not ``__main__``: the model is a library of
    functions, and the user code guarded by ``if __name__ == '__main__'`` is
    not meant to run on each evaluation.
    """
    global _EXEC
    namespace = {"__name__": "persalys_model"}
    exec(compile(code, "<persalys model code>", "exec"), namespace)
    _EXEC = namespace["_exec"]


def evaluate(code, points, process_number):
    """Evaluate the given points, in parallel if it is worth it.

    Parameters
    ----------
    code : str
        The code defining the model, that is the ``_exec`` function.
    points : list of tuple
        The points to evaluate.
    process_number : int
        The number of worker processes to use, 0 meaning all of them. Never
        more workers than points are started; the pool is grown, up to this
        number, when a bigger batch comes in.

    Returns
    -------
    results : list
        The outputs of the points that could be evaluated, in the order of
        the points that succeeded.
    ok_indices : list of int
        The indices, in ``points``, of those points.
    failed_indices : list of int
        The indices, in ``points``, of the points that failed, in increasing
        order.
    messages : list of str
        The reason of each failure, aligned with ``failed_indices``.
    """
    global _POOL, _POOL_CODE, _POOL_PROCESS_NUMBER, _POOL_WORKERS

    # no more workers than points: with the fork start method every worker is
    # started eagerly when the pool is created, so workers beyond the size of
    # the batch would be forked only to stay idle
    workers = process_number or os.cpu_count() or 1
    wanted = max(1, min(workers, len(points)))

    if (_POOL is not None
            and (code != _POOL_CODE
                 or process_number != _POOL_PROCESS_NUMBER
                 or wanted > _POOL_WORKERS)):
        # the workers would evaluate another model or are too few for the
        # batch, they are of no use anymore
        shutdown()

    if _POOL is None:
        _POOL = _start(code, wanted)
        _POOL_CODE = code
        _POOL_PROCESS_NUMBER = process_number
        _POOL_WORKERS = wanted

    return _evaluate(points, process_number)


def shutdown():
    """Stop the worker processes, if any are running.

    The next evaluation will start new ones.  Called on its own when the
    process exits.
    """
    global _POOL, _POOL_CODE, _POOL_PROCESS_NUMBER, _POOL_WORKERS
    if _POOL is not None:
        _POOL.shutdown()
    _POOL = None
    _POOL_CODE = None
    _POOL_PROCESS_NUMBER = None
    _POOL_WORKERS = None


def _start(code, process_number):
    """Start the pool of worker processes evaluating the given code."""
    # Forking is kept on purpose. A worker needs about fifteen milliseconds to
    # appear with fork against seventy with spawn, and it is the same once it
    # is up: what each worker spends the most time on is importing the model
    # code, half a second for either method. The difference is small, but the
    # other way round is not an option, since spawn makes every worker import
    # this script again and so requires the caller to guard it with
    # if __name__ == "__main__", which a user model or the graphical interface
    # has no reason to do.
    context = None
    if "fork" in multiprocessing.get_all_start_methods():
        context = multiprocessing.get_context("fork")

    if sys.platform == "win32":
        # without this each worker opens a console window
        multiprocessing.set_executable(
            os.path.join(sys.exec_prefix, "pythonw.exe"))

    return ProcessPoolExecutor(max_workers=process_number or None,
                               mp_context=context,
                               initializer=initialize,
                               initargs=(code,))


def _evaluate(points, process_number):
    """Spread the points over the workers of the pool and collect the outputs.

    The points are split in at most one chunk per worker: each chunk costs one
    inter-process message, which costs far more than evaluating a point, so
    the fewer messages the better.  When there are fewer points than workers
    there is no chunking to do, and one message per point it is.
    """
    number_of_points = len(points)
    workers = process_number or (os.cpu_count() or 1) or 1
    chunk_size = ceil(number_of_points / min(number_of_points, workers))
    chunks = [points[start:start + chunk_size]
              for start in range(0, number_of_points, chunk_size)]

    results = [None] * number_of_points
    failed_indices = []
    messages = []

    futures = [_POOL.submit(_evaluate_chunk, chunk) for chunk in chunks]
    offset = 0
    for chunk, future in zip(chunks, futures):
        try:
            chunk_results, chunk_failures = future.result()
        except Exception as exception:
            # the whole chunk is lost: the worker died, or it returned
            # something that cannot be sent back
            chunk_results = [None] * len(chunk)
            chunk_failures = [(index, "%s" % (exception,))
                              for index in range(len(chunk))]
        for index, result in enumerate(chunk_results):
            results[offset + index] = result
        for index, message in chunk_failures:
            failed_indices.append(offset + index)
            messages.append(message)
        offset += len(chunk)

    failed = set(failed_indices)
    ok_indices = [index for index in range(number_of_points)
                  if index not in failed]
    return ([results[index] for index in ok_indices],
            ok_indices,
            failed_indices,
            messages)


def _evaluate_chunk(chunk):
    """Evaluate the points of a chunk in a worker process.

    Returns the outputs, in the order of the chunk and with ``None`` where the
    evaluation failed, together with the reason of each failure.  Nothing is
    raised, so that a single failing point does not cost us the whole chunk.
    """
    results = []
    failures = []
    for index, args in enumerate(chunk):
        try:
            result = _EXEC(*args)
            # a model of a single output returns the value itself, not a
            # sequence of values
            if not isinstance(result, (tuple, list)):
                result = (result,)
            results.append(result)
        except Exception as exception:
            results.append(None)
            failures.append((index, "%s" % (exception,)))
    return results, failures
