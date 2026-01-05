#include <filesystem>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdint>

namespace fs = std::filesystem;

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

struct CategoryStats {
    std::uint64_t count = 0;
    std::uint64_t bytes = 0;
};

int main(int argc, char* argv[]) {
    std::cout << "=== Filesystem directory analyzer ===\n\n";

    if (argc < 2) {
        std::cout << "Usage: " << (argc > 0 ? argv[0] : "analyzer") << " <directory_path>\n";
        return 1;
    }

    fs::path root = fs::path(argv[1]);

    std::error_code ec;
    if (!fs::exists(root, ec) || !fs::is_directory(root, ec)) {
        std::cout << "Error: path does not exist or is not a directory.\n";
        std::cout << "Given path: " << root.string() << "\n";
        return 2;
    }

    CategoryStats txt;
    CategoryStats images;
    CategoryStats exe;
    CategoryStats other;

    const auto isImageExt = [](const std::string& ext) {
        return ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".gif" || ext == ".tiff";
        };

    std::uint64_t totalFiles = 0;
    std::uint64_t totalBytes = 0;
    std::uint64_t skippedEntries = 0;

    fs::recursive_directory_iterator it(
        root,
        fs::directory_options::skip_permission_denied,
        ec
    );

    if (ec) {
        std::cout << "Error: cannot iterate directory. " << ec.message() << "\n";
        return 3;
    }

    for (const auto& entry : it) {
        if (ec) {
            ++skippedEntries;
            ec.clear();
            continue;
        }

        if (!entry.is_regular_file(ec)) {
            if (ec) ec.clear();
            continue;
        }

        ++totalFiles;

        std::uint64_t size = 0;
        size = static_cast<std::uint64_t>(entry.file_size(ec));
        if (ec) {
            ++skippedEntries;
            ec.clear();
            continue;
        }

        totalBytes += size;

        std::string ext = toLower(entry.path().extension().string());

        if (ext == ".txt") {
            ++txt.count;
            txt.bytes += size;
        }
        else if (isImageExt(ext)) {
            ++images.count;
            images.bytes += size;
        }
        else if (ext == ".exe") {
            ++exe.count;
            exe.bytes += size;
        }
        else {
            ++other.count;
            other.bytes += size;
        }
    }

    auto printCategory = [](const std::string& name, const CategoryStats& s) {
        std::cout << name << ":\n";
        std::cout << "  Files: " << s.count << "\n";
        std::cout << "  Bytes: " << s.bytes << "\n";
        std::cout << "  KB:    " << (s.bytes / 1024.0) << "\n";
        std::cout << "  MB:    " << (s.bytes / (1024.0 * 1024.0)) << "\n\n";
        };

    std::cout << "Directory: " << root.string() << "\n\n";

    printCategory("Text files (.txt)", txt);
    printCategory("Images (.jpg .jpeg .png .bmp .gif .tiff)", images);
    printCategory("Executables (.exe)", exe);
    printCategory("Other files", other);

    std::cout << "Totals:\n";
    std::cout << "  Files: " << totalFiles << "\n";
    std::cout << "  Bytes: " << totalBytes << "\n";
    std::cout << "  KB:    " << (totalBytes / 1024.0) << "\n";
    std::cout << "  MB:    " << (totalBytes / (1024.0 * 1024.0)) << "\n\n";

    if (skippedEntries > 0) {
        std::cout << "Skipped entries due to errors/permissions: " << skippedEntries << "\n";
    }

    std::cout << "=== Done ===\n";
    return 0;
}
