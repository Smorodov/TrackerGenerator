#ifndef TEMPLATE_CONTAINER_H
#define TEMPLATE_CONTAINER_H

#include <string>
#include <map>

typedef std::map<std::string, std::string> Argmap;

namespace tp
{

/**
 * @brief The TemplateContainer class
 */
class TemplateContainer
{
public:

  /**
   * @brief TemplateContainer
   */
  TemplateContainer();

  /**
   * @brief TemplateContainer
   * @param path
   */
  TemplateContainer(std::string file_path);
  /**
   * @brief TemplateContainer
   * @param arguments
   * @param body
   */
  TemplateContainer(Argmap arguments, std::string body, std::string extension);

  /**
   * @brief setArgument
   * @param argument
   * @param value
   */
  void setArgument(std::string argument, std::string value);

  /**
   * @brief getArgument
   * @param argument
   * @return
   */
  std::string getArgument(std::string argument) const;

  /**
   * @brief processTemplate
   * @return
   */
  std::string processTemplate() const;

  /**
   * @brief processAndSaveTemplate
   * @param base_path
   */
  void processAndSaveTemplate(std::string base_path, std::string name) const;

  /**
   * @brief getArgmap
   * @return
   */
  Argmap getArgmap() const;

  /**
   * @brief getExtension
   * @return
   */
  std::string getExtension() const;

  /**
   * @brief printArguments
   */
  void printArguments() const;

private:
  Argmap arguments_;
  std::string body_;
  std::string extension_;
  std::map<std::string, TemplateContainer> sub_containers_;
};

} // tp namespace
#endif
