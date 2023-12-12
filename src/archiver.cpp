#include "MyArchiver.h"
int main(int argc, char** argv) {
    std::vector<std::string> args;
    for (size_t i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    Archiver a;
    if (args.size() >= 3 && args[0] == "-c") {
        std::vector<std::string> filepaths;
        for (size_t j = 2; j < args.size(); ++j) {
            filepaths.push_back(args[j]);
        }
        a.ZipFiles(filepaths, args[1]);
        return 0;
    }
    if (args.size() == 2 && args[0] == "-d") {
        a.UnzipArchive(args[1]);
        return 0;
    }
    std::cout << "Help: \n -c archive_name file1 [file2 ...] zips file1, file2, ..., to archive archive_name \n";
    std::cout << "-d archive_name unzips archive archive_name into current dirrectory \n";
    std::cout << "-h prints Help \n";
}
