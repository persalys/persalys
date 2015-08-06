// YACSPhysicalModel.hxx

#ifndef YACSPHYSICALMODEL_H
#define YACSPHYSICALMODEL_H

#include "PhysicalModel.hxx"

namespace OTGUI {
class YACSPhysicalModel : public PhysicalModel
{
public:
  YACSPhysicalModel();

  virtual ~YACSPhysicalModel();

  std::string getXMLFile() const;

  void loadDataWithYACS(const std::string fileName);

private:
  std::string XMLFile_;
};
}
#endif