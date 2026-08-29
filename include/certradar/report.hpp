#pragma once

#include "certradar/diagnosis.hpp"

#include <string>
#include <vector>

namespace certradar {

std::string mask_identifier(const std::string& value);
std::string mask_path(const std::string& path);
std::string build_support_summary(const std::vector<Finding>& findings);
std::string build_redacted_report_json(const std::vector<Finding>& findings);

}  // namespace certradar
