#pragma once

#include <ostream>
#include <string>
#include <string_view>

void PrintHelp(std::ostream& out);
bool PrintFilterHelp(std::ostream& out, std::string_view filter_name);
bool IsFilterName(std::string_view filter_name);
std::string NormalizeFilterName(std::string_view filter_arg);

bool IsHelpArg(std::string_view arg);
