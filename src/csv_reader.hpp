#ifndef CSV_READER_HPP
#define CSV_READER_HPP
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

struct record_t 
{
    std::uint64_t receive_ts{};
    double price{};
};

class csv_reader 
{
    public:
        explicit csv_reader(fs::path file_path);

        std::vector<record_t> read_records() const;



    private:
        fs::path _file_path;
};

#endif // CSV_READER_HPP