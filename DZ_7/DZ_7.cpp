#include <filesystem>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <vector>
#include <queue>
#include <ranges>
#include <unordered_set>
#include <limits>

namespace fs = std::filesystem;

struct FileInfo {
    fs::path path;
    std::uint64_t size = 0;
    std::string extension;
};

struct CategoryStats {
    std::uint64_t count = 0;
    std::uint64_t bytes = 0;
};

static void normalizeExtension(std::string& ext) {
    std::ranges::transform(
        ext,
        ext.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); }
    );
}

static bool isImageExt(const std::string& ext) {
    static const std::unordered_set<std::string> exts{
        ".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff"
    };
    return exts.contains(ext);
}

static void printCategory(const std::string& name, const CategoryStats& s) {
    std::cout << name << ":\n";
    std::cout << "  Files: " << s.count << "\n";
    std::cout << "  Bytes: " << s.bytes << "\n";
    std::cout << "  KB:    " << (s.bytes / 1024.0) << "\n";
    std::cout << "  MB:    " << (s.bytes / (1024.0 * 1024.0)) << "\n\n";
}

static void printSummary(const CategoryStats& txt,
    const CategoryStats& images,
    const CategoryStats& exe,
    const CategoryStats& other,
    std::uint64_t totalFiles,
    std::uint64_t totalBytes,
    std::uint64_t skippedEntries) {
    std::cout << "\n=== Summary ===\n";
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
        std::cout << "Skipped entries due to errors/permissions: " << skippedEntries << "\n\n";
    }
}

static void printMenu() {
    std::cout << "\n=== Filter menu ===\n";
    std::cout << "1) List text files (.txt)\n";
    std::cout << "2) List image files (.jpg .jpeg .png .bmp .gif .tiff)\n";
    std::cout << "3) List executables (.exe)\n";
    std::cout << "4) List large files (>= 1 GiB)\n";
    std::cout << "5) List other files (not txt/image/exe)\n";
    std::cout << "6) Show HW6 summary again\n";
    std::cout << "0) Exit\n";
    std::cout << "Choice: ";
}

static int readChoice() {
    int choice = -1;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    return choice;
}

static void printFileList(std::ranges::input_range auto&& view, std::size_t limit = 200) {
    std::size_t shown = 0;
    for (const auto& f : view) {
        std::cout << f.path.string() << " | " << f.size << " bytes | " << f.extension << "\n";
        if (++shown >= limit) {
            std::cout << "... (limited to " << limit << " items)\n";
            break;
        }
    }
    if (shown == 0) {
        std::cout << "(no files)\n";
    }
}

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

    std::uint64_t totalFiles = 0;
    std::uint64_t totalBytes = 0;
    std::uint64_t skippedEntries = 0;

    std::vector<FileInfo> files;
    files.reserve(4096);

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

        std::uint64_t size = static_cast<std::uint64_t>(entry.file_size(ec));
        if (ec) {
            ++skippedEntries;
            ec.clear();
            continue;
        }

        FileInfo info;
        info.path = entry.path();
        info.size = size;
        info.extension = info.path.extension().string();
        normalizeExtension(info.extension);

        files.push_back(std::move(info));

        ++totalFiles;
        totalBytes += size;

        const std::string& ext = files.back().extension;

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

    std::cout << "Directory: " << root.string() << "\n";
    std::cout << "Regular files scanned: " << files.size() << "\n";
    if (skippedEntries > 0) {
        std::cout << "Skipped entries due to errors/permissions: " << skippedEntries << "\n";
    }

    printSummary(txt, images, exe, other, totalFiles, totalBytes, skippedEntries);

    constexpr std::uint64_t GiB = 1024ULL * 1024ULL * 1024ULL;

    for (;;) {
        printMenu();
        int choice = readChoice();

        if (choice == 0) {
            std::cout << "Exiting.\n";
            break;
        }

        if (choice == 6) {
            printSummary(txt, images, exe, other, totalFiles, totalBytes, skippedEntries);
            continue;
        }

        if (choice == 1) {
            auto view = files | std::views::filter([](const FileInfo& f) {
                return f.extension == ".txt";
                });
            std::cout << "\n=== Text files (.txt) ===\n";
            printFileList(view);
            continue;
        }

        if (choice == 2) {
            auto view = files | std::views::filter([](const FileInfo& f) {
                return isImageExt(f.extension);
                });
            std::cout << "\n=== Image files ===\n";
            printFileList(view);
            continue;
        }

        if (choice == 3) {
            auto view = files | std::views::filter([](const FileInfo& f) {
                return f.extension == ".exe";
                });
            std::cout << "\n=== Executables (.exe) ===\n";
            printFileList(view);
            continue;
        }

        if (choice == 4) {
            auto view = files | std::views::filter([](const FileInfo& f) {
                return f.size >= GiB;
                });
            std::cout << "\n=== Large files (>= 1 GiB) ===\n";
            printFileList(view);
            continue;
        }

        if (choice == 5) {
            auto view = files | std::views::filter([](const FileInfo& f) {
                bool isTxt = (f.extension == ".txt");
                bool isImg = isImageExt(f.extension);
                bool isExe = (f.extension == ".exe");
                return !(isTxt || isImg || isExe);
                });
            std::cout << "\n=== Other files ===\n";
            printFileList(view);
            continue;
        }

        std::cout << "Invalid choice. Try again.\n";
    }

    std::cout << "\n=== Done ===\n";
    return 0;
}
