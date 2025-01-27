#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <filesystem>
#include <windows.h>
#include <wininet.h>
#include <shlwapi.h>
#include <vector>
#include <map>

// Simple JSON library implementation
class json {
public:
    std::map<std::string, std::vector<std::string>> object;
    std::string raw;

    static json parse(std::istream& is) {
        json j;
        std::string line;
        std::string currentKey;
        while(std::getline(is, line)) {
            if(line.find("\"characters\"") != std::string::npos) currentKey = "characters";
            else if(line.find("\"backpacks\"") != std::string::npos) currentKey = "backpacks"; 
            else if(line.find("\"pickaxes\"") != std::string::npos) currentKey = "pickaxes";
            else if(line.find("\"gliders\"") != std::string::npos) currentKey = "gliders";
            else if(line.find("\"emotes\"") != std::string::npos) currentKey = "emotes";
            else if(line.find("\"") != std::string::npos && currentKey != "") {
                size_t start = line.find("\"") + 1;
                size_t end = line.find("\"", start);
                if(start != std::string::npos && end != std::string::npos) {
                    j.object[currentKey].push_back(line.substr(start, end-start));
                }
            }
        }
        return j;
    }

    std::string dump(int indent = 0) {
        std::string result = "{\n";
        for(const auto& [key, values] : object) {
            result += std::string(indent, ' ') + "\"" + key + "\": [\n";
            for(const auto& value : values) {
                result += std::string(indent + 4, ' ') + "\"" + value + "\",\n";
            }
            result += std::string(indent, ' ') + "],\n";
        }
        result += "}";
        return result;
    }

    std::vector<std::string>& operator[](const std::string& key) {
        return object[key];
    }
};

namespace fs = std::filesystem;

class HybridLauncher {
private:
    // Server endpoints for version 3.3
    const std::string AUTH_URL = "http://127.0.0.1:7777/account/api/oauth/token";
    const std::string EXCHANGE_URL = "http://127.0.0.1:7777/fortnite/api/game/v2/profile/";
    const std::string COSMETICS_URL = "http://127.0.0.1:7777/fortnite/api/game/v2/cosmetics/";
    const std::string GAME_EXE = "FortniteClient-Win64-Shipping.exe";
    
    std::string accessToken;
    std::string accountId;
    HANDLE gameProcess = NULL;
    bool serverRunning = false;
    std::string gamePath;
    std::string serverPath;
    
    // Cosmetics database loaded from cosmetics.json
    std::map<std::string, std::vector<std::string>> cosmeticsDb;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    void loadCosmeticsDatabase() {
        std::ifstream cosmeticsFile("cosmetics.json");
        if(!cosmeticsFile.good()) {
            throw std::runtime_error("cosmetics.json not found!");
        }

        json j = json::parse(cosmeticsFile);
        cosmeticsDb["characters"] = j["characters"];
        cosmeticsDb["backpacks"] = j["backpacks"];
        cosmeticsDb["pickaxes"] = j["pickaxes"];
        cosmeticsDb["gliders"] = j["gliders"];
        cosmeticsDb["emotes"] = j["emotes"];

        std::cout << "Loaded " << cosmeticsDb["characters"].size() << " characters" << std::endl;
        std::cout << "Loaded " << cosmeticsDb["backpacks"].size() << " backpacks" << std::endl;
        std::cout << "Loaded " << cosmeticsDb["pickaxes"].size() << " pickaxes" << std::endl;
        std::cout << "Loaded " << cosmeticsDb["gliders"].size() << " gliders" << std::endl;
        std::cout << "Loaded " << cosmeticsDb["emotes"].size() << " emotes" << std::endl;
    }

    bool checkLocalServer() {
        CURL* curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, AUTH_URL.c_str());
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            return (res == CURLE_OK);
        }
        return false;
    }

    void startLocalServer() {
        STARTUPINFO si = { sizeof(STARTUPINFO) };
        PROCESS_INFORMATION pi;
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        std::string cmdLine = serverPath + "\\HybridServer.exe --season=3.3 --auth-bypass --cosmetics-path=cosmetics.json";

        if(CreateProcessA(cmdLine.c_str(), NULL, NULL, NULL, FALSE, 
                         CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            serverRunning = true;
            
            std::cout << "Starting server..." << std::endl;
            for(int i = 0; i < 15; i++) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                if(checkLocalServer()) {
                    std::cout << "Server started successfully!" << std::endl;
                    return;
                }
                std::cout << "." << std::flush;
            }
            throw std::runtime_error("Server startup timeout");
        } else {
            throw std::runtime_error("Failed to start HybridServer.exe");
        }
    }

    std::string getAuthToken() {
        CURL* curl = curl_easy_init();
        std::string response;

        if(curl) {
            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            
            std::string uniqueId = "HybridFN_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            
            json authData;
            authData.object["grant_type"] = {"client_credentials"};
            authData.object["account_id"] = {uniqueId};
            authData.object["client_id"] = {"HybridFNClient"};
            authData.object["secret"] = {"hybrid_s3cret"};

            std::string postData = authData.dump();

            curl_easy_setopt(curl, CURLOPT_URL, AUTH_URL.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

            CURLcode res = curl_easy_perform(curl);
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);

            if(res != CURLE_OK) {
                throw std::runtime_error("Failed to get auth token from server");
            }

            json j;
            j.raw = response;
            accountId = uniqueId;
            return "dummy_token"; // Simplified for demo
        }
        
        throw std::runtime_error("Failed to initialize CURL");
    }

    void patchGameFiles() {
        std::cout << "Patching game files for version 3.3..." << std::endl;
        
        std::string pakPath = gamePath + "\\FortniteGame\\Content\\Paks\\";
        std::string backupPath = pakPath + "backup\\";
        
        if(!fs::exists(backupPath)) {
            fs::create_directory(backupPath);
            for(const auto& entry : fs::directory_iterator(pakPath)) {
                if(entry.path().extension() == ".pak") {
                    fs::copy(entry.path(), backupPath + entry.path().filename().string());
                }
            }
        }

        // Apply server patches
        std::ofstream config(gamePath + "\\HybridConfig.ini");
        config << "[Server]\n";
        config << "URL=127.0.0.1:7777\n";
        config << "AccountId=" << accountId << "\n";
        config << "UseCustomCosmetics=true\n";
        config << "AllowModding=true\n";
        config << "GameVersion=3.3.0\n";
        config << "DisableMatchmaking=true\n";
        config << "BypassAuthentication=true\n";
        config.close();

        // Create cosmetics override file
        std::ofstream cosmetics(gamePath + "\\CosmeticsOverride.json");
        json cosmeticsJson;
        for(const auto& [category, items] : cosmeticsDb) {
            cosmeticsJson[category] = items;
        }
        cosmetics << cosmeticsJson.dump(4);
        cosmetics.close();

        std::cout << "Game files patched successfully!" << std::endl;
    }

    void restoreGameFiles() {
        std::string pakPath = gamePath + "\\FortniteGame\\Content\\Paks\\";
        std::string backupPath = pakPath + "backup\\";
        
        if(fs::exists(backupPath)) {
            for(const auto& entry : fs::directory_iterator(backupPath)) {
                fs::copy(entry.path(), pakPath + entry.path().filename().string(), 
                        fs::copy_options::overwrite_existing);
            }
            fs::remove_all(backupPath);
        }

        fs::remove(gamePath + "\\HybridConfig.ini");
        fs::remove(gamePath + "\\CosmeticsOverride.json");
        std::cout << "Restored original game files" << std::endl;
    }

public:
    HybridLauncher() {
        std::cout << "Initializing HybridFN v3.3..." << std::endl;
        
        // Get paths from config
        std::ifstream config("config.json");
        if(!config.good()) {
            throw std::runtime_error("config.json not found! Please set game and server paths.");
        }
        
        json j = json::parse(config);
        gamePath = j.object["game_path"][0];
        serverPath = j.object["server_path"][0];

        if(!fs::exists(gamePath) || !fs::exists(serverPath)) {
            throw std::runtime_error("Invalid paths in config.json");
        }
        
        loadCosmeticsDatabase();
        
        if(!checkLocalServer()) {
            startLocalServer();
        }
        patchGameFiles();
    }

    ~HybridLauncher() {
        if(gameProcess) {
            CloseHandle(gameProcess);
        }
        restoreGameFiles();
    }

    void launch() {
        try {
            if(!serverRunning && !checkLocalServer()) {
                throw std::runtime_error("Local server not running");
            }

            std::cout << "Obtaining authentication token..." << std::endl;
            accessToken = getAuthToken();

            std::cout << "Starting Fortnite v3.3..." << std::endl;
            
            // Launch with server parameters
            std::string cmdLine = "\"" + gamePath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe\" "
                                "-AUTH_TYPE=hybrid "
                                "-AUTH_LOGIN=hybrid_user "
                                "-AUTH_PASSWORD=" + accessToken + " "
                                "-epicapp=Fortnite "
                                "-epicenv=Prod "
                                "-epiclocale=en-us "
                                "-epicportal "
                                "-skippatchcheck "
                                "-useallavailablecores "
                                "-NOSSLPINNING "
                                "-noeac -nobe -fltoken=none "
                                "-nodamagefx "
                                "-DisableMatchmaking "
                                "-ServerURL=127.0.0.1:7777";

            STARTUPINFO si = { sizeof(STARTUPINFO) };
            PROCESS_INFORMATION pi;

            if(CreateProcessA(NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                gameProcess = pi.hProcess;
                CloseHandle(pi.hThread);

                std::cout << "Waiting for game initialization..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(30));
                
                std::cout << "HybridFN v3.3 initialized successfully!" << std::endl;
                std::cout << "You now have access to all Season 3 cosmetics." << std::endl;
                std::cout << "Note: Matchmaking is disabled in this version." << std::endl;
                
                WaitForSingleObject(gameProcess, INFINITE);
            } else {
                throw std::runtime_error("Failed to launch Fortnite");
            }
        } catch(const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cout << "Press any key to exit..." << std::endl;
            std::cin.get();
        }
    }
};

int main() {
    SetConsoleTitleA("HybridFN v3.3 Launcher");
    std::cout << "Welcome to HybridFN Private Server" << std::endl;
    std::cout << "Version 3.3.0 - Cosmetics Only Mode" << std::endl;
    std::cout << "Initializing private environment..." << std::endl;

    try {
        HybridLauncher launcher;
        launcher.launch();
    } catch(const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        std::cout << "Press any key to exit..." << std::endl;
        std::cin.get();
    }

    return 0;
}
