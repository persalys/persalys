#! /usr/bin/env python

from __future__ import print_function
import persalys
import openturns as ot
import openturns.testing
import math

sample = ot.Sample(0,3)
sample.add([4,2,4])
sample.add([2,math.nan,4])
sample.add([2,3,7])
sample.add([8,7,3])
sample.add([math.inf,math.nan,7])



clean = persalys.DataCleaningTools(sample)
openturns.testing.assert_almost_equal(clean.getMean(), [4,4,5])
openturns.testing.assert_almost_equal(clean.getMedian(), [3,3,4])

#remove points containing Nans/Infs
clean.removeAllNans()
openturns.testing.assert_almost_equal(
    clean.getSample(), [[4,2,4],[2,3,7],[8,7,3]])

clean = persalys.DataCleaningTools(sample)
#replace Nans/Infs with zeros
clean.replaceAllNans([0,0,0])
openturns.testing.assert_almost_equal(
    clean.getSample(), [[4,2,4],[2,0,4],[2,3,7],[8,7,3],[0,0,7]])

clean = persalys.DataCleaningTools(sample)
#replace Nans/Infs with medians
clean.replaceAllNans(clean.getMedian())
openturns.testing.assert_almost_equal(
    clean.getSample(), [[4,2,4],[2,3,4],[2,3,7],[8,7,3],[3,3,7]])

clean.computeGeometricMAD()
openturns.testing.assert_almost_equal(clean.getMAD(), [1,0,1])
openturns.testing.assert_almost_equal(clean.getGeometricMAD(), 1.41421)

clean = persalys.DataCleaningTools(sample)
#replace Nans/Infs with means
clean.replaceAllNans(clean.getMean())
openturns.testing.assert_almost_equal(
    clean.getSample(), [[4,2,4],[2,4,4],[2,3,7],[8,7,3],[4,4,7]])

#compute MAD
clean.computeGeometricMAD()
openturns.testing.assert_almost_equal(clean.getMAD(), [1,1,1])
openturns.testing.assert_almost_equal(clean.getGeometricMAD(), 1.73205)

clean = persalys.DataCleaningTools(sample)
#replace Nans by column
clean.replaceNansByColumn(0,0)
clean.replaceNansByColumn(1,0)
openturns.testing.assert_almost_equal(
    clean.getSample(), [[4,2,4],[2,0,4],[2,3,7],[8,7,3],[0,0,7]])

clean = persalys.DataCleaningTools(sample)
#remove point / replace values column by column
clean.removeNansByColumn(0)
openturns.testing.assert_almost_equal(
    clean.getSample(), [[4,2,4],[2,math.nan,4],[2,3,7],[8,7,3]])
clean.replaceNansByColumn(1, -2)
openturns.testing.assert_almost_equal(
    clean.getSample(), [[4,2,4],[2,-2,4][2,3,7],[8,7,3]])
