#ifndef CSV_READER_HPP
#define CSV_READER_HPP
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <variant>

namespace fs = std::filesystem;

struct level_record_t 
{
    uint64_t receive_ts;
    uint64_t exchange_ts;
    double price;
    double quantity;
    std::string side;  // "bid" или "ask"
    int rebuild;
};

struct trade_record_t 
{
    uint64_t receive_ts;
    uint64_t exchange_ts;
    double price;
    double quantity;
    std::string side;  // "bid" или "ask"
};

enum class csv_file_type 
{
    LEVEL,
    TRADE
};

class csv_reader 
{
public:
    explicit csv_reader(fs::path file_path);
    
    // Возвращает vector 
    std::variant<std::vector<level_record_t>, std::vector<trade_record_t>> read_records() const;

    csv_file_type get_type() const;

private:

    std::vector<level_record_t> parse_level() const;
    std::vector<trade_record_t> parse_trade() const;
    
    std::vector<std::string> split_csv_line(const std::string& line) const;

    fs::path _file_path;
    csv_file_type _type;
};

#endif // CSV_READER_HPP