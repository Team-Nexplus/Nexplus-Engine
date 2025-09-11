#include "RetroEngine.hpp"

TextMenu GameMenu[TEXTMENU_COUNT];
int TextMenuSurfaceNo = 0;

FontCharacter fontCharacterList[FONTCHAR_COUNT];

void SetupTextMenu(TextMenu *menu, int rowCount)
{
    menu->textDataPos = 0;
    menu->rowCount    = rowCount;
}
void AddTextMenuEntry(TextMenu *menu, const char *text)
{
    menu->entryStart[menu->rowCount] = menu->textDataPos;
    menu->entrySize[menu->rowCount]  = 0;
    for (int i = 0; i < StrLength(text); ++i) {
        menu->textData[menu->textDataPos] = text[i];
        if (menu->textData[menu->textDataPos] == ' ')
            menu->textData[menu->textDataPos] = 0;
        if (menu->textData[menu->textDataPos] > '/' && menu->textData[menu->textDataPos] < ':')
            menu->textData[menu->textDataPos] -= 21;
        if (menu->textData[menu->textDataPos] > '9' && menu->textData[menu->textDataPos] < 'f')
            menu->textData[menu->textDataPos] -= '@';
        ++menu->textDataPos;
        ++menu->entrySize[menu->rowCount];
    }
    menu->rowCount++;
}
void EditTextMenuEntry(TextMenu *menu, const char *text, int rowID)
{
    int pos                = menu->entryStart[rowID];
    menu->entrySize[rowID] = 0;
    for (int i = 0; i < StrLength(text); ++i) {
        menu->textData[pos] = text[i];
        if (menu->textData[pos] == ' ')
            menu->textData[pos] = 0;
        if (menu->textData[pos] > '/' && menu->textData[pos] < ':')
            menu->textData[pos] -= 21;
        if (menu->textData[pos] > '9' && menu->textData[pos] < 'f')
            menu->textData[pos] -= '@';
        ++menu->entrySize[rowID];
        pos++;
    }
}
void LoadConfigListText(TextMenu *menu, int listNo)
{
    FileInfo info;
    char strBuf[0x100];
    byte fileBuffer = 0;
    byte count      = 0;
    byte strLen     = 0;
    if (LoadFile("Data/Game/GameConfig.bin", &info)) {
        // Name
        FileRead(&strLen, 1);
        FileRead(&strBuf, strLen);
        strBuf[strLen] = 0;

        // 'Data'
        FileRead(&strLen, 1);
        FileRead(&strBuf, strLen);
        strBuf[strLen] = 0;

        // About
        FileRead(&strLen, 1);
        FileRead(&strBuf, strLen);
        strBuf[strLen] = 0;

        // Script Paths
        FileRead(&count, 1);
        for (int s = 0; s < count; ++s) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
        }

        // Variables
        FileRead(&count, 1);
        for (int v = 0; v < count; ++v) {
            // Var Name
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;

            // Var Value
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
            FileRead(&fileBuffer, 1);
        }

        // SFX
        FileRead(&count, 1);
        for (int s = 0; s < count; ++s) {
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
        }

        // Players
        FileRead(&count, 1);
        for (int p = 0; p < count; ++p) {
            // Player Anim
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
            // Player Script
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;
            // Player Name
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen);
            strBuf[strLen] = 0;

            if (listNo == 0) // Player List
				AddTextMenuEntry(menu, strBuf);
        }

        // Categories
        for (int c = 1; c <= 4; ++c) {
            byte stageCnt = 0;
            FileRead(&stageCnt, 1);
            for (int s = 0; s < stageCnt; ++s) {
                // Stage Folder
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = 0;

                // Stage ID
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = 0;

                // Stage Name
                FileRead(&strLen, 1);
                FileRead(&strBuf, strLen);
                strBuf[strLen] = '\0';

                // IsHighlighted
                FileRead(&fileBuffer, 1);
                if (listNo == c) {
                    AddTextMenuEntry(menu, strBuf);
                    menu->entryHighlight[menu->rowCount - 1] = fileBuffer;
                }
            }
        }
        CloseFile();
    }
}

void LoadTextFile(TextMenu *menu, const char *filePath, byte mapCode, bool loadVer){
	FileInfo info;
	
	char buffer[0x80];
	StrCopy(buffer, "Data/Strings/");
	StrAdd(buffer, filePath);
	filePath = buffer;
	
	byte fileBuffer = 0;
	if (LoadFile(filePath, &info)) {
		menu->textDataPos				= 0;
		menu->rowCount				   = 0;
		menu->entryStart[menu->rowCount] = menu->textDataPos;
		menu->entrySize[menu->rowCount]  = 0;

		if (!loadVer) {
			bool flag = false;
			FileRead(&fileBuffer, 1);
			if (fileBuffer == 0xFF) {
				FileRead(&fileBuffer, 1);
				while (!flag) {
					ushort val = 0;
					FileRead(&fileBuffer, 1);
					val = fileBuffer;
					FileRead(&fileBuffer, 1);
					val |= fileBuffer << 8;

					if (val != '\n') {
						if (val == '\r') {
							menu->rowCount += 1;
							if (menu->rowCount > 511) {
								flag = true;
							}
							else {
								menu->entryStart[menu->rowCount] = menu->textDataPos;
								menu->entrySize[menu->rowCount]  = 0;
							}
						}
						else {
							if (mapCode) {
								int i = 0;
								while (i < 1024) {
									if (fontCharacterList[i].id == val) {
										val = i;
										i   = 1025;
									}
									else {
										++i;
									}
								}
								if (i == 1024) {
									val = 0;
								}
							}
							menu->textData[menu->textDataPos++] = val;
							menu->entrySize[menu->rowCount]++;
						}
					}
					if (!flag) {
						flag = ReachedEndOfFile();
						if (menu->textDataPos >= TEXTDATA_COUNT)
							flag = true;
					}
				}
			}
			else {
				ushort val = fileBuffer;
				if (val != '\n') {
					if (val == '\r') {
						menu->rowCount++;
						menu->entryStart[menu->rowCount] = menu->textDataPos;
						menu->entrySize[menu->rowCount]  = 0;
					}
					else {
						if (mapCode) {
							int i = 0;
							while (i < 1024) {
								if (fontCharacterList[i].id == val) {
									val = i;
									i   = 1025;
								}
								else {
									++i;
								}
							}
							if (i == 1024) {
								val = 0;
							}
						}
						menu->textData[menu->textDataPos++] = val;
						menu->entrySize[menu->rowCount]++;
					}
				}

				while (!flag) {
					FileRead(&fileBuffer, 1);
					val = fileBuffer;
					if (val != '\n') {
						if (val == '\r') {
							menu->rowCount++;
							if (menu->rowCount > 511) {
								flag = true;
							}
							else {
								menu->entryStart[menu->rowCount] = menu->textDataPos;
								menu->entrySize[menu->rowCount]  = 0;
							}
						}
						else {
							if (mapCode) {
								int i = 0;
								while (i < 1024) {
									if (fontCharacterList[i].id == val) {
										val = i;
										i   = 1025;
									}
									else {
										++i;
									}
								}
								if (i == 1024)
									val = 0;
							}
							menu->textData[menu->textDataPos++] = val;
							menu->entrySize[menu->rowCount]++;
						}
					}
					if (!flag) {
						flag = ReachedEndOfFile();
						if (menu->textDataPos >= TEXTDATA_COUNT)
							flag = true;
					}
				}
			}
		} else {
			while (menu->textDataPos < TEXTDATA_COUNT && !ReachedEndOfFile()) {
				FileRead(&fileBuffer, 1);
//				if (fileBuffer != '\n') {
					if (fileBuffer == '\n') {
						menu->rowCount++;
						menu->entryStart[menu->rowCount] = menu->textDataPos;
						menu->entrySize[menu->rowCount]  = 0;
					}
					else {
//						menu->textData[menu->textDataPos++] = (fileBuffer - 0x40); //For text to appear correctly with the debug font
						menu->textData[menu->textDataPos++] = fileBuffer;
						menu->entrySize[menu->rowCount]++;
					}
//				}
			}
		}

		menu->rowCount++;
		CloseFile();
	}
}