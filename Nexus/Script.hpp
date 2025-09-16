#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>

#define SCRIPTDATA_COUNT (0x40000)
#define JUMPTABLE_COUNT  (0x4000)
#define FUNCTION_COUNT   (0x200)

#define JUMPSTACK_COUNT (0x400)
#define FUNCSTACK_COUNT (0x400)

struct ScriptPtr {
    int scriptCodePtr;
    int jumpTablePtr;
};

struct ScriptFunction {
    char name[0x20];
    ScriptPtr ptr;
};

struct ObjectScript {
    byte frameCount;
    byte spriteSheetID;
    ScriptPtr subMain;
    ScriptPtr subPlayerInteraction;
    ScriptPtr subDraw;
    ScriptPtr subStartup;
    SpriteFrame *frameStartPtr;
};

struct ScriptEngine {
    int operands[10];
    int tempValue[8];
    int arrayPosition[3];
    int checkResult;
    std::string strOperands[10];
    std::string tempStr[8];
};

enum ScriptSubs { SUB_MAIN = 0, SUB_PLAYERINTERACTION = 1, SUB_DRAW = 2, SUB_SETUP = 3, SUB_PLAYERMAIN = 4, SUB_PLAYERSTATE = 5 };

extern ObjectScript ObjectScriptList[OBJECT_COUNT];

extern ScriptFunction ScriptFunctionList[FUNCTION_COUNT];
extern int ScriptFunctionCount;

extern int ScriptData[SCRIPTDATA_COUNT];
extern int JumpTableData[JUMPTABLE_COUNT];

extern int JumpTableStack[JUMPSTACK_COUNT];
extern int FunctionStack[FUNCSTACK_COUNT];

extern int JumpTableStackPos;
extern int functionStackPos;

extern ScriptEngine ScriptEng;
extern char ScriptText[0x100];

extern int ScriptDataPos;
extern int scriptDataOffset;
extern int JumpTableDataPos;
extern int JumpTableOffset;

extern int NO_ALIASES;
extern int lineID;

void CheckAliasText(char *text);
void ConvertArithmaticSyntax(char *text);
void ConvertIfWhileStatement(char *text);
bool ConvertSwitchStatement(char *text);
void ConvertFunctionText(char *text);
void CheckCaseNumber(char *text);
bool ReadSwitchCase(char *text);
void AppendIntegerToString(char *text, int value);
bool ConvertStringToInteger(char *text, int *value);
void CopyAliasStr(char *dest, char *text, bool arrayIndex);
bool CheckOpcodeType(char *text); // Never actually used

void ParseScriptFile(char *scriptName, int scriptID);

void ProcessScript(int scriptCodePtr, int jumpTablePtr, byte scriptSub);

void ClearScriptData();

#endif // !SCRIPT_H
