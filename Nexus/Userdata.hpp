#ifndef USERDATA_H
#define USERDATA_H

#define GLOBALVAR_COUNT (0x100)

#define SAVEDATA_SIZE (0x2000)

extern int NO_GLOBALVARIABLES;
extern int GlobalVariables[GLOBALVAR_COUNT];
extern char GlobalVariableNames[GLOBALVAR_COUNT][0x20];

extern char gamePath[0x100];
extern int saveRAM[SAVEDATA_SIZE];

inline int GetGlobalVariableByName(const char *name)
{
    for (int v = 0; v < NO_GLOBALVARIABLES; ++v) {
        if (StrComp(name, GlobalVariableNames[v]))
            return GlobalVariables[v];
    }
    return 0;
}

inline void SetGlobalVariableByName(const char *name, int value)
{
    for (int v = 0; v < NO_GLOBALVARIABLES; ++v) {
        if (StrComp(name, GlobalVariableNames[v])) {
            GlobalVariables[v] = value;
            break;
        }
    }
}

extern bool useSGame;
bool ReadSaveRAMData();
bool WriteSaveRAMData();

void InitUserdata();
void WriteSettings();

#endif //!USERDATA_H
