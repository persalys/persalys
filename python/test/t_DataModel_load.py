import openturns as ot
import persalys
import openturns.testing as ott

myStudy = persalys.Study("myStudy")

importedDataset = persalys.ImportedDataset("Housing-prices-Boston.csv", list(range(13)), [13])
myDataModel = persalys.DataModel("myDataModel", importedDataset)
myStudy.add(myDataModel)

loadedStudy = persalys.Study.Open("DataModel_load.xml")
loadedDataModel = loadedStudy.getDataModelByName("HousingPricesBoston")

ott.assert_almost_equal(myDataModel.getSample(), loadedDataModel.getSample())
assert loadedDataModel.getType() == persalys.DataModel.GENERIC

model = loadedStudy.getPhysicalModels()[0]
observations = loadedStudy.getDataModelByName("observations_0")

assert observations.getPhysicalModel() == model
ott.assert_almost_equal(observations.getSample()[0], ot.Point([2.59753113216, 7.79259339647]))

myStudy.save("test_DataModel.xml")
reloadedStudy = persalys.Study.Open("test_DataModel.xml")

reloadedDataModel = reloadedStudy.getDataModelByName("myDataModel")
ott.assert_almost_equal(myDataModel.getSample(), reloadedDataModel.getSample())

script = myStudy.getPythonScript()
exec(script)