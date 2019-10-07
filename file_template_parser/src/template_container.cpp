#include "file_template_parser/template_container.h"
#include "file_template_parser/template_management_tools.h"
#include <tinyxml.h>
#include <stdexcept>
#include <iostream>
#include <boost/algorithm/string/replace.hpp>

namespace tp
{

/*
 * Helper functions
 */

/**
 * @brief getAttribute
 * @param attribute_name
 * @param xml_element
 * @return
 */
std::string getAttribute(std::string attribute_name, TiXmlElement* xml_element)
{
  const char* attribute = xml_element->Attribute(attribute_name.c_str());
  if (attribute == NULL)
  {
    throw std::runtime_error("Missing attribute: '" + attribute_name + "'");
  }

  return std::string(attribute);
}

/**
 * @brief getArguments
 * @param root_element
 * @return
 */
Argmap getArguments(TiXmlElement* root_element)
{
  Argmap arguments;

  // Iterate through all arguments
  for (TiXmlElement* arg_element = root_element->FirstChildElement("arg");
                     arg_element != NULL;
                     arg_element = arg_element->NextSiblingElement("arg"))
  {
    std::string name          = getAttribute("name", arg_element);
    std::string default_value = getAttribute("default", arg_element);
    arguments[name] = default_value;
  }

  // Check if there are any arguments
  if (arguments.empty())
  {
    throw std::runtime_error("The file template does not contain arguments");
  }

  return arguments;
}

/**
 * @brief getBody
 * @param root_element
 * @return
 */
std::string getBody(TiXmlElement* root_element)
{
  TiXmlElement* body_element = root_element->FirstChildElement("body");

  // Check the body element
  if(body_element == NULL)
  {
    throw std::runtime_error("The file template does not contain a body element");
  }

  const char* body = body_element->GetText();

  // Check the text
  if (body == NULL)
  {
    throw std::runtime_error("The text in the 'body' is compromised");
  }

  return std::string(body);
}

// -------------------------------------------------------------------------------------------- //

/*
 * Template container class
 */

// Default constructor
TemplateContainer::TemplateContainer()
{}

// Constructs the template object from the given template file
TemplateContainer::TemplateContainer(std::string file_path)
{
  TiXmlDocument template_xml;

  try
  {
    // Open the template file
    if(!template_xml.LoadFile(file_path.c_str()))
    {
      throw std::runtime_error("Cannot open the file template");
    }

    // Get the root element
    TiXmlElement* root_element = template_xml.FirstChildElement("f_template");

    // Check if any element was received
    if( root_element == NULL )
    {
      throw std::runtime_error("Missing 'f_template' element");
    }

    // Get the output file extension hint
    extension_ = getAttribute("extension", root_element);

    // Check if the args is false
    std::string args_attr;
    try
    {
      args_attr = getAttribute("args", root_element);
    }
    catch(std::runtime_error e)
    {
      // Do nothing about it, args attribute is not mandatory ... yet
    }

    if (args_attr != "false")
    {
      // Get template arguments
      arguments_ = getArguments(root_element);
    }

    // Get template body
    body_ = getBody(root_element);
  }
  catch(std::runtime_error e)
  {
    template_xml.Clear();
    throw std::runtime_error(std::string(e.what()) + ": in '" + file_path + "'");
  }

  catch(std::exception e)
  {
    template_xml.Clear();
    throw std::runtime_error(std::string(e.what()) + ": in '" + file_path + "'");
  }

  template_xml.Clear();
}

// TemplateContainer constructor
TemplateContainer::TemplateContainer(Argmap arguments, std::string body, std::string extension)
: arguments_(arguments),
  body_(body),
  extension_(extension)
{}

// Set template argument
void TemplateContainer::setArgument(std::string argument, std::string value)
{
  if (arguments_.find(argument) != arguments_.end())
  {
    arguments_[argument] = value;
  }
  else
  {
    throw std::runtime_error("Cannot recognize the argument '" + argument + "'");
  }
}

// Get template argument
std::string TemplateContainer::getArgument(std::string argument) const
{
  return arguments_.find(argument)->second;
}

// Get the argument map
Argmap TemplateContainer::getArgmap() const
{
  return arguments_;
}

// Get the extension
std::string TemplateContainer::getExtension() const
{
  return extension_;
}

// Print the arguments
void TemplateContainer::printArguments() const
{
  for (auto arg : arguments_)
  {
    std::cout << " * name: " << arg.first << "; value: " << arg.second << std::endl;
  }
}

// Process the template
// TODO: Check for arg syntax errors
std::string TemplateContainer::processTemplate() const
{
  std::string processed_body = body_;

  for (auto arg : arguments_)
  {
    boost::replace_all(processed_body, "$(arg " + arg.first + ")", arg.second);
  }

  return processed_body;
}

// Process and save the template
// TODO: Check the base path
void TemplateContainer::processAndSaveTemplate(std::string base_path, std::string name) const
{
  saveStrToFile(this->processTemplate(), base_path, name, extension_);
}

} // tp namespace
