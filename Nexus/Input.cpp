#include "RetroEngine.hpp"

#include <algorithm>
#include <vector>

InputData GKeyPress = InputData();
InputData GKeyDown  = InputData();

bool anyPress = false;

int touchDown[8];
int touchX[8];
int touchY[8];
int touchID[8];
int touches = 0;

#if !RETRO_USE_ORIGINAL_CODE
#include <algorithm>
#include <vector>

InputButton inputDevice[INPUT_MAX];
int inputType = 0;

// mania deadzone vals lol
float LSTICK_DEADZONE   = 0.3;
float RSTICK_DEADZONE   = 0.3;
float LTRIGGER_DEADZONE = 0.3;
float RTRIGGER_DEADZONE = 0.3;

int mouseHideTimer = 0;
int lastMouseX     = 0;
int lastMouseY     = 0;

#if RETRO_USING_SDL2
std::vector<SDL_GameController *> controllers;
#endif

#if RETRO_USING_SDL1
byte keyState[SDLK_LAST];

SDL_Joystick *controller = nullptr;
#endif

#define normalize(val, minVal, maxVal) ((float)(val) - (float)(minVal)) / ((float)(maxVal) - (float)(minVal))

#if RETRO_USING_SDL2
bool getControllerButton(byte buttonID) {
    bool pressed = false;

    for (int i = 0; i < controllers.size(); ++i) {
        SDL_GameController *controller = controllers[i];

        if (SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)buttonID)) {
            pressed |= true;
            continue;
        } else {
            switch (buttonID) {
                default: break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                    float delta = 0;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta < -LSTICK_DEADZONE;
                    continue;
                }
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                    float delta = 0;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta > LSTICK_DEADZONE;
                    continue;
                }
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                    float delta = 0;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta < -LSTICK_DEADZONE;
                    continue;
                }
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: {
                    int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                    float delta = 0;
                    if (axis < 0)
                        delta = -normalize(-axis, 1, 32768);
                    else
                        delta = normalize(axis, 0, 32767);
                    pressed |= delta > LSTICK_DEADZONE;
                    continue;
                }
            }
        }

        switch (buttonID) {
            default: break;
            case SDL_CONTROLLER_BUTTON_ZL: {
                float delta = normalize(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT), 0, 32767);
                pressed |= delta > LTRIGGER_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_ZR: {
                float delta = normalize(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT), 0, 32767);
                pressed |= delta > RTRIGGER_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_UP: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -LSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_DOWN: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > LSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_LEFT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > LSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_LSTICK_RIGHT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -LSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_UP: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -RSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_DOWN: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > RSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_LEFT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta > RSTICK_DEADZONE;
                continue;
            }
            case SDL_CONTROLLER_BUTTON_RSTICK_RIGHT: {
                int axis    = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
                float delta = 0;
                if (axis < 0)
                    delta = -normalize(-axis, 1, 32768);
                else
                    delta = normalize(axis, 0, 32767);
                pressed |= delta < -RSTICK_DEADZONE;
                continue;
            }
        }
    }

    return pressed;
}
#endif

void controllerInit(byte controllerID) {
    SDL_GameController *controller = SDL_GameControllerOpen(controllerID);
    if (controller) {
        controllers.push_back(controller);
        inputType = 1;
    }
}

void controllerClose(byte controllerID) {
    SDL_GameController *controller = SDL_GameControllerFromInstanceID(controllerID);
    if (controller) {
        SDL_GameControllerClose(controller);
        controllers.erase(std::remove(controllers.begin(), controllers.end(), controller), controllers.end());
    }

    if (controllers.empty()) {
        inputType = 0;
    }
}

void ReadInputDevice() {
#if RETRO_USING_SDL2
    int length           = 0;
    const byte *keyState = SDL_GetKeyboardState(&length);

    if (inputType == 0) {
        for (int i = 0; i < INPUT_ANY; i++) {
            if (keyState[inputDevice[i].keyMappings]) {
                inputDevice[i].setHeld();
                if (!inputDevice[INPUT_ANY].hold)
                    inputDevice[INPUT_ANY].setHeld();
            } else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    } else if (inputType == 1) {
        for (int i = 0; i < INPUT_ANY; i++) {
            if (getControllerButton(inputDevice[i].contMappings)) {
                inputDevice[i].setHeld();
                if (!inputDevice[INPUT_ANY].hold)
                    inputDevice[INPUT_ANY].setHeld();
            } else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }

    bool isPressed = false;
    for (int i = 0; i < INPUT_MAX; i++) {
        if (keyState[inputDevice[i].keyMappings]) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType = 0;
    else if (inputType == 0)
        inputDevice[INPUT_ANY].setReleased();

    isPressed = false;
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        if (getControllerButton(i)) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType = 1;
    else if (inputType == 1)
        inputDevice[INPUT_ANY].setReleased();

#ifdef RETRO_USING_MOUSE
    if (touches <= 0) {
		int mx = 0, my = 0;
		SDL_GetMouseState(&mx, &my);

		if ((mx == lastMouseX && my == lastMouseY)) {
		    ++mouseHideTimer;
		    if (mouseHideTimer == 120) {
		        SDL_ShowCursor(false);
		    }
		} else {
		    if (mouseHideTimer >= 120)
		        SDL_ShowCursor(true);
		    mouseHideTimer = 0;
		}

		lastMouseX = mx;
		lastMouseY = my;
    }
#endif //! RETRO_USING_MOUSE

#elif RETRO_USING_SDL1
    if (SDL_NumJoysticks() > 0) {
        controller = SDL_JoystickOpen(0);

        // There's a problem opening the joystick
        if (controller == NULL) {
            // Uh oh
        } else {
            inputType = 1;
        }
    } else {
        if (controller) {
            // Close the joystick
            SDL_JoystickClose(controller);
        }
        controller = nullptr;
        inputType  = 0;
    }

    if (inputType == 0) {
        for (int i = 0; i < INPUT_MAX; i++) {
            if (keyState[inputDevice[i].keyMappings]) {
                inputDevice[i].setHeld();
                inputDevice[INPUT_ANY].setHeld();
                continue;
            } else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    } else if (inputType == 1 && controller) {
        for (int i = 0; i < INPUT_MAX; i++) {
            if (SDL_JoystickGetButton(controller, inputDevice[i].contMappings)) {
                inputDevice[i].setHeld();
                inputDevice[INPUT_ANY].setHeld();
                continue;
            } else if (inputDevice[i].hold)
                inputDevice[i].setReleased();
        }
    }

    bool isPressed = false;
    for (int i = 0; i < INPUT_MAX; i++) {
        if (keyState[inputDevice[i].keyMappings]) {
            isPressed = true;
            break;
        }
    }
    if (isPressed)
        inputType = 0;
    else if (inputType == 0)
        inputDevice[INPUT_ANY].setReleased();

    int buttonCnt = 0;
    if (controller)
        buttonCnt = SDL_JoystickNumButtons(controller);
    bool flag = false;
    for (int i = 0; i < buttonCnt; ++i) {
        flag      = true;
        inputType = 1;
    }
    if (!flag && inputType == 1) {
        inputDevice[INPUT_ANY].setReleased();
    }
#endif
}
#endif

void CheckKeyPress(InputData *input, byte flags) {
	input->up = inputDevice[INPUT_UP].press;
	input->down = inputDevice[INPUT_DOWN].press;
	input->left = inputDevice[INPUT_LEFT].press;
	input->right = inputDevice[INPUT_RIGHT].press;
	input->A = inputDevice[INPUT_BUTTONA].press;
	input->B = inputDevice[INPUT_BUTTONB].press;
	input->C = inputDevice[INPUT_BUTTONC].press;
	input->X = inputDevice[INPUT_BUTTONX].press;
	input->Y = inputDevice[INPUT_BUTTONY].press;
	input->Z = inputDevice[INPUT_BUTTONZ].press;
	input->L = inputDevice[INPUT_BUTTONL].press;
	input->R = inputDevice[INPUT_BUTTONR].press;
	input->start = inputDevice[INPUT_START].press;
	input->select = inputDevice[INPUT_SELECT].press;
	anyPress = inputDevice[INPUT_ANY].press;
    if (!anyPress) {
        for (int t = 0; t < touches; ++t) {
            if (touchDown[t])
                anyPress = true;
        }
    }
}

void CheckKeyDown(InputData *input, byte flags) {
	input->up = inputDevice[INPUT_UP].hold;
	input->down = inputDevice[INPUT_DOWN].hold;
	input->left = inputDevice[INPUT_LEFT].hold;
	input->right = inputDevice[INPUT_RIGHT].hold;
	input->A = inputDevice[INPUT_BUTTONA].hold;
	input->B = inputDevice[INPUT_BUTTONB].hold;
	input->C = inputDevice[INPUT_BUTTONC].hold;
	input->X = inputDevice[INPUT_BUTTONX].hold;
	input->Y = inputDevice[INPUT_BUTTONY].hold;
	input->Z = inputDevice[INPUT_BUTTONZ].hold;
	input->L = inputDevice[INPUT_BUTTONL].hold;
	input->R = inputDevice[INPUT_BUTTONR].hold;
	input->start = inputDevice[INPUT_START].hold;
	input->select = inputDevice[INPUT_SELECT].hold;
}
