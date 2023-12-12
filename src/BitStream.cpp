#include "BitStream.h"
void BitReader::ReadBuff() {
    pos_ = 0;
    stream_.read(buff_, BUFFSIZE);
    if (BUFFSIZE != stream_.gcount()) {
        file_end_ = stream_.gcount() * CHARBITS;
    }
}
bool BitReader::ReadBit() {
    if (pos_ == MAXPOS) {
        ReadBuff();
    }
    if (pos_ == file_end_) {
        is_eof_ = true;
        return false;
    }
    bool res = (buff_[pos_ >> CHARBITSLOG] & (1 << (CHARBITS - (pos_ & (CHARBITS - 1)) - 1))) > 0;
    ++pos_;
    return res;
}
BitReader::BitReader() {
}
BitReader::BitReader(const std::string& str) {
    stream_.open(str, std::ios::binary);
    if (!stream_.is_open()) {
        throw std::runtime_error("Error, cannot open file for reading.");
    }
}
void BitReader::Open(const std::string& path) {
    pos_ = MAXPOS;
    file_end_ = BUFFSIZE * CHARBITS;
    is_eof_ = false;
    stream_.open(path, std::ios::binary);
    if (!stream_.is_open()) {
        throw std::runtime_error("Error, cannot open file for reading.");
    }
}
void BitReader::Close() {
    stream_.close();
}
bool BitReader::Eof() const {
    return is_eof_;
}
size_t BitReader::ReadBits(size_t cnt) {
    size_t res = 0;
    size_t st = (static_cast<size_t>(1) << (cnt - 1));
    for (size_t i = 0; i < cnt; ++i) {
        if (ReadBit()) {
            res += st;
        }
        if (Eof()) {
            return 0;
        }
        st >>= 1;
    }
    return res;
}
void BitReader::Read(char* arr, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        arr[i] = static_cast<char>(ReadBits(CHARBITS));
    }
}
BitWriter::~BitWriter() {
    Close();
};
BitWriter::BitWriter() {
}
BitWriter::BitWriter(const std::string& str) {
    for (size_t i = 0; i < BitReader::BUFFSIZE; ++i) {
        buff_[i] = 0;
    }
    pos_ = 0;
    stream_.open(str, std::ios::binary);
    if (!stream_.is_open()) {
        throw std::runtime_error("Error, cannot open file for writing.");
    }
}
void BitWriter::Print(size_t s, size_t length) {
    for (size_t i = length; i > 0; --i) {
        if (pos_ == BitReader::MAXPOS) {
            FlushChars();
        }
        buff_[pos_ >> BitReader::CHARBITSLOG] <<= 1;
        if (s & (static_cast<size_t>(1) << (i - 1))) {
            ++buff_[pos_ >> BitReader::CHARBITSLOG];
        }
        ++pos_;
    }
}
void BitWriter::FlushChars() {
    stream_.write(buff_, static_cast<int64_t>(pos_ >> BitReader::CHARBITSLOG));
    stream_.flush();
    for (size_t i = 0; i < (pos_ >> BitReader::CHARBITSLOG); ++i) {
        buff_[i] = 0;
    }
    if ((pos_ & (BitReader::CHARBITS - 1)) && pos_ >= BitReader::CHARBITS) {
        std::swap(buff_[0], buff_[pos_ >> BitReader::CHARBITSLOG]);
    }
    pos_ &= (BitReader::CHARBITS - 1);
}
void BitWriter::FlushAll() {
    FlushChars();
    if (pos_) {
        for (size_t j = 0; j < BitReader::CHARBITS - pos_; ++j) {
            buff_[0] <<= 1;
        }
        stream_.write(buff_, 1);
        stream_.flush();
        buff_[0] = 0;
        pos_ = 0;
    }
}
void BitWriter::Open(const std::string& path) {
    for (size_t i = 0; i < BitReader::BUFFSIZE; ++i) {
        buff_[i] = 0;
    }
    pos_ = 0;
    stream_.open(path, std::ios::binary);
    if (!stream_.is_open()) {
        throw std::runtime_error("Error, cannot open file for writing.");
    }
}
void BitWriter::Close() {
    FlushAll();
    stream_.close();
}
void BitWriter::Write(char* arr, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        Print(arr[i], BitReader::CHARBITS);
    }
}