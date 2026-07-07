"""
Tests for system reliability analysis features:
  - System LimitState (Union / Intersection), addFailureEvent, removeFailureEvent
  - Monte Carlo on system events with/without per-event individual probabilities
  - Importance Sampling on system events with multiple design points
  - FORM on system events (MultiFORMResult)
  - FORM-IS on system events
  - Python script round-trip
"""

import math
import openturns as ot
import openturns.testing as ott
import persalys

ot.RandomGenerator.SetSeed(0)

myStudy = persalys.Study("myStudy")

# -------------------------------------------------------
# Model: X0, X1 ~ N(0,1); Y0 = X0+X1; Y1 = X0-X1
# FORM design points: dp(Y0>2) = (1,1), dp(Y1>2) = (1,-1), beta = sqrt(2)
# -------------------------------------------------------
X0 = persalys.Input("X0", ot.Normal(0, 1))
X1 = persalys.Input("X1", ot.Normal(0, 1))
Y0 = persalys.Output("Y0")
Y1 = persalys.Output("Y1")
model = persalys.SymbolicPhysicalModel(
    "model", [X0, X1], [Y0, Y1], ["X0 + X1", "X0 - X1"]
)
myStudy.add(model)

# -------------------------------------------------------
# 1. LimitState API
# -------------------------------------------------------

# Single-event: not a system limit state
ls_single = persalys.LimitState("ls_single", model, "Y0", ot.Greater(), 2.0)
myStudy.add(ls_single)
assert not ls_single.isSystemLimitState()
assert ls_single.getNumberOfFailureEvents() == 1

# Union of two events
ls_union = persalys.LimitState(
    "ls_union",
    model,
    ["Y0", "Y1"],
    [ot.Greater(), ot.Greater()],
    persalys.LimitState.Union,
    [2.0, 2.0],
)
myStudy.add(ls_union)

assert ls_union.isSystemLimitState()
assert ls_union.getNumberOfFailureEvents() == 2
assert ls_union.getType() == persalys.LimitState.Union

# Intersection of two events
ls_inter = persalys.LimitState(
    "ls_inter",
    model,
    ["Y0", "Y1"],
    [ot.Greater(), ot.Greater()],
    persalys.LimitState.Intersection,
    [2.0, 2.0],
)
myStudy.add(ls_inter)

assert ls_inter.getType() == persalys.LimitState.Intersection

# addFailureEvent / removeFailureEvent
ls_dyn = persalys.LimitState("ls_dyn", model, "Y0", ot.Greater(), 1.0)
myStudy.add(ls_dyn)

assert ls_dyn.getNumberOfFailureEvents() == 1

ls_dyn.addFailureEvent("Y1", ot.Less(), 3.0)

assert ls_dyn.getNumberOfFailureEvents() == 2
assert ls_dyn.isSystemLimitState()

ls_dyn.removeFailureEvent(1)

assert ls_dyn.getNumberOfFailureEvents() == 1
assert not ls_dyn.isSystemLimitState()

# setType
ls_union.setType(persalys.LimitState.Intersection)

assert ls_union.getType() == persalys.LimitState.Intersection

ls_union.setType(persalys.LimitState.Union)

# -------------------------------------------------------
# 2. Monte Carlo on Union event – no per-event probabilities (default)
# -------------------------------------------------------
mc = persalys.MonteCarloReliabilityAnalysis("mc", ls_union)
mc.setMaximumCalls(10000)
mc.setBlockSize(100)
mc.setSeed(42)
myStudy.add(mc)
mc.run()

result_mc = mc.getResult()

assert not result_mc.hasPerEventSimulationResults()

# -------------------------------------------------------
# 3. Monte Carlo on Union event – with per-event probabilities
# -------------------------------------------------------
mc2 = persalys.MonteCarloReliabilityAnalysis("mc2", ls_union)
mc2.setMaximumCalls(10000)
mc2.setBlockSize(100)
mc2.setSeed(42)
mc2.setComputeIndividualEventProbabilities(True)
myStudy.add(mc2)
mc2.run()

result_mc2 = mc2.getResult()

assert result_mc2.hasPerEventSimulationResults()

pf_sys_mc = result_mc2.getSimulationResult().getProbabilityEstimate()
pf0_mc = result_mc2.getPerEventSimulationResult(0).getProbabilityEstimate()
pf1_mc = result_mc2.getPerEventSimulationResult(1).getProbabilityEstimate()

# Union bound: P(A∪B) >= P(A) and P(A∪B) >= P(B)
assert pf_sys_mc >= pf0_mc, f"Expected P(union)={pf_sys_mc} >= P(Y0>2)={pf0_mc}"
assert pf_sys_mc >= pf1_mc, f"Expected P(union)={pf_sys_mc} >= P(Y1>2)={pf1_mc}"

assert pf0_mc > 0.0
assert pf1_mc > 0.0

# -------------------------------------------------------
# 4. Importance Sampling on Union event – multiple design points
#    dp(Y0>2) = (1,1)  dp(Y1>2) = (1,-1)  (exact FORM design points)
# -------------------------------------------------------

# Without individual probabilities (default)
is_sys = persalys.ImportanceSamplingAnalysis("is_sys", ls_union)
is_sys.setMaximumCalls(10000)
is_sys.setBlockSize(100)
is_sys.setSeed(42)
is_sys.setStandardSpaceDesignPoints([[1.0, 1.0], [1.0, -1.0]])
myStudy.add(is_sys)
is_sys.run()

assert not is_sys.getResult().hasPerEventSimulationResults()

# With individual probabilities
is_sys2 = persalys.ImportanceSamplingAnalysis("is_sys2", ls_union)
is_sys2.setMaximumCalls(10000)
is_sys2.setBlockSize(100)
is_sys2.setSeed(42)
is_sys2.setStandardSpaceDesignPoints([[1.0, 1.0], [1.0, -1.0]])
is_sys2.setComputeIndividualEventProbabilities(True)
myStudy.add(is_sys2)
is_sys2.run()

result_is2 = is_sys2.getResult()

assert result_is2.hasPerEventSimulationResults()

pf_sys_is = result_is2.getSimulationResult().getProbabilityEstimate()
pf0_is = result_is2.getPerEventSimulationResult(0).getProbabilityEstimate()
pf1_is = result_is2.getPerEventSimulationResult(1).getProbabilityEstimate()

assert pf_sys_is >= pf0_is, f"Expected P(union)={pf_sys_is} >= P(Y0>2)={pf0_is}"
assert pf_sys_is >= pf1_is, f"Expected P(union)={pf_sys_is} >= P(Y1>2)={pf1_is}"

assert pf0_is > 0.0
assert pf1_is > 0.0

# -------------------------------------------------------
# 5. Monte Carlo on Intersection event – with per-event probabilities
# -------------------------------------------------------
mc_inter = persalys.MonteCarloReliabilityAnalysis("mc_inter", ls_inter)
mc_inter.setMaximumCalls(10000)
mc_inter.setBlockSize(100)
mc_inter.setSeed(42)
mc_inter.setComputeIndividualEventProbabilities(True)
myStudy.add(mc_inter)
mc_inter.run()

result_inter = mc_inter.getResult()

pf_inter = result_inter.getSimulationResult().getProbabilityEstimate()
pf0_inter = result_inter.getPerEventSimulationResult(0).getProbabilityEstimate()
pf1_inter = result_inter.getPerEventSimulationResult(1).getProbabilityEstimate()

# Intersection bound: P(A∩B) <= P(A) and P(A∩B) <= P(B)
assert pf_inter <= pf0_inter, f"Expected P(inter)={pf_inter} <= P(Y0>2)={pf0_inter}"
assert pf_inter <= pf1_inter, f"Expected P(inter)={pf_inter} <= P(Y1>2)={pf1_inter}"

# -------------------------------------------------------
# 6. FORM on Union event (MultiFORMResult)
# -------------------------------------------------------
form_sys = persalys.FORMAnalysis("form_sys", ls_union)
myStudy.add(form_sys)
form_sys.run()
form_collection = form_sys.getResult().getMultiFORMResult().getFORMResultCollection()
assert len(form_collection) == ls_union.getNumberOfFailureEvents()

# Beta for Y0>2 and Y1>2 is sqrt(2)
ott.assert_almost_equal(
    form_collection[0].getGeneralisedReliabilityIndex(), math.sqrt(2.0), 1e-4, 0.0
)
ott.assert_almost_equal(
    form_collection[1].getGeneralisedReliabilityIndex(), math.sqrt(2.0), 1e-4, 0.0
)

# -------------------------------------------------------
# 7. FORM-IS on Union event
# -------------------------------------------------------
formis_sys = persalys.FORMImportanceSamplingAnalysis("formis_sys", ls_union)
formis_sys.setMaximumCalls(5000)
formis_sys.setBlockSize(100)
formis_sys.setSeed(42)
myStudy.add(formis_sys)
formis_sys.run()

result_formis = formis_sys.getResult()

assert not result_formis.hasPerEventSimulationResults()
assert result_formis.getSimulationResult().getProbabilityEstimate() > 0.0

# -------------------------------------------------------
# 8. Python script round-trip
# -------------------------------------------------------
script = myStudy.getPythonScript()
exec(script)
