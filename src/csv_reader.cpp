#include "csv_reader.hpp"
#include <boost/algorithm/string.hpp>
#include <spdlog/spdlog.h>

csv_reader::csv_reader(fs::path file_path) : _file_path{std::move(file_path)} {}

std::vector<record_t> csv_reader::read_records() const 
{
    std::ifstream fin(_file_path);
    if (!fin.is_open()) 
    {
        throw std::runtime_error("cannot open csv file: {}" +  _file_path.string());
    }

    std::vector<record_t> records;
    std::string line;

    // пропускаем заголовок
    if (!std::getline(fin, line)) 
    {
        return records;
    }

    std::size_t line_number{1};

    while (std::getline(fin, line)) 
    {
        ++line_number;
        if (line.empty()) 
        {
            continue;
        }

        std::vector<std::string> cols;
        boost::split(cols, line, boost::is_any_of(";"));

        if (cols.size() < 3) 
        {
            spdlog::warn(
            "invalid csv format in {} at line {}: column count {} < 3",
            _file_path.string(),
            line_number,
            cols.size());
            continue;
        }

        try 
        {
            record_t rec
            {
                .receive_ts = std::stoull(cols[0]),
                .price = std::stod(cols[2])
            };
            records.push_back(rec);
        } 
        catch (const std::exception& ex) 
        {
            spdlog::warn(
            "failed to parse line {} in {}: {}",
            line_number,
            _file_path.string(),
            ex.what());
        }
    }

    return records;
}


