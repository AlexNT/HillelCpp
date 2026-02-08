#include "mylib.h"

#include <iostream>
#include <ranges>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <system_error>

namespace fs = std::filesystem;

static void normalizeExtension(std::string& ext) {
    std::ranges::transform(ext, ext.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
}

static bool isImageExt(const std::string& ext) {
    static const std::unordered_set<std::string> exts{
        ".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff"
    };
    return exts.contains(ext);
}

static void addToCategory(ScanResult& r, const FileInfo& f) {
    ++r.totalFiles;
    r.totalBytes += f.size;

    if (f.extension == ".txt") {
        ++r.txt.count;
        r.txt.bytes += f.size;
        return;
    }

    if (isImageExt(f.extension)) {
        ++r.images.count;
        r.images.bytes += f.size;
        return;
    }

    if (f.extension == ".exe") {
        ++r.exe.count;
        r.exe.bytes += f.size;
        return;
    }

    ++r.other.count;
    r.other.bytes += f.size;
}

ScanResult scanDirectoryRecursive(const fs::path& root) {
    ScanResult r;

    std::error_code ec;
    r.inputPathValid = fs::exists(root, ec) && fs::is_directory(root, ec);
    if (!r.inputPathValid) {
        return r;
    }

    fs::recursive_directory_iterator it(
        root,
        fs::directory_options::skip_permission_denied,
        ec
    );

    if (ec) {
        return r;
    }

    for (const auto& entry : it) {
        if (ec) {
            ++r.skippedEntries;
            ec.clear();
            continue;
        }

        if (!entry.is_regular_file(ec)) {
            if (ec) ec.clear();
            continue;
        }

        std::uint64_t size = static_cast<std::uint64_t>(entry.file_size(ec));
        if (ec) {
            ++r.skippedEntries;
            ec.clear();
            continue;
        }

        FileInfo info;
        info.path = entry.path();
        info.size = size;
        info.extension = info.path.extension().string();
        normalizeExtension(info.extension);

        r.files.push_back(std::move(info));
        addToCategory(r, r.files.back());
    }

    return r;
}

std::vector<FileInfo> filterTextFiles(const std::vector<FileInfo>& files) {
    auto view = files | std::views::filter([](const FileInfo& f) { return f.extension == ".txt"; });
    return {view.begin(), view.end()};
}

std::vector<FileInfo> filterImageFiles(const std::vector<FileInfo>& files) {
    auto view = files | std::views::filter([](const FileInfo& f) { return isImageExt(f.extension); });
    return {view.begin(), view.end()};
}

std::vector<FileInfo> filterExeFiles(const std::vector<FileInfo>& files) {
    auto view = files | std::views::filter([](const FileInfo& f) { return f.extension == ".exe"; });
    return {view.begin(), view.end()};
}

std::vector<FileInfo> filterLargeFilesGiB(const std::vector<FileInfo>& files, std::uint64_t minGiB) {
    const std::uint64_t threshold = minGiB * 1024ULL * 1024ULL * 1024ULL;
    auto view = files | std::views::filter([&](const FileInfo& f) { return f.size >= threshold; });
    return {view.begin(), view.end()};
}

std::vector<FileInfo> filterOtherFiles(const std::vector<FileInfo>& files) {
    auto view = files | std::views::filter([](const FileInfo& f) {
        bool isTxt = (f.extension == ".txt");
        bool isImg = isImageExt(f.extension);
        bool isExe = (f.extension == ".exe");
        return !(isTxt || isImg || isExe);
    });
    return {view.begin(), view.end()};
}

static void printCategory(const std::string& name, const CategoryStats& s) {
    std::cout << name << ":\n";
    std::cout << "  Files: " << s.count << "\n";
    std::cout << "  Bytes: " << s.bytes << "\n";
    std::cout << "  KB:    " << (s.bytes / 1024.0) << "\n";
    std::cout << "  MB:    " << (s.bytes / (1024.0 * 1024.0)) << "\n\n";
}

void printSummary(const ScanResult& r) {
    std::cout << "\n=== Summary ===\n";
    printCategory("Text files (.txt)", r.txt);
    printCategory("Images (.jpg .jpeg .png .bmp .gif .tiff)", r.images);
    printCategory("Executables (.exe)", r.exe);
    printCategory("Other files", r.other);

    std::cout << "Totals:\n";
    std::cout << "  Files: " << r.totalFiles << "\n";
    std::cout << "  Bytes: " << r.totalBytes << "\n";
    std::cout << "  MB:    " << (r.totalBytes / (1024.0 * 1024.0)) << "\n\n";

    if (r.skippedEntries > 0) {
        std::cout << "Skipped entries due to errors/permissions: " << r.skippedEntries << "\n\n";
    }
}

void printFileList(const std::vector<FileInfo>& list, std::size_t limit) {
    std::size_t shown = 0;
    for (const auto& f : list) {
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
