#ifndef MEDIAN_CALCULATOR_HPP
#define MEDIAN_CALCULATOR_HPP

#include <filesystem>
#include <fstream>
#include <vector>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

class median_calculator 
{
public:
    explicit median_calculator(fs::path output_file);
    
    // Добавляет цену из level_record_t или trade_record_t
    void add_price(double price);
    
    // Вычисляет текущую медиану и возвращает true если она изменилась
    bool update_median(double price);
    
    // Получить текущую медиану
    double get_current_median() const;
    
    // Сохранить текущее состояние в файл
    void flush_to_file() const;

private:
    double calculate_median() const;
    
    mutable std::mutex _mutex;
    std::vector<double> _prices;
    double _current_median{0.0};
    fs::path _output_file;
    mutable std::unique_ptr<std::ofstream> _output_stream;
    bool _median_changed{false};
};

#endif // MEDIAN_CALCULATOR_HPP