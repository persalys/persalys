
#include "persalys/AnsysWizard.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/CheckableHeaderView.hxx"


#include "openturns/XMLToolbox.hxx"

#include <limits>
#include <iostream>

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>


using namespace OT;

namespace PERSALYS
{

AnsysWizard::AnsysWizard(QWidget * parent)
  : Wizard(parent)
  , parser_(0)
{
  docLink_ = "user_manual/graphical_interface/physical_model/user_manual_physical_model.html#ansys-wizard";
  setWindowTitle(tr("Persalys: Ansys coupling"));
  varPage_ = new AnsysWizardVariablePage(this);
  addPage(varPage_);

  sysPage_ = new AnsysWizardSystemPage(this);
  addPage(sysPage_);

  connect(varPage_, SIGNAL(loadModelRequested()), this, SLOT(loadModel()));
  connect(varPage_, &AnsysWizardVariablePage::executableFileFound, [=]() {
      parser_->setExecutableFileName(varPage_->executableLineEdit_->text().toStdString());
    });
}


void AnsysWizard::loadModel()
{
  parser_ = new AnsysParser(varPage_->modelFileLineEdit_->text().toStdString());
  varPage_->varModel_->loadData(parser_);
  varPage_->varTable_->show();

  sysPage_->sysModel_->loadData(parser_);
  sysPage_->sysTable_->show();

  if (getExecutableFileName().isEmpty())
  {
    QString defaultExecutable;
#ifdef _WIN32
    defaultExecutable = "C:/Program Files/ANSYS Inc/v"
      + QString(parser_->getAnsysVersion().c_str())
      + "/Framework/bin/Win64/RunWB2.exe";
#else
    defaultExecutable = "/ansys_inc/v"
      + QString(parser_->getAnsysVersion().c_str())
      + "/Framework/bin/Linux64/runwb2";
#endif
    varPage_->executableLineEdit_->setText(defaultExecutable);
  }
}


void AnsysWizard::validateVariables()
{
  InputCollection inputs;
  OutputCollection outputs;
  InputCollection rawInputs = parser_->getInputVariables();
  for(UnsignedInteger i = 0; i < rawInputs.getSize(); ++i)
    if(varPage_->varModel_->getInputVariables().contains(QString(rawInputs[i].getName().c_str())))
      inputs.add(rawInputs[i]);

  OutputCollection rawOutputs = getParser()->getOutputVariables();
  for(UnsignedInteger i = 0; i < rawOutputs.getSize(); ++i)
    if(varPage_->varModel_->getOutputVariables().contains(QString(rawOutputs[i].getName().c_str())))
      outputs.add(rawOutputs[i]);

  parser_->setInputVariables(inputs);
  parser_->setOutputVariables(outputs);
}


void AnsysWizard::validateSystems()
{
  std::map<String, std::pair<String, String> > sys;
  std::map<String, std::pair<String, String> > rawSys = parser_->getSystems();
  for (auto const& iSys : rawSys)
    if(sysPage_->sysModel_->getSystems().contains(QString(iSys.first.c_str())))
      sys[iSys.first] = iSys.second;
  parser_->setSystems(sys);
}


QString AnsysWizard::getExecutableFileName() const{
  return varPage_->executableLineEdit_->text();
}


QString AnsysWizard::getModelFileName() const{
  return varPage_->modelFileLineEdit_->text();
}


AnsysParser* AnsysWizard::getParser() const {
  return parser_;
}


AnsysWizardVariablePage::AnsysWizardVariablePage(QWidget * parent)
  : QWizardPage(parent)
  , errorMessageLabel_(0)
{
  setSubTitle(tr("Variable selection"));
  QGridLayout *gridLayout = new QGridLayout;

  QLabel *label = new QLabel(tr("Model file"));
  label->setToolTip(tr("Path to the wbpj file"));
  modelFileLineEdit_ = new QLineEdit;
  QPushButton * findFile = new QPushButton(tr("..."));
  connect(findFile, SIGNAL(clicked()), this, SLOT(findModelFile()));

  gridLayout->addWidget(label, 1, 0);
  gridLayout->addWidget(modelFileLineEdit_, 1, 1, 1, 2);
  gridLayout->addWidget(findFile, 1, 3);
  gridLayout->setColumnStretch(1, 2);

  label = new QLabel(tr("Ansys solver"));
  label->setToolTip(tr("Path to the runwb2 executable"));
  executableLineEdit_ = new QLineEdit;
  findFile = new QPushButton(tr("..."));
  connect(findFile, SIGNAL(clicked()), this, SLOT(findExecutableFile()));
  gridLayout->addWidget(label, 2, 0);
  gridLayout->addWidget(executableLineEdit_, 2, 1, 1, 2);
  gridLayout->addWidget(findFile, 2, 3);
  gridLayout->setColumnStretch(1, 2);

  varTable_ = new QTableView;    // - header view
  CheckableHeaderView * varTableHeaderView = new CheckableHeaderView;
  varTable_->setHorizontalHeader(varTableHeaderView);
  varTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  varTable_->verticalHeader()->hide();
  varTable_->setAlternatingRowColors(false);
  varTable_->hide();
  varModel_ = new AnsysVariableTableModel;
  varTable_->setModel(varModel_);
  gridLayout->addWidget(varTable_, 3, 0, 1, 4);
  gridLayout->setRowStretch(3, 2);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(varModel_, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
          errorMessageLabel_, SLOT(reset()));
  gridLayout->addWidget(errorMessageLabel_, 4, 0, 1, 4);
  setLayout(gridLayout);
}


void AnsysWizardVariablePage::findExecutableFile()
{
#ifdef _WIN32
  QString file = QFileDialog::getOpenFileName(this, tr("Select the solver"),
                                              executableLineEdit_->text(),
                                              tr("Executable file (*.exe);; All files (*)"));
#else
  QString file = QFileDialog::getOpenFileName(this, tr("Select the solver"),
                                              executableLineEdit_->text(),
                                              tr("All files (*)"));
#endif

  if (!file.isEmpty())
  {
    executableLineEdit_->setText(file);
  }
}


void AnsysWizardVariablePage::findModelFile()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select the model"), FileTools::GetCurrentDir(),
                                              tr("Ansys model (*.wbpj);; All files (*)"));
  if (!file.isEmpty())
  {
    modelFileLineEdit_->setText(file);
    FileTools::SetCurrentDir(file);
    emit loadModelRequested();
  }
}


bool AnsysWizardVariablePage::validatePage()
{
  if(!QFile(executableLineEdit_->text()).exists())
  {
    errorMessageLabel_->setErrorMessage(tr("Cannot find Ansys solver."));
    return false;
  }
  if(!QFile(modelFileLineEdit_->text()).exists())
  {
    errorMessageLabel_->setErrorMessage(tr("Cannot find project file."));
    return false;
  }
  if(varModel_->getInputVariables().isEmpty() &&
     varModel_->getOutputVariables().isEmpty())
  {
    errorMessageLabel_->setErrorMessage(tr("Please select at least one variable."));
    return false;
  }
  emit executableFileFound();
  return QWizardPage::validatePage();
}


AnsysWizardSystemPage::AnsysWizardSystemPage(QWidget * parent)
  : QWizardPage(parent)
  , errorMessageLabel_(0)
{
  setSubTitle(tr("System selection"));

  QVBoxLayout * pageLayout = new QVBoxLayout;
  QLabel * lbl = new QLabel(tr("Choose system(s) that need(s) updating"));
  pageLayout->addWidget(lbl);

  sysTable_ = new QTableView;
  CheckableHeaderView * sysTableHeaderView = new CheckableHeaderView;
  sysTable_->setHorizontalHeader(sysTableHeaderView);
  sysTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  sysTable_->verticalHeader()->hide();
  sysTable_->setAlternatingRowColors(false);
  sysTable_->hide();
  sysTable_->setMinimumSize(500, 400);
  sysModel_ = new AnsysSystemTableModel;
  sysTable_->setModel(sysModel_);
  pageLayout->addWidget(sysTable_);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  connect(sysModel_, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
          errorMessageLabel_, SLOT(reset()));
  pageLayout->addWidget(errorMessageLabel_);

  setLayout(pageLayout);
}

bool AnsysWizardSystemPage::validatePage()
{
  if(sysModel_->getSystems().isEmpty())
  {
    errorMessageLabel_->setErrorMessage(tr("Please select at least one system."));
    return false;
  }
  return QWizardPage::validatePage();
}

}
