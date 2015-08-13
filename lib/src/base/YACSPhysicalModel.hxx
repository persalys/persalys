// YACSPhysicalModel.hxx

#ifndef YACSPHYSICALMODEL_H
#define YACSPHYSICALMODEL_H

#include "PhysicalModel.hxx"

namespace OTGUI {
class YACSPhysicalModel : public PhysicalModelImplementation
{
public:
  YACSPhysicalModel(const std::string & name);
  YACSPhysicalModel(const std::string & name, const std::string & fileName);
  YACSPhysicalModel(const YACSPhysicalModel & other);
  virtual YACSPhysicalModel * clone() const;

  virtual ~YACSPhysicalModel();

  std::string getXMLFile() const;
  void setXMLFile(const std::string & fileName);

  void loadDataWithYACS();
  void loadDataWithYACS(const std::string & fileName);

private:
  std::string XMLFile_;
};
}
#endif