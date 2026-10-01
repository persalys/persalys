#!/usr/bin/env python

import multiprocessing

from persalys import _script_evaluation

CODE = r'''
def _exec(x, y):
    return x * y, x + y
'''
SINGLE_OUTPUT_CODE = r'''
def _exec(x, y):
    return x + y
'''
FAILING_CODE = r'''
def _exec(x, y):
    if y < 0:
        raise ValueError("negative y: %g" % y)
    return x * y, x + y
'''
GUARDED_CODE = r'''
if __name__ == "__main__":
    raise RuntimeError("the model code must not run as __main__")

def _exec(x, y):
    return x * y, x + y
'''


def checkResults(results, okIndices, failedIndices, messages, points):
    """The outputs must be those of the points that succeeded, in their order."""
    assert len(failedIndices) == len(messages), (failedIndices, messages)
    assert len(results) == len(okIndices), (results, okIndices)
    assert okIndices == sorted(okIndices), okIndices
    assert failedIndices == sorted(failedIndices), failedIndices
    assert not (set(okIndices) & set(failedIndices)), (okIndices, failedIndices)
    for result, index in zip(results, okIndices):
        x, y = points[index]
        assert len(result) == 2, result
        assert abs(result[0] - x * y) < 1e-12, (result, x, y)
        assert abs(result[1] - (x + y)) < 1e-12, (result, x, y)


# a model of several outputs, evaluated in more points than there are workers
points = [(float(i), float(i) + 0.5) for i in range(37)]
results, okIndices, failedIndices, messages = _script_evaluation.evaluate(CODE, points, 7)
checkResults(results, okIndices, failedIndices, messages, points)
assert not failedIndices, failedIndices
assert okIndices == list(range(37)), okIndices
print("7 workers, 37 points:", len(results), "results")

# the workers must be reused between batches, not restarted: it is not enough
# to check that they are still running, a pool rebuilt for every batch would
# give the same number of them, so check that they are the same processes
pidsBefore = sorted(child.pid for child in multiprocessing.active_children())
assert len(pidsBefore) == 7, pidsBefore
_script_evaluation.evaluate(CODE, points, 7)
_script_evaluation.evaluate(CODE, points, 7)
pidsAfter = sorted(child.pid for child in multiprocessing.active_children())
assert pidsAfter == pidsBefore, (pidsBefore, pidsAfter)
print("workers still running after the batches:", len(pidsAfter))

# the code is sent to the workers, they must run the new one, which means they
# must be replaced since they would otherwise run the previous one
results, okIndices, failedIndices, messages = _script_evaluation.evaluate(CODE, points, 7)
assert not failedIndices, failedIndices
pidsBeforeCodeChange = sorted(
    child.pid for child in multiprocessing.active_children())
# the second and the fifth points below make the model fail
signedPoints = [(1.0, -1.0), (2.0, 3.0), (3.0, -4.0), (4.0, 5.0), (5.0, -6.0)]
results, okIndices, failedIndices, messages = \
    _script_evaluation.evaluate(FAILING_CODE, signedPoints, 7)
assert failedIndices == [0, 2, 4], failedIndices
assert okIndices == [1, 3], okIndices
assert len(messages) == 3, messages
assert "negative y" in messages[0], messages
pidsAfterCodeChange = sorted(
    child.pid for child in multiprocessing.active_children())
assert not (set(pidsBeforeCodeChange) & set(pidsAfterCodeChange)), \
    (pidsBeforeCodeChange, pidsAfterCodeChange)
print("changed code, failed points:", failedIndices)

# back to the first code, the failing workers must have been replaced
results, okIndices, failedIndices, messages = _script_evaluation.evaluate(CODE, points, 7)
checkResults(results, okIndices, failedIndices, messages, points)
assert not failedIndices, failedIndices
print("back to the first code:", len(results), "results")

# a model of a single output returns a value, not a sequence of values
points = [(float(i), float(i) + 0.5) for i in range(11)]
results, okIndices, failedIndices, messages = \
    _script_evaluation.evaluate(SINGLE_OUTPUT_CODE, points, 3)
assert not failedIndices, failedIndices
for result, index in zip(results, okIndices):
    x, y = points[index]
    assert abs(result[0] - (x + y)) < 1e-12, (result, x, y)
print("single output model:", len(results), "results")

# model code guarded by __name__ == '__main__' must not run in the workers
results, okIndices, failedIndices, messages = _script_evaluation.evaluate(GUARDED_CODE, points, 3)
checkResults(results, okIndices, failedIndices, messages, points)
assert not failedIndices, (failedIndices, messages)
print("guarded model code:", len(results), "results")

# fewer points than workers: there is nothing to chunk, one message per point
points = [(float(i), float(i) + 0.5) for i in range(2)]
results, okIndices, failedIndices, messages = _script_evaluation.evaluate(CODE, points, 7)
checkResults(results, okIndices, failedIndices, messages, points)
assert not failedIndices, failedIndices
assert okIndices == [0, 1], okIndices
print("fewer points than workers:", len(results), "results")

# no more processes than points are ever started: the workers are started
# eagerly when the pool is created, so idle ones would only cost fork time
_script_evaluation.shutdown()
points = [(float(i), float(i) + 0.5) for i in range(2)]
results, okIndices, failedIndices, messages = _script_evaluation.evaluate(CODE, points, 7)
checkResults(results, okIndices, failedIndices, messages, points)
assert not failedIndices, failedIndices
assert len(multiprocessing.active_children()) == 2, multiprocessing.active_children()
print("capped workers for a small batch:", len(multiprocessing.active_children()))

# a bigger batch grows the pool, up to the number of processes
points = [(float(i), float(i) + 0.5) for i in range(37)]
results, okIndices, failedIndices, messages = _script_evaluation.evaluate(CODE, points, 7)
checkResults(results, okIndices, failedIndices, messages, points)
assert not failedIndices, failedIndices
assert len(multiprocessing.active_children()) == 7, multiprocessing.active_children()
print("grown workers for a bigger batch:", len(multiprocessing.active_children()))

# a single point
points = [(3.0, 4.0)]
results, okIndices, failedIndices, messages = _script_evaluation.evaluate(CODE, points, 7)
checkResults(results, okIndices, failedIndices, messages, points)
assert not failedIndices, failedIndices
print("a single point:", len(results), "results")

# every point fails
results, okIndices, failedIndices, messages = \
    _script_evaluation.evaluate(FAILING_CODE, [(-1.0, -2.0)], 7)
assert not results, results
assert not okIndices, okIndices
assert failedIndices == [0], failedIndices
print("every point fails:", len(messages), "messages")

# the default number of workers
points = [(float(i), float(i) + 0.5) for i in range(5)]
results, okIndices, failedIndices, messages = _script_evaluation.evaluate(CODE, points, 0)
checkResults(results, okIndices, failedIndices, messages, points)
assert not failedIndices, failedIndices
print("default number of workers:", len(results), "results")

# no worker must be left running once the evaluation is over
_script_evaluation.shutdown()
assert not multiprocessing.active_children(), multiprocessing.active_children()
print("no worker left after shutdown")

print("OK")
