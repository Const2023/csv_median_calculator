# csv_median_calculator


Описание приложения:

Требования — список зависимостей и системных требований

Сборка проекта — инструкции по сборке с CMake

Запуск приложения — примеры использования с параметрами

Формат конфигурации — описание всех параметров конфигурационного файла

Примеры — примеры входных и выходных данных


Cтруктура проекта:
csv_median_calculator/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── config_parser.hpp
│   ├── csv_reader.hpp
│   ├── median_calculator.hpp
│   └── ...
├── config.toml
└── examples/
    ├── input/
    │   ├── btcusdt_level_2024.csv
    │   └── btcusdt_trade_2024.csv
    └── output/
        └── median_result.csv
