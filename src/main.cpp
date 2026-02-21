#include "csv_reader.hpp"

int main() 
{
    fs::path file_path = "./data/trade.csv";
    csv_reader reader{file_path};

    auto records = reader.read_records();

    std::cout << "Прочитано записей: " << records.size() << "\n";
    for (const auto& rec : records) {
        std::cout << "ts: " << rec.receive_ts << ", price: " << rec.price << "\n";
    }
}