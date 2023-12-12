#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
class BitReader {

public:
    bool ReadBit();
    BitReader();
    explicit BitReader(const std::string& str);
    void Open(const std::string& path);
    void Close();
    static const size_t BUFFSIZE = 1024;
    static const size_t CHARBITS = 8;
    static const size_t MAXPOS = BUFFSIZE * CHARBITS;
    static const size_t CHARBITSLOG = 3;
    bool Eof() const;
    size_t ReadBits(size_t cnt);
    void Read(char* arr, size_t size);

private:
    size_t file_end_ = BUFFSIZE * CHARBITS;
    std::ifstream stream_;
    size_t pos_ = MAXPOS;
    bool is_eof_ = false;
    char buff_[BUFFSIZE];
    void ReadBuff();
};
class BitWriter {
    size_t pos_ = 0;
    std::ofstream stream_;
    char buff_[BitReader::BUFFSIZE];

public:
    ~BitWriter();
    BitWriter();
    explicit BitWriter(const std::string& str);
    void Print(size_t s, size_t length);
    void FlushChars();
    void FlushAll();
    void Open(const std::string& path);
    void Close();
    void Write(char* arr, size_t size);
};