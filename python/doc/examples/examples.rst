Examples
========

Example 1: Axial stressed beam
------------------------------

This example is a simple beam, restrained at one side and stressed by a traction load F at the other side.

1- Problem statement
````````````````````

a- Inputs
'''''''''

- Stochastics variables:

====== ======================== ==================
 Name  Description              Distribution
====== ======================== ==================
F      Traction load            Normal(75e3, 5e3)
sigma  Axial stress             LogNormal(300, 30)
====== ======================== ==================

- Deterministic independent variables:

======== ================================ =================
Variable Description                      Value
======== ================================ =================
D        diameter                         20.0
======== ================================ =================

b- Output
'''''''''

Primary energy savings :math:`G`

.. math::

    G = \sigma_e -\frac{F}{\pi \frac{D^2}{4} }


2- Central tendency analysis
````````````````````````````

2-1 Inputs
''''''''''

The central tendency analysis is performed with the Taylor Expansions method.

3-1 Results
'''''''''''
================ ================= ================== ===========
First order mean Second order mean Standard deviation Variance
================ ================= ================== ===========
0.0597305        0.0596787         0.0115612          0.000133661
================ ================= ================== ===========

3- Deterministic parametric analysis
````````````````````````````````````

3-1 Inputs
''''''''''

The minimum and the maximum values are computed automatically thanks to
the distribution of the variables. The minimum value is the quantile at the
probability of 0.05 and the maximum one is the quantile at the probability of
0.95. The number of used values per variable is by default 2.

======== ======= ======= ================
Variable Min     Max     Number of values
======== ======= ======= ================
Q        10035.5 10364.5        2
E        2975.33 3024.67        2
C        3901.31 4098.69        2
======== ======= ======= ================

3-2 Results
'''''''''''

3-2-1 Values
************

======= ======= ======= =========
Q       E       C       Ep 
======= ======= ======= =========
10035.5 2975.33 3901.31 0.0600365
10364.5 2975.33 3901.31 0.0292239
10035.5 3024.67 3901.31 0.0684295
10364.5 3024.67 3901.31 0.037892
10035.5 2975.33 4098.69 0.0812679
10364.5 2975.33 4098.69 0.0511513
10035.5 3024.67 4098.69 0.0892877
10364.5 3024.67 4098.69 0.0594339
======= ======= ======= =========

The points are generated according to the structure of a box design of experiments.
This deterministic design of experiments has 8 points obtained by regularly discretizing
the pavement :math:`[10035.5, 10364.5] \times [2975.33, 3024.67] \times [3901.31, 4098.69]`.

The minimum value of :math:`Ep` is 0.0292239 with X=[10364.5 2975.33 3901.31].
The maximum value of :math:`Ep` is 0.0892877 with X=[10035.5 3024.67 4098.69].
