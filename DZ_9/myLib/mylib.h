#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>

struct FileInfo {
    std::filesystem::path path;
    std::uint64_t size = 0;
    std::string extension;
};

struct CategoryStats {
    std::uint64_t count = 0;
    std::uint64_t bytes = 0;
};

struct ScanResult {
    std::vector<FileInfo> files;

    CategoryStats txt;
    CategoryStats images;
    CategoryStats exe;
    CategoryStats other;

    std::uint64_t totalFiles = 0;
    std::uint64_t totalBytes = 0;
    std::uint64_t skippedEntries = 0;

    bool inputPathValid = false;
};

ScanResult scanDirectoryRecursive(const std::filesystem::path& root);

std::vector<FileInfo> filterTextFiles(const std::vector<FileInfo>& files);
std::vector<FileInfo> filterImageFiles(const std::vector<FileInfo>& files);
std::vector<FileInfo> filterExeFiles(const std::vector<FileInfo>& files);
std::vector<FileInfo> filterLargeFilesGiB(const std::vector<FileInfo>& files, std::uint64_t minGiB = 1);
std::vector<FileInfo> filterOtherFiles(const std::vector<FileInfo>& files);

void printSummary(const ScanResult& r);
void printFileList(const std::vector<FileInfo>& list, std::size_t limit = 200);
