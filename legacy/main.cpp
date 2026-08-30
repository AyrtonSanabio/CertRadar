#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>

#include <algorithm>
#include <cctype>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

namespace {

const unsigned long max_directories = 100000UL;

std::string join_path(const std::string& parent, const std::string& child) {
    if (parent.empty()) return child;
    const char last = parent[parent.size() - 1];
    return last == '\\' || last == '/' ? parent + child : parent + "\\" + child;
}

bool is_directory(const std::string& path) {
    const DWORD attributes = GetFileAttributesA(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool equals_ignore_case(const std::string& left, const std::string& right) {
    if (left.size() != right.size()) return false;
    for (std::string::size_type index = 0; index < left.size(); ++index) {
        const unsigned char left_character = static_cast<unsigned char>(left[index]);
        const unsigned char right_character = static_cast<unsigned char>(right[index]);
        if (std::tolower(left_character) != std::tolower(right_character)) return false;
    }
    return true;
}

bool contains_path(const std::vector<std::string>& paths, const std::string& candidate) {
    for (std::vector<std::string>::const_iterator item = paths.begin(); item != paths.end(); ++item) {
        if (equals_ignore_case(*item, candidate)) return true;
    }
    return false;
}

bool has_container_extension(const std::string& path) {
    const std::string::size_type dot = path.find_last_of('.');
    if (dot == std::string::npos) return false;
    std::string extension = path.substr(dot);
    for (std::string::size_type index = 0; index < extension.size(); ++index) {
        extension[index] = static_cast<char>(
            std::tolower(static_cast<unsigned char>(extension[index])));
    }
    return extension == ".pfx" || extension == ".p12";
}

void append_unique_directory(std::vector<std::string>& roots, const std::string& path) {
    if (!path.empty() && is_directory(path) && !contains_path(roots, path)) roots.push_back(path);
}

std::vector<std::string> default_roots() {
    std::vector<std::string> roots;
    char profile[MAX_PATH] = {0};
    const DWORD profile_length = GetEnvironmentVariableA("USERPROFILE", profile, MAX_PATH);
    if (profile_length > 0 && profile_length < MAX_PATH) {
        append_unique_directory(roots, join_path(profile, "Downloads"));
    }

    char known_folder[MAX_PATH] = {0};
    if (SUCCEEDED(SHGetFolderPathA(
            0, CSIDL_DESKTOPDIRECTORY, 0, 0, known_folder))) {
        append_unique_directory(roots, known_folder);
    }
    known_folder[0] = '\0';
    if (SUCCEEDED(SHGetFolderPathA(0, CSIDL_PERSONAL, 0, 0, known_folder))) {
        append_unique_directory(roots, known_folder);
    }
    if (profile_length > 0 && profile_length < MAX_PATH) append_unique_directory(roots, profile);
    return roots;
}

void scan_root(
    const std::string& root,
    std::vector<std::string>& candidates,
    unsigned long& directories_visited,
    unsigned long& inaccessible_directories) {
    if (!is_directory(root)) {
        ++inaccessible_directories;
        return;
    }

    std::deque<std::string> pending;
    pending.push_back(root);
    while (!pending.empty() && directories_visited < max_directories) {
        const std::string current = pending.front();
        pending.pop_front();
        ++directories_visited;
        if (current.size() + 3 >= MAX_PATH) {
            ++inaccessible_directories;
            continue;
        }

        WIN32_FIND_DATAA entry;
        const HANDLE search = FindFirstFileA(join_path(current, "*").c_str(), &entry);
        if (search == INVALID_HANDLE_VALUE) {
            ++inaccessible_directories;
            continue;
        }
        do {
            const std::string name(entry.cFileName);
            if (name == "." || name == "..") continue;
            const std::string path = join_path(current, name);
            if ((entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                if ((entry.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) == 0 &&
                    path.size() < MAX_PATH) {
                    pending.push_back(path);
                }
            } else if (has_container_extension(name) && !contains_path(candidates, path)) {
                candidates.push_back(path);
            }
        } while (FindNextFileA(search, &entry) != 0);
        FindClose(search);
    }
}

void print_platform() {
    OSVERSIONINFOA version;
    ZeroMemory(&version, sizeof(version));
    version.dwOSVersionInfoSize = sizeof(version);
    if (GetVersionExA(&version) != 0) {
        std::cout << "Windows " << version.dwMajorVersion << '.' << version.dwMinorVersion
                  << " build " << version.dwBuildNumber << "\n";
    } else {
        std::cout << "Versao do Windows indisponivel\n";
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    std::cout << "CertRadar Legacy 0.1.0 - diagnostico basico, somente leitura\n";
    std::cout << "AVISO: Windows XP nao recebe atualizacoes de seguranca.\n";
    print_platform();

    std::vector<std::string> roots;
    if (argc == 3 && std::string(argv[1]) == "--scan-root") {
        append_unique_directory(roots, argv[2]);
    } else if (argc == 1) {
        roots = default_roots();
    } else {
        std::cerr << "Uso: certradar-legacy.exe [--scan-root CAMINHO]\n";
        return 2;
    }

    if (roots.empty()) {
        std::cerr << "Nenhuma pasta acessivel foi encontrada para busca.\n";
        return 1;
    }

    std::vector<std::string> candidates;
    unsigned long directories_visited = 0;
    unsigned long inaccessible_directories = 0;
    for (std::vector<std::string>::const_iterator root = roots.begin(); root != roots.end(); ++root) {
        scan_root(*root, candidates, directories_visited, inaccessible_directories);
    }
    std::sort(candidates.begin(), candidates.end());
    for (std::vector<std::string>::const_iterator candidate = candidates.begin();
         candidate != candidates.end(); ++candidate) {
        std::cout << "[candidato por extensao] " << *candidate << "\n";
    }
    std::cout << candidates.size() << " candidato(s); " << directories_visited
              << " pasta(s) visitada(s); " << inaccessible_directories
              << " pasta(s) inacessivel(is).\n";
    std::cout << "Nenhum arquivo foi aberto, copiado ou alterado.\n";
    return 0;
}
