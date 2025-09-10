#ifndef MOD_API_H
#define MOD_API_H

#if RETRO_USE_MOD_LOADER
#include <string>
#include <map>
#include <unordered_map>

struct ModInfo {
    std::string name;
    std::string desc;
    std::string author;
    std::string version;
    std::map<std::string, std::string> fileMap;
    std::string folder;
    bool redirectSave;
    std::string savePath;
    bool active;
};

extern std::vector<ModInfo> modList;
extern int activeMod;
extern char modsPath[0x100];

extern bool redirectSave;

inline void SetActiveMod(int id) { activeMod = id; }

void InitMods();
bool LoadMod(ModInfo *info, std::string modsPath, std::string folder, bool active);
void ScanModFolder(ModInfo *info);
void SaveMods();

void RefreshEngine();

#endif

#if RETRO_USE_MOD_LOADER || !RETRO_USE_ORIGINAL_CODE
extern char savePath[0x100];
#endif

#endif