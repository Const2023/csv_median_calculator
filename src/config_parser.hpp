#ifndef CONFIG_PARSER_CPP
#define CONFIG_PARSER_CPP

#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

struct config 
{
    fs::path input_dir;           
    fs::path output_dir;          
    std::vector<std::string> filename_mask;  
};

class config_parser 
{
public:
    explicit config_parser(int argc, char* argv[]);
    const config& get() const { return _config; }
    fs::path get_default_config_path() const;

private:
    config parse_toml(const fs::path& config_path) const;
    config create_default_config() const;
    fs::path find_config_in_current_dir() const;
    
    config _config;
};



#endif // CONFIG_PARSER_CPP