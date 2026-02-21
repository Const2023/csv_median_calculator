#include "config_parser.hpp"
#include <boost/program_options.hpp>
#include <toml++/toml.hpp>
#include <spdlog/spdlog.h>
#include <iostream>

namespace po = boost::program_options;

config_parser::config_parser(int argc, char* argv[]) 
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("config,c", po::value<std::string>(), "path to config file")
        ("cfg", po::value<std::string>(), "path to config file (short)");
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if (vm.count("help")) 
    {
        std::cout << desc << "\n";
        std::exit(0);
    }
    
    fs::path config_path;
    if (vm.count("config") || vm.count("cfg")) 
    {
        std::string config_arg = vm.count("config") ? 
            vm["config"].as<std::string>() : vm["cfg"].as<std::string>();
        config_path = config_arg;
    } 
    else 
    {
        config_path = find_config_in_current_dir();
    }
    
    if (!fs::exists(config_path)) 
    {
        spdlog::warn("Config {} not found. Using defaults.", config_path.string());
        _config = create_default_config();
    } 
    else 
    {
        try 
        {
            _config = parse_toml(config_path);
        } 
        catch (const std::exception& ex) 
        {
            spdlog::error("Config error: {}. Using defaults.", ex.what());
            _config = create_default_config();
        }
    }
    
    if (_config.output_dir.empty()) 
    {
        _config.output_dir = fs::current_path() / "output";
    }
    fs::create_directories(_config.output_dir);
    
    spdlog::info("Config loaded: input={}, output={}", 
        _config.input_dir.string(), _config.output_dir.string());
}

config config_parser::parse_toml(const fs::path& config_path) const 
{
    auto data = toml::parse_file(config_path.string());
    config cfg;
    
    if (!data["main"].is_table())
    {
        throw std::runtime_error("No [main] section");
    }
    
    auto main_table = data["main"];
    
    // input
    std::string input_str = main_table["input"].value_or<std::string>("");
    cfg.input_dir = input_str;
    if (cfg.input_dir.empty() || !fs::exists(cfg.input_dir)) 
    {
        throw std::runtime_error("Invalid input directory: " + input_str);
    }
    
    // output
    if (main_table["output"].is_string()) 
    {
        cfg.output_dir = main_table["output"].value_or<std::string>("");
    }
    
    // filename_mask
    if (main_table["filename_mask"].is_array()) 
    {
        auto& arr = *main_table["filename_mask"].as_array();
        for (size_t i = 0; i < arr.size(); ++i) 
        {
            if (arr[i].is_string()) 
            {
                cfg.filename_mask.push_back(arr[i].value_or<std::string>(""));
            }
        }
    }
    
    return cfg;
}

config config_parser::create_default_config() const 
{
    config cfg;
    cfg.input_dir = fs::current_path() / "data";
    cfg.output_dir = fs::current_path() / "output";
    cfg.filename_mask = {"level", "trade"};
    return cfg;
}

fs::path config_parser::find_config_in_current_dir() const 
{
    return fs::current_path() / "config.toml";
}
