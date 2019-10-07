#include "file_template_parser/template_management_tools.h"
#include <fstream>

namespace tp
{

// Process and save the template
void saveStrToFile(std::string input_string,
                   std::string base_path,
                   std::string name,
                   std::string extension)
{
  std::ofstream file_out(base_path + "/" + name + extension);
  file_out << input_string;
  file_out.close();
}

} // tp namespace
