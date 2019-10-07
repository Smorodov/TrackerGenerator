#ifndef TEMPLATE_MANAGEMENT_TOOLS_H
#define TEMPLATE_MANAGEMENT_TOOLS_H

#include <string>

namespace tp
{

/**
 * @brief saveStrToFile
 * @param input_string
 * @param base_path
 * @param name
 */
void saveStrToFile(std::string input_string,
                   std::string base_path,
                   std::string name,
                   std::string extension);

} // tp namespace

#endif
