#include "StudyTreeViewModel.hxx"

#include "OTStudy.hxx"

namespace OTGUI {

StudyTreeViewModel::StudyTreeViewModel()
  : QStandardItemModel()
{
}


StudyTreeViewModel::~StudyTreeViewModel()
{
  
}


void StudyTreeViewModel::update(Observable* source, const std::string & message)
{
  if (message=="addStudy")
  {
    OTStudy * study = static_cast<OTStudy*>(source);
    addStudyItem(study);
  }
}


void StudyTreeViewModel::createNewStudy()
{
  // find a name not used
  OTStudy * newStudy = new OTStudy("aStudy");
}


void StudyTreeViewModel::addStudyItem(OTStudy * study)
{
  StudyItem * studyItem = new StudyItem(study);
  connect(studyItem, SIGNAL(newPhysicalModelItemCreated(PhysicalModelItem*)), this, SIGNAL(newPhysicalModelCreated(PhysicalModelItem*)));
  connect(studyItem, SIGNAL(newParametricCalculusItemCreated(ParametricCalculusItem*)), this, SIGNAL(newParametricCalculusCreated(ParametricCalculusItem*)));
  study->addObserver(studyItem);
  invisibleRootItem()->appendRow(studyItem);
}


void StudyTreeViewModel::addPhysicalModelItem(const QModelIndex & parentIndex)
{
  // TODO: find a name for the new item
  PhysicalModel newPhysicalModel("aModelPhys");
  StudyItem * parentItem = static_cast<StudyItem*>(itemFromIndex(parentIndex));
  parentItem->getStudy()->addPhysicalModel(newPhysicalModel);
}

}