#include <iostream>
#include "file_template_parser/file_template_parser.h"
#include <stdexcept>

int main(int argc, char **argv)
{
    std::string base_path = "./../../test/";
    tp::TemplateContainer t_random(base_path + "templates/template.xml");
    tp::TemplateContainer t_cmakelists(base_path + "templates/temoto_ai_cmakelists.xml");
    tp::TemplateContainer t_packagexml(base_path + "templates/temoto_ai_packagexml.xml");
    

    // 
    // Set the arguments
    // 
    t_random.setArgument("noun", "dog");
    t_random.setArgument("adjective", "blue");
    t_random.setArgument("ai_name", "start");
    t_cmakelists.setArgument("ai_name", "ai_name_test");
    t_packagexml.setArgument("ai_name", "ai_name_test");

    // 
    // Save the processed template
    // 
    t_random.processAndSaveTemplate(base_path, "template_output");
    t_cmakelists.processAndSaveTemplate(base_path, "CmakeLists");
    t_packagexml.processAndSaveTemplate(base_path, "package");  
    
    t_random.printArguments();
    // 
    // Print out the body
    // 
    std::cout << t_random.processTemplate() << std::endl;

  return 0;
}
