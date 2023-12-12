#include "algorithm"
#include "BitBor.h"
#include "BitStream.h"
#include <queue>
#include <unordered_map>
#include <stack>
class Archiver {
public:
    static const size_t MAXSYMBOL;
    static const size_t FILENAME_END;
    static const size_t ONE_MORE_FILE;
    static const size_t ARCHIVE_END;
    std::unordered_map<size_t, std::pair<size_t, size_t>> CanonicalHuffmanEncode(
        std::vector<std::pair<size_t, size_t>>& vec);
    std::string GetFileName(const std::string& filepath);
    void BuildBorZip(std::unordered_map<size_t, size_t>& cnt, BitBor& b);
    void AddFile(const std::string& file, BitWriter& w, bool one_more_file);
    void ZipFiles(const std::vector<std::string>& files, const std::string& str);
    void BuildBorUnZip(std::vector<std::pair<size_t, size_t>>& alphabet, BitBor& b);
    void UnzipArchive(std::string archive_path);
};
