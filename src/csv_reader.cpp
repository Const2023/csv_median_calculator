#include "csv_reader.hpp"
#include <boost/algorithm/string.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>


csv_reader::csv_reader(fs::path file_path) 
    : _file_path{std::move(file_path)} 
{
    std::string filename = _file_path.filename().string();
    if (filename.find("level.csv") != std::string::npos) 
    {
        _type = csv_file_type::LEVEL;
    } else if (filename.find("trade.csv") != std::string::npos) 
    {
        _type = csv_file_type::TRADE;
    } else 
    {
        throw std::runtime_error("Unknown CSV type: " + filename);
    }
}

std::variant<std::vector<level_record_t>, std::vector<trade_record_t>> 
csv_reader::read_records() const 
{
    if (_type == csv_file_type::LEVEL) 
    {
        return parse_level();
    } else 
    {
        return parse_trade();
    }
}

std::vector<std::string> csv_reader::split_csv_line(const std::string& line) const 
{
    std::vector<std::string> cols;
    boost::split(cols, line, boost::is_any_of(";"));
    return cols;
}

std::vector<level_record_t> csv_reader::parse_level() const 
{
    std::ifstream fin(_file_path);
    if (!fin.is_open()) 
    {
        throw std::runtime_error("cannot open level csv file: " + _file_path.string());
    }

    std::vector<level_record_t> records;
    std::string line;
    std::size_t line_number = 1;

    // пропускаем заголовок
    if (!std::getline(fin, line)) 
    {
        spdlog::warn("empty level file: {}", _file_path.string());
        return records;
    }

    while (std::getline(fin, line)) 
    {
        ++line_number;
        if (line.empty()) 
        {
            continue;
        }

        auto cols = split_csv_line(line);
        if (cols.size() < 6) 
        {
            spdlog::warn(
                "invalid level csv format in {} at line {}: column count {} < 6",
                _file_path.string(), line_number, cols.size());
            continue;
        }

        try 
        {
            level_record_t rec
            {
                .receive_ts = std::stoull(cols[0]),
                .exchange_ts = std::stoull(cols[1]),
                .price = std::stod(cols[2]),
                .quantity = std::stod(cols[3]),
                .side = cols[4],
                .rebuild = std::stoi(cols[5])
            };
            records.push_back(rec);
        } 
        catch (const std::exception& ex) 
        {
            spdlog::warn(
                "failed to parse level line {} in {}: {}",
                line_number, _file_path.string(), ex.what());
        }
    }

    return records;
}

std::vector<trade_record_t> csv_reader::parse_trade() const
{
    std::ifstream fin(_file_path);
    if (!fin.is_open()) 
    {
        throw std::runtime_error("cannot open trade csv file: " + _file_path.string());
    }

    std::vector<trade_record_t> records;
    std::string line;
    std::size_t line_number = 1;

    // пропускаем заголовок
    if (!std::getline(fin, line)) 
    {
        spdlog::warn("empty trade file: {}", _file_path.string());
        return records;
    }

    while (std::getline(fin, line)) 
    {
        ++line_number;
        if (line.empty()) 
        {
            continue;
        }

        auto cols = split_csv_line(line);
        if (cols.size() < 5) 
        {
            spdlog::warn(
                "invalid trade csv format in {} at line {}: column count {} < 5",
                _file_path.string(), line_number, cols.size());
            continue;
        }

        try 
        {
            trade_record_t rec
            {
                .receive_ts = std::stoull(cols[0]),
                .exchange_ts = std::stoull(cols[1]),
                .price = std::stod(cols[2]),
                .quantity = std::stod(cols[3]),
                .side = cols[4]
            };
            records.push_back(rec);
        } catch (const std::exception& ex) 
        {
            spdlog::warn(
                "failed to parse trade line {} in {}: {}",
                line_number, _file_path.string(), ex.what());
        }
    }

    return records;
}

csv_file_type csv_reader::get_type() const
{
    if (_type == csv_file_type::LEVEL) 
    {
        return csv_file_type::LEVEL;
    } else 
    {
        return csv_file_type::TRADE;
    }
}
