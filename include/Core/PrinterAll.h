#pragma once

class Printer {
public:
    PrinterAll();
    ~PrinterAll();

    void print();
private:
    std::vector<std::unique_ptr<Printer>> m_Printers;
}