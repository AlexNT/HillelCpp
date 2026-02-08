#include <iostream>
#include <filesystem>
#include <limits>
#include "mylib.h"

static void printMenu() {
    std::cout << "\n=== Filter menu ===\n";
    std::cout << "1) List text files (.txt)\n";
    std::cout << "2) List image files\n";
    std::cout << "3) List executables (.exe)\n";
    std::cout << "4) List large files (>= 1 GiB)\n";
    std::cout << "5) List other files\n";
    std::cout << "6) Show summary\n";
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

int main(int argc, char* argv[]) {
    std::cout << "=== Filesystem analyzer ===\n\n";

    if (argc < 2) {
        std::cout << "Usage: " << (argc > 0 ? argv[0] : "app") << " <directory_path>\n";
        return 1;
    }

    std::filesystem::path root = std::filesystem::path(argv[1]);

    ScanResult r = scanDirectoryRecursive(root);

    if (!r.inputPathValid) {
        std::cout << "Error: path does not exist or is not a directory.\n";
        std::cout << "Given path: " << root.string() << "\n";
        return 2;
    }

    std::cout << "Directory: " << root.string() << "\n";
    std::cout << "Regular files scanned: " << r.files.size() << "\n";
    if (r.skippedEntries > 0) {
        std::cout << "Skipped entries due to errors/permissions: " << r.skippedEntries << "\n";
    }

    printSummary(r);

    for (;;) {
        printMenu();
        int choice = readChoice();

        if (choice == 0) {
            std::cout << "Exiting.\n";
            break;
        }

        switch (choice) {
            case 6: {
                printSummary(r);
                break;
            }
            case 1: {
                auto list = filterTextFiles(r.files);
                std::cout << "\n=== Text files (.txt) ===\n";
                printFileList(list);
                break;
            }
            case 2: {
                auto list = filterImageFiles(r.files);
                std::cout << "\n=== Image files ===\n";
                printFileList(list);
                break;
            }
            case 3: {
                auto list = filterExeFiles(r.files);
                std::cout << "\n=== Executables (.exe) ===\n";
                printFileList(list);
                break;
            }
            case 4: {
                auto list = filterLargeFilesGiB(r.files, 1);
                std::cout << "\n=== Large files (>= 1 GiB) ===\n";
                printFileList(list);
                break;
            }
            case 5: {
                auto list = filterOtherFiles(r.files);
                std::cout << "\n=== Other files ===\n";
                printFileList(list);
                break;
            }
            default: {
                std::cout << "Invalid choice. Try again.\n";
                break;
            }
        }
    }

    std::cout << "\n=== Done ===\n";
    return 0;
}
