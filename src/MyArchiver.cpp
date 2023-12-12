#include "MyArchiver.h"
#define Elem std::pair<size_t, std::shared_ptr<BitBor::Node>>
const size_t Archiver::MAXSYMBOL = 258;
const size_t Archiver::FILENAME_END = 256;
const size_t Archiver::ONE_MORE_FILE = 257;
const size_t Archiver::ARCHIVE_END = 258;
class Compare {
public:
    bool operator()(Elem& e1, Elem& e2) {
        return (e1.first > e2.first) || (e1.first == e2.first && e1.second->val > e2.second->val);
    }
};
std::unordered_map<size_t, std::pair<size_t, size_t>> Archiver::CanonicalHuffmanEncode(
    std::vector<std::pair<size_t, size_t>>& vec) {
    std::sort(vec.begin(), vec.end());
    std::unordered_map<size_t, std::pair<size_t, size_t>> res;
    size_t curr = 0;
    for (size_t i = 0; i < vec.size(); ++i) {
        curr <<= (i ? (vec[i].first - vec[i - 1].first) : 0);
        res[vec[i].second] = {vec[i].first, curr};
        ++curr;
    }
    return res;
}
std::string Archiver::GetFileName(const std::string& filepath) {
    size_t pos = filepath.find_last_of('/');
    return (pos != std::string::npos) ? filepath.substr(pos + 1) : filepath;
}
void Archiver::BuildBorZip(std::unordered_map<size_t, size_t>& cnt, BitBor& b) {
    std::priority_queue<Elem, std::vector<Elem>, Compare> q;
    for (size_t i = 0; i <= MAXSYMBOL; ++i) {
        if (cnt[i]) {
            q.push(Elem{cnt[i], b.AddNode(i, nullptr, nullptr, true)});
        }
    }
    while (q.size() > 1) {
        auto v1 = q.top();
        q.pop();
        auto v2 = q.top();
        q.pop();
        q.push(Elem{v1.first + v2.first, b.AddNode(SIZE_MAX, v1.second, v2.second, false)});
    }
    b.root = q.top().second;
}
void Archiver::AddFile(const std::string& filepath, BitWriter& w, bool one_more_file) {
    std::unordered_map<size_t, size_t> cnt;
    for (size_t i = 0; i < MAXSYMBOL + 1; ++i) {
        cnt[i] = 0;
    }
    BitReader r;
    std::string filename = GetFileName(filepath);
    for (auto j : filename) {
        ++cnt[static_cast<size_t>(j)];
    }
    ++cnt[FILENAME_END];
    ++cnt[ONE_MORE_FILE];
    ++cnt[ARCHIVE_END];
    r.Open(filepath);
    size_t c = 0;
    while (!r.Eof()) {
        c = r.ReadBits(BitReader::CHARBITS);
        if (r.Eof()) {
            break;
        }
        ++cnt[c];
    }
    r.Close();
    BitBor b;
    BuildBorZip(cnt, b);
    std::vector<std::pair<size_t, size_t>> info;
    b.GetInfo(b.root, 0, info);
    auto canon = CanonicalHuffmanEncode(info);
    w.Print(info.size(), BitReader::CHARBITS + 1);
    for (size_t i = 0; i < info.size(); ++i) {
        w.Print(info[i].second, BitReader::CHARBITS + 1);
    }
    size_t index = 0;
    for (size_t len = 1; len <= info.back().first; ++len) {
        size_t cnt_symb = 0;
        for (; index < info.size() && info[index].first == len; ++index) {
            ++cnt_symb;
        }
        w.Print(cnt_symb, BitReader::CHARBITS + 1);
    }
    for (size_t i = 0; i < filename.size(); ++i) {
        w.Print(canon[filename[i]].second, canon[filename[i]].first);
    }
    w.Print(canon[FILENAME_END].second, canon[FILENAME_END].first);
    BitReader r1;
    r1.Open(filepath);
    while (!r1.Eof()) {
        c = r1.ReadBits(BitReader::CHARBITS);
        if (r1.Eof()) {
            break;
        }
        w.Print(canon[c].second, canon[c].first);
    }
    if (one_more_file) {
        w.Print(canon[ONE_MORE_FILE].second, canon[ONE_MORE_FILE].first);
    } else {
        w.Print(canon[ARCHIVE_END].second, canon[ARCHIVE_END].first);
    }
}
void Archiver::ZipFiles(const std::vector<std::string>& files, const std::string& str) {
    BitWriter w(str);
    for (size_t i = 0; i < files.size(); ++i) {
        AddFile(files[i], w, (i + 1 != files.size()));
    }
    w.FlushAll();
    w.Close();
}
void Archiver::BuildBorUnZip(std::vector<std::pair<size_t, size_t>>& alphabet, BitBor& b) {
    b.root = b.AddNode(0, nullptr, nullptr, false);
    std::stack<std::shared_ptr<BitBor::Node>> st;
    st.push(b.root);
    size_t curr = 0;
    for (size_t i = 0; i < alphabet.size(); ++i) {
        for (size_t j = 0; j < (i ? (alphabet[i].first - alphabet[i - 1].first) : alphabet[i].first); ++j) {
            st.top()->l = b.AddNode(0, nullptr, nullptr, false);
            st.push(st.top()->l);
            curr <<= 1;
        }
        st.top()->val = alphabet[i].second;
        st.top()->is_leaf = true;
        if (i + 1 == alphabet.size()) {
            break;
        }
        size_t add_length = 0;
        for (size_t j = 0; j < alphabet[i].first; ++j) {
            st.pop();
            ++add_length;
            if (!((static_cast<size_t>(1) << j) & curr)) {
                break;
            }
        }
        --add_length;
        ++curr;
        st.top()->r = b.AddNode(0, nullptr, nullptr, false);
        st.push(st.top()->r);
        for (size_t j = 0; j < add_length; ++j) {
            st.top()->l = b.AddNode(0, nullptr, nullptr, false);
            st.push(st.top()->l);
        }
    }
}
void Archiver::UnzipArchive(std::string archive_path) {
    BitReader r(archive_path);
    while (!r.Eof()) {
        size_t cnt = r.ReadBits(BitReader::CHARBITS + 1);
        std::vector<std::pair<size_t, size_t>> alphabet(cnt);
        for (size_t i = 0; i < cnt; ++i) {
            alphabet[i].second = r.ReadBits(BitReader::CHARBITS + 1);
        }
        size_t pos = 0;
        for (size_t j = 1; j <= alphabet.size() && pos < alphabet.size(); ++j) {
            size_t cnt_chars = r.ReadBits(BitReader::CHARBITS + 1);
            for (size_t k = 0; k < cnt_chars; ++k) {
                alphabet[pos + k].first = j;
            }
            pos += cnt_chars;
        }
        BitBor b;
        BuildBorUnZip(alphabet, b);
        std::shared_ptr<BitBor::Node> node = b.root;
        std::string filename;
        while (!r.Eof()) {
            node = b.Go(node, r.ReadBit());
            if (node->is_leaf) {
                if (node->val == FILENAME_END) {
                    break;
                }
                filename.push_back(node->val);
                node = b.root;
            }
        }
        node = b.root;
        BitWriter w(filename);
        bool one_more_file = false;
        while (!r.Eof()) {
            node = b.Go(node, r.ReadBit());
            if (node->is_leaf) {
                if (node->val == ONE_MORE_FILE) {
                    one_more_file = true;
                    break;
                }
                if (node->val == ARCHIVE_END) {
                    break;
                }
                w.Print(node->val, BitReader::CHARBITS);
                node = b.root;
            }
        }
        w.FlushChars();
        if (!one_more_file) {
            break;
        }
    }
}
