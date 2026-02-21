#include "csv_reader.hpp"
#include "median_calculator.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <iomanip>

namespace fs = std::filesystem;

int main() 
{
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);
    spdlog::set_level(spdlog::level::info);

    // Создаем выходной файл
    fs::create_directories("output");
    median_calculator calc("output/median_history.csv");

    spdlog::info("=== CSV Median Calculator Started ===");

    try 
    {
        // Читаем level.csv
        csv_reader level_reader("data/level.csv");
        auto levels = std::get<std::vector<level_record_t>>(level_reader.read_records());
        spdlog::info("Loaded {} level records", levels.size());

        // Читаем trade.csv
        csv_reader trade_reader("data/trade.csv");
        auto trades = std::get<std::vector<trade_record_t>>(trade_reader.read_records());
        spdlog::info("Loaded {} trade records", trades.size());

        // ИНКРЕМЕНТАЛЬНЫЙ РАСЧЕТ МЕДИАНЫ
        spdlog::info("=== Calculating running median ===");
        
        // Обрабатываем все цены из level
        for (const auto& rec : levels) {
            calc.update_median(rec.price);  // Добавляет цену и проверяет изменение медианы
        }
        
        // Обрабатываем все цены из trade
        for (const auto& rec : trades) {
            calc.update_median(rec.price);
        }

        double final_median = calc.get_current_median();
        spdlog::info("=== FINAL RESULT ===");
        spdlog::info("Total prices processed: {}", levels.size() + trades.size());
        spdlog::info("Final median: {:.8f}", final_median);
        calc.flush_to_file();

    } catch (const std::exception& ex) {
        spdlog::error("Error: {}", ex.what());
        return 1;
    }

    spdlog::info("=== Done ===");
    return 0;
}
