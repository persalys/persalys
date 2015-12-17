// ImportTablePage.cxx

#include "otgui/ImportTablePage.hxx"

#include "otgui/HorizontalHeaderViewWithCombobox.hxx"

#include <NumericalSample.hxx>

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>

using namespace OT;

namespace OTGUI {

ImportTablePage::ImportTablePage(const DesignOfExperiment & designOfExperiment, QWidget *parent)
  : QWizardPage(parent)
  , designOfExperiment_(designOfExperiment)
  , pageValidity_(false)
{
  buildInterface();
}

void ImportTablePage::buildInterface()
{
  setWindowTitle(tr("Import table from file"));

  QGridLayout * mainGridLayout = new QGridLayout(this);

  // first row
  QHBoxLayout * hboxLayout = new QHBoxLayout;
  QLabel * label = new QLabel(tr("File path:"));
  hboxLayout->addWidget(label);

  filePathLineEdit_ = new QLineEdit;
  hboxLayout->addWidget(filePathLineEdit_);

  QPushButton * openFileButton = new QPushButton(tr("Open"));
  connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileRequested()));
  hboxLayout->addWidget(openFileButton);

  mainGridLayout->addLayout(hboxLayout, 0, 0, 1, 3);

  QGroupBox * groupBox = new QGroupBox(tr("File Preview"));
  QGridLayout * gridLayout = new QGridLayout(groupBox);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);

  dataPreviewTableView_ = new OTguiTableView(groupBox);
  gridLayout->addWidget(dataPreviewTableView_, 0, 0, 1, 1);
  
  mainGridLayout->addWidget(groupBox, 1, 0, 1, 1);
}


void ImportTablePage::openFileRequested()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Data to import..."),
                     QDir::homePath(),
                     tr("Text Files (*.txt);;"));

  if (!fileName.isEmpty())
  {
    filePathLineEdit_->setText(fileName);
    try
    {
      designOfExperiment_.setFileName(filePathLineEdit_->text().toStdString());
      std::vector< std::string > separatorsList(3);
      separatorsList[0] = " ";
      separatorsList[1] = ",";
      separatorsList[2] = ";";
      NumericalSample sample;
      for (int i=0; i<3; ++i)
      {
        // import sample from the file
        sample = NumericalSample::ImportFromTextFile(fileName.toStdString(), separatorsList[i]);
        if (sample.getSize())
          break;
      }
      if (!sample.getSize())
        throw;
      Description desc = Description(sample.getDimension());
      Description inputNames = designOfExperiment_.getPhysicalModel().getInputNames();
      for (int i=0; i<designOfExperiment_.getColumns().getSize(); ++i)
        desc[designOfExperiment_.getColumns()[i]] = inputNames[i];
      sample.setDescription(desc);
      dataPreviewTableView_->setModel(new OTguiTableModel(sample));
      connect(dataPreviewTableView_->model(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, SLOT(columnChanged(Qt::Orientation,int,int)));

      QStringList comboBoxItems;
      for (int i=0; i<inputNames.getSize(); ++i)
        comboBoxItems << inputNames[i].c_str();
      comboBoxItems << "";

      QVector<int> columns(dataPreviewTableView_->model()->columnCount());
      for (int i=0; i<columns.size(); ++i)
        columns[i] = i;
      HorizontalHeaderViewWithCombobox * header = new HorizontalHeaderViewWithCombobox(comboBoxItems, columns, dataPreviewTableView_);
      dataPreviewTableView_->setHorizontalHeader(header);
      dataPreviewTableView_->horizontalHeader()->show();

      pageValidity_ = true;
    }
    catch (...)
    {
      pageValidity_ = false;
    }
  }
}


void ImportTablePage::columnChanged(Qt::Orientation, int, int)
{
  Description inputNames = designOfExperiment_.getPhysicalModel().getInputNames();
  Indices columns;
  for (int i=0; i<inputNames.getSize(); ++i)
    for (int j=0; j<dataPreviewTableView_->model()->columnCount(); ++j)
      if (inputNames[i] == dataPreviewTableView_->model()->headerData(j, Qt::Horizontal).toString().toStdString())
      {
        columns.add(j);
        break;
      }

  try
  {
    designOfExperiment_.setColumns(columns);
    pageValidity_ = true;
  }
  catch(InvalidArgumentException &)
  {
    pageValidity_ = false;
  }
}

bool ImportTablePage::validatePage()
{
  return pageValidity_;
}
}