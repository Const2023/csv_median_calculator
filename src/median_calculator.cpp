#include "median_calculator.hpp"
#include <algorithm>
#include <iomanip>

median_calculator::median_calculator(fs::path output_file) 
    : _output_file{std::move(output_file)} 
{
    _output_stream = std::make_unique<std::ofstream>(_output_file);
    if (!_output_stream->is_open()) {
        throw std::runtime_error("Cannot open output file: " + _output_file.string());
    }
    
    // Заголовок файла
    *_output_stream << "timestamp;median_price\n";
    _output_stream->flush();
    spdlog::info("Median calculator initialized, output: {}", _output_file.string());
}

void median_calculator::add_price(double price) 
{
    std::lock_guard<std::mutex> lock(_mutex);
    _prices.push_back(price);
    spdlog::debug("Added price: {:.6f}, total: {}", price, _prices.size());
}

bool median_calculator::update_median(double price) 
{
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Добавляем новую цену
    _prices.push_back(price);
    
    // Вычисляем новую медиану
    double new_median = calculate_median();
    
    // Проверяем изменение
    bool changed = std::abs(new_median - _current_median) > 1e-10;
    
    if (changed) {
        _current_median = new_median;
        _median_changed = true;
        
        // Записываем в файл: timestamp + новая медиана
        auto now = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        *_output_stream << now << ";" << std::fixed << std::setprecision(8) << _current_median << "\n";
        _output_stream->flush();
        
        spdlog::debug("Median changed: {:.6f} -> {:.6f} (total prices: {})", 
            _current_median, new_median, _prices.size());
    }
    
    return changed;
}

double median_calculator::get_current_median() const 
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _current_median;
}

double median_calculator::calculate_median() const 
{
    if (_prices.empty()) {
        return 0.0;
    }
    
    std::vector<double> sorted = _prices;
    std::sort(sorted.begin(), sorted.end());
    
    size_t n = sorted.size();
    if (n % 2 == 1) {
        // Нечетное количество: средний элемент
        return sorted[n / 2];
    } else {
        // Четное количество: среднее двух центральных
        return (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;
    }
}

void median_calculator::flush_to_file() const 
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_output_stream && _output_stream->is_open()) {
        _output_stream->flush();
        spdlog::info("Flushed {} median records to {}", 
            std::count_if(_prices.begin(), _prices.end(), 
                [this](double p) { return std::abs(calculate_median() - p) < 1e-10; }),
            _output_file.string());
    }
}
