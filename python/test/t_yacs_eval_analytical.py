#! /usr/bin/env python

from __future__ import print_function
import sys

# FIXME: submodule of salome ?
import evalyfx

XMLfilename = 'example.xml'
if len(sys.argv) > 1:
    XMLfilename = sys.argv[1]

session = evalyfx.YACSEvalSession()  # 1 session par instance d'IHM
session.launch()  # launch session
efx = evalyfx.YACSEvalYFX.BuildFromFile(XMLfilename)
inps = efx.getFreeInputPorts()  # on a les inputs : on peut:

# on peut savoir si un input a une valeur par defaut
inps[0].hasDefaultValueDefined()
inps[0].getDefaultValueDefined()  # on peut connaitre sa valeur par defaut
# on peut changer la valeur par default d'un input
inps[0].setDefaultValue(3.4)
inps[0].getName()  # on peut connaitre son nom
inps[0].getTypeOfData()  # on peut connaitre son type
#...
# on assigne a un input une serie de valeurs
p = [10.1, 10.2, 10.3]
inps[1].setSequenceOfValuesToEval(p)
# idem pour les outputs (noms,types...)
outps = efx.getFreeOutputPorts()

# on bloque les outputs que l'on souhaite
efx.lockPortsForEvaluation([outps[0], outps[1]])
# on demande les resources virtuelles sur lesquels on peut iterer
rss = efx.giveResources()
for i in range(len(rss)):
    elt1 = rss[i]
    for j in range(len(elt1)):
        elt2 = elt1[j]
        elt2.setWantedMachine('localhost')
        pass
    pass
#
a, b = efx.run(session)
res = efx.getResults()
w, z = res
print('@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ w=', w, 'z=', z)
