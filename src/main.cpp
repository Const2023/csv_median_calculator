#include "csv_reader.hpp"
#include "median_calculator.hpp"
#include "config_parser.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <iomanip>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) 
{
    // Инициализация spdlog
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);
    spdlog::set_level(spdlog::level::info);
    
    // 1. Парсинг конфигурации
    config_parser parser(argc, argv);
    const auto& cfg = parser.get();
    
    // 2. Поиск и чтение CSV файлов
    std::vector<fs::path> csv_files;
    for (const auto& entry : fs::directory_iterator(cfg.input_dir)) 
    {
        if (entry.is_regular_file() && entry.path().extension() == ".csv") 
        {
            std::string filename = entry.path().stem().string();
            bool matches_mask = cfg.filename_mask.empty();
            
            if (!matches_mask) {
                for (const auto& mask : cfg.filename_mask) 
                {
                    if (filename.find(mask) != std::string::npos) 
                    {
                        matches_mask = true;
                        break;
                    }
                }
            }
            
            if (matches_mask) 
            {
                csv_files.push_back(entry.path());
                spdlog::info("Found CSV file: {}", entry.path().string());
            }
        }
    }
    
    // 3. Создание выходного файла
    fs::path output_file = cfg.output_dir / "median_history.csv";
    median_calculator calc(output_file);
    
    // 4. Обработка всех CSV файлов
    for (const auto& csv_file : csv_files) 
    {
        spdlog::info("Processing file: {}", csv_file.string());
        csv_reader reader(csv_file);
        
        if (reader.get_type() == csv_file_type::LEVEL) 
        {
            auto levels = std::get<std::vector<level_record_t>>(reader.read_records());
            for (const auto& rec : levels) 
            {
                calc.update_median(rec.price, rec.receive_ts);
            }
        } 
        else 
        {
            auto trades = std::get<std::vector<trade_record_t>>(reader.read_records());
            for (const auto& rec : trades) 
            {
                calc.update_median(rec.price, rec.receive_ts);
            }
        }
    }
    
    calc.flush_to_file();
    spdlog::info("Median calculation completed. Output: {}", output_file.string());
    return 0;
}
