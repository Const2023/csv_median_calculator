#include "csv_reader.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <filesystem>
#include <iomanip>

namespace fs = std::filesystem;

int main() {
    // Инициализация spdlog с debug уровнем
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);
    spdlog::set_level(spdlog::level::debug);

    spdlog::info("=== CSV Median Calculator Started ===");

    // Проверка существования файлов
    if (!fs::exists("data/level.csv")) {
        spdlog::error("File 'data/level.csv' not found!");
        return 1;
    }
    if (!fs::exists("data/trade.csv")) {
        spdlog::error("File 'data/trade.csv' not found!");
        return 1;
    }

    spdlog::info("Files found, creating readers...");

    try {
        csv_reader level_reader("data/level.csv");
        spdlog::info("Level reader created, type: {}", 
            level_reader.get_type() == csv_file_type::LEVEL ? "LEVEL" : "TRADE");

        spdlog::info("Reading level.csv...");
        auto levels = std::get<std::vector<level_record_t>>(level_reader.read_records());
        spdlog::info("Loaded {} level records", levels.size());

        // ВЫВОД ПЕРВЫХ 5 LEVEL ЗАПИСЕЙ
        spdlog::info("=== FIRST 5 LEVEL RECORDS ===");
        for (size_t i = 0; i < std::min<size_t>(5, levels.size()); ++i) {
            const auto& rec = levels[i];
            spdlog::info("Level[{}]: ts={} price={:.2f} qty={:.3f} side={} rebuild={}", 
                i,
                rec.receive_ts,
                rec.price,
                rec.quantity,
                rec.side,
                rec.rebuild);
        }

        spdlog::info("Reading trade.csv...");
        csv_reader trade_reader("data/trade.csv");
        spdlog::info("Trade reader created, type: {}", 
            trade_reader.get_type() == csv_file_type::LEVEL ? "LEVEL" : "TRADE");

        auto trades = std::get<std::vector<trade_record_t>>(trade_reader.read_records());
        spdlog::info("Loaded {} trade records", trades.size());

        // ВЫВОД ПЕРВЫХ 5 TRADE ЗАПИСЕЙ
        spdlog::info("=== FIRST 5 TRADE RECORDS ===");
        for (size_t i = 0; i < std::min<size_t>(5, trades.size()); ++i) {
            const auto& rec = trades[i];
            spdlog::info("Trade[{}]: ts={} price={:.2f} qty={:.3f} side={}", 
                i,
                rec.receive_ts,
                rec.price,
                rec.quantity,
                rec.side);
        }

        // СТАТИСТИКА
        spdlog::info("=== SUMMARY ===");
        spdlog::info("Level records: {}", levels.size());
        spdlog::info("Trade records: {}", trades.size());
        spdlog::info("Total records: {}", levels.size() + trades.size());

    } catch (const std::exception& ex) {
        spdlog::error("Error: {}", ex.what());
        return 1;
    }

    spdlog::info("=== Program finished successfully ===");
    return 0;
}
