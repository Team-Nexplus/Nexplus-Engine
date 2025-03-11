#include "RetroEngine.hpp"

Player PlayerList[PLAYER_COUNT];
PlayerScript PlayerScriptList[PLAYER_COUNT];
int PlayerNo                 = 0;
int activePlayerCount        = 1;
int PlayerListPos            = 0;

ushort DelayUp           = 0;
ushort DelayDown         = 0;
ushort DelayLeft         = 0;
ushort DelayRight        = 0;
ushort DelayJumpPress    = 0;
ushort DelayJumpHold     = 0;
ushort DelaytriggerPress = 0;
ushort DelaytriggerHold  = 0;
ushort DelaybumperPress  = 0;
ushort DelaybumperHold   = 0;
ushort DelayaPress       = 0;
ushort DelayaHold        = 0;
ushort DelaybPress       = 0;
ushort DelaybHold        = 0;
ushort DelaycPress       = 0;
ushort DelaycHold        = 0;
ushort DelayxPress       = 0;
ushort DelayxHold        = 0;
ushort DelayyPress       = 0;
ushort DelayyHold        = 0;
ushort DelayzPress       = 0;
ushort DelayzHold        = 0;
ushort DelaylPress       = 0;
ushort DelaylHold        = 0;
ushort DelayrPress       = 0;
ushort DelayrHold        = 0;

void LoadPlayerFromList(byte characterID, byte playerID) {
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
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen); // player anim file
            strBuf[strLen] = '\0';

            FileRead(&strLen, 1);
            FileRead(&PlayerScriptList[p].scriptPath, strLen); // player script file
            PlayerScriptList[p].scriptPath[strLen] = '\0';

            if (characterID == p) {
                GetFileInfo(&info);
                CloseFile();
#if RETRO_USE_MOD_LOADER
                strcpy(PlayerScriptList[playerID].scriptPath, PlayerScriptList[p].scriptPath);
#endif
                LoadPlayerAnimation(strBuf, playerID);
                SetFileInfo(&info);
            }
            FileRead(&strLen, 1);
            FileRead(&strBuf, strLen); // player name
            strBuf[strLen] = '\0';
        }
        CloseFile();
    }
}

void ProcessPlayerAnimationChange(Player *player) {
    if (player->animation != player->prevAnimation) {
		if (GetGlobalVariableByName("GAME_NEXPLUS")) {
			if (player->jumpHitboxOffset == 1)
				player->YPos += (PlayerCBoxes[0].bottom[0] - PlayerCBoxes[1].bottom[0]) << 16;
			else
				player->YPos -= (PlayerCBoxes[0].bottom[0] - PlayerCBoxes[1].bottom[0]) << 16;
		} else {
			if (player->animation == ANI_JUMPING)
				player->YPos += (PlayerCBoxes[0].bottom[0] - PlayerCBoxes[1].bottom[0]) << 16;
			if (player->prevAnimation == ANI_JUMPING)
				player->YPos -= (PlayerCBoxes[0].bottom[0] - PlayerCBoxes[1].bottom[0]) << 16;
		}
        player->prevAnimation  = player->animation;
        player->frame          = 0;
        player->animationTimer = 0;
    }
}

void DrawPlayer(Player *player, SpriteFrame *frame) {
	if (!GetGlobalVariableByName("GAME_NEXPLUS")) {
		player->rotate = 0;
		switch (player->animation) {
		    case ANI_RUNNING:
		    case ANI_WALKING:
		    case ANI_PEELOUT:
		    case ANI_CORKSCREW:
		        if (player->rotation >= 0x80) {
		            player->rotate = 0x200 - ((266 - player->rotation) >> 5 << 6);
		            }
		        else {
		            player->rotate = (player->rotation + 10) >> 5 << 6;
		            }
				player->rotate = player->rotation * 2;
		        break;
		    default: break;
		}
	}
    DrawRotatedSprite(player->direction, player->screenXPos, player->screenYPos, -frame->pivotX, -frame->pivotY, frame->sprX, frame->sprY,
                      frame->width, frame->height, player->rotate, frame->sheetID);
}

void ProcessPlayerControl(Player *Player) {
    if (Player->controlMode == CONTROLMODE_NONE) {
        DelayUp <<= 1;
        DelayUp |= (byte)Player->up;
        DelayDown <<= 1;
        DelayDown |= (byte)Player->down;
        DelayLeft <<= 1;
        DelayLeft |= (byte)Player->left;
        DelayRight <<= 1;
        DelayRight |= (byte)Player->right;
        DelayJumpPress <<= 1;
        DelayJumpPress |= (byte)Player->jumpPress;
        DelayJumpHold <<= 1;
        DelayJumpHold |= (byte)Player->jumpHold;
        DelaytriggerPress <<= 1;
        DelaytriggerPress |= (byte)Player->triggerPress;
        DelaytriggerHold <<= 1;
        DelaytriggerHold |= (byte)Player->triggerHold;
        DelaybumperPress <<= 1;
        DelaybumperPress |= (byte)Player->bumperPress;
        DelaybumperHold <<= 1;
        DelaybumperHold |= (byte)Player->bumperHold;
        DelayaPress <<= 1;
        DelayaPress |= (byte)Player->aPress;
        DelayaHold <<= 1;
        DelayaHold |= (byte)Player->aHold;
        DelaybPress <<= 1;
        DelaybPress |= (byte)Player->bPress;
        DelaybHold <<= 1;
        DelaybHold |= (byte)Player->bHold;
        DelaycPress <<= 1;
        DelaycPress |= (byte)Player->cPress;
        DelaycHold <<= 1;
        DelaycHold |= (byte)Player->cHold;
        DelayxPress <<= 1;
        DelayxPress |= (byte)Player->xPress;
        DelayxHold <<= 1;
        DelayxHold |= (byte)Player->xHold;
        DelayyPress <<= 1;
        DelayyPress |= (byte)Player->yPress;
        DelayyHold <<= 1;
        DelayyHold |= (byte)Player->yHold;
        DelayzPress <<= 1;
        DelayzPress |= (byte)Player->zPress;
        DelayzHold <<= 1;
        DelayzHold |= (byte)Player->zHold;
        DelaylPress <<= 1;
        DelaylPress |= (byte)Player->lPress;
        DelaylHold <<= 1;
        DelaylHold |= (byte)Player->lHold;
        DelayrPress <<= 1;
        DelayrPress |= (byte)Player->rPress;
        DelayrHold <<= 1;
        DelayrHold |= (byte)Player->rHold;
    } else if (Player->controlMode == CONTROLMODE_SIDEKICK) {
        Player->up           = DelayUp >> 15;
        Player->down         = DelayDown >> 15;
        Player->left         = DelayLeft >> 15;
        Player->right        = DelayRight >> 15;
        Player->jumpPress    = DelayJumpPress >> 15;
        Player->jumpHold     = DelayJumpHold >> 15;
        Player->triggerPress = DelaytriggerPress >> 15;
        Player->triggerHold  = DelaytriggerHold >> 15;
        Player->bumperPress  = DelaybumperPress >> 15;
        Player->bumperHold   = DelaybumperHold >> 15;
        Player->aPress = DelayaPress >> 15;
        Player->aHold  = DelayaHold >> 15;
        Player->bPress = DelaybPress >> 15;
        Player->bHold  = DelaybHold >> 15;
        Player->cPress = DelaycPress >> 15;
        Player->cHold  = DelaycHold >> 15;
        Player->xPress = DelayxPress >> 15;
        Player->xHold  = DelayxHold >> 15;
        Player->yPress = DelayyPress >> 15;
        Player->yHold  = DelayyHold >> 15;
        Player->zPress = DelayzPress >> 15;
        Player->zHold  = DelayzHold >> 15;
        Player->lPress = DelaylPress >> 15;
        Player->lHold  = DelaylHold >> 15;
        Player->rPress = DelayrPress >> 15;
        Player->rHold  = DelayrHold >> 15;
    } else if (Player->controlMode == CONTROLMODE_NORMAL) {
        Player->up   = GKeyDown.up;
        Player->down = GKeyDown.down;
        if (!GKeyDown.left || !GKeyDown.right) {
            Player->left  = GKeyDown.left;
            Player->right = GKeyDown.right;
        } else {
            Player->left  = false;
            Player->right = false;
        }
        Player->jumpPress = GKeyPress.C | GKeyPress.B | GKeyPress.A;
        Player->jumpHold  = GKeyDown.C | GKeyDown.B | GKeyDown.A;
        Player->triggerPress = GKeyPress.L | GKeyPress.R;
        Player->triggerHold  = GKeyDown.L | GKeyDown.R;
        Player->bumperPress = GKeyPress.X | GKeyPress.Z;
        Player->bumperHold  = GKeyDown.X | GKeyDown.Z;
        Player->aPress = GKeyPress.A;
        Player->aHold  = GKeyDown.A;
        Player->bPress = GKeyPress.B;
        Player->bHold  = GKeyDown.B;
        Player->cPress = GKeyPress.C;
        Player->cHold  = GKeyDown.C;
        Player->xPress = GKeyPress.X;
        Player->xHold  = GKeyDown.X;
        Player->yPress = GKeyPress.Y;
        Player->yHold  = GKeyDown.Y;
        Player->zPress = GKeyPress.Z;
        Player->zHold  = GKeyDown.Z;
        Player->lPress = GKeyPress.L;
        Player->lHold  = GKeyDown.L;
        Player->rPress = GKeyPress.R;
        Player->rHold  = GKeyDown.R;
        DelayUp <<= 1;
        DelayUp |= (byte)Player->up;
        DelayDown <<= 1;
        DelayDown |= (byte)Player->down;
        DelayLeft <<= 1;
        DelayLeft |= (byte)Player->left;
        DelayRight <<= 1;
        DelayRight |= (byte)Player->right;
        DelayJumpPress <<= 1;
        DelayJumpPress |= (byte)Player->jumpPress;
        DelayJumpHold <<= 1;
        DelayJumpHold |= (byte)Player->jumpHold;
        DelaytriggerPress <<= 1;
        DelaytriggerPress |= (byte)Player->triggerPress;
        DelaytriggerHold <<= 1;
        DelaytriggerHold |= (byte)Player->triggerHold;
        DelaybumperPress <<= 1;
        DelaybumperPress |= (byte)Player->bumperPress;
        DelaybumperHold <<= 1;
        DelaybumperHold |= (byte)Player->bumperHold;
        DelayaPress <<= 1;
        DelayaPress |= (byte)Player->aPress;
        DelayaHold <<= 1;
        DelayaHold |= (byte)Player->aHold;
        DelaybPress <<= 1;
        DelaybPress |= (byte)Player->bPress;
        DelaybHold <<= 1;
        DelaybHold |= (byte)Player->bHold;
        DelaycPress <<= 1;
        DelaycPress |= (byte)Player->cPress;
        DelaycHold <<= 1;
        DelaycHold |= (byte)Player->cHold;
        DelayxPress <<= 1;
        DelayxPress |= (byte)Player->xPress;
        DelayxHold <<= 1;
        DelayxHold |= (byte)Player->xHold;
        DelayyPress <<= 1;
        DelayyPress |= (byte)Player->yPress;
        DelayyHold <<= 1;
        DelayyHold |= (byte)Player->yHold;
        DelayzPress <<= 1;
        DelayzPress |= (byte)Player->zPress;
        DelayzHold <<= 1;
        DelayzHold |= (byte)Player->zHold;
        DelaylPress <<= 1;
        DelaylPress |= (byte)Player->lPress;
        DelaylHold <<= 1;
        DelaylHold |= (byte)Player->lHold;
        DelayrPress <<= 1;
        DelayrPress |= (byte)Player->rPress;
        DelayrHold <<= 1;
        DelayrHold |= (byte)Player->rHold;
    }
}

void SetMovementStats(PlayerMovementStats *stats) {
    stats->topSpeed            = 0x60000;
    stats->acceleration        = 0xC00;
    stats->deceleration        = 0xC00;
    stats->airAcceleration     = 0x1800;
    stats->airDeceleration     = 0x600;
    stats->gravityStrength     = 0x3800;
    stats->jumpStrength        = 0x68000;
    stats->rollingDeceleration = 0x2000;
}

void ProcessDefaultAirMovement(Player *player) {
    if (player->speed <= -player->stats.topSpeed) {
        if (player->left)
            player->direction = FLIP_X;
    } else if (player->left) {
        player->speed -= player->stats.airAcceleration;
        player->direction = FLIP_X;
    }
    if (player->speed >= player->stats.topSpeed) {
        if (player->right)
            player->direction = FLIP_NONE;
    } else if (player->right) {
        player->speed += player->stats.airAcceleration;
        player->direction = FLIP_NONE;
    }

    if (player->YVelocity > -0x40001 && player->YVelocity < 1)
        player->speed -= player->speed >> 5;
}

void ProcessDefaultGravityFalse(Player *player) {
    player->trackScroll = false;
    player->XVelocity   = (player->speed * CosValue256[player->angle]) >> 8;
    player->YVelocity   = (player->speed * SinValue256[player->angle]) >> 8;
}

void ProcessDefaultGravityTrue(Player *player) {
    player->trackScroll = true;
    player->YVelocity += player->stats.gravityStrength;
    if (player->YVelocity >= -0x33CB0) {
        player->timer = 0;
    } else if (!player->jumpHold && player->timer > 0) {
        player->timer     = 0;
        player->YVelocity = -0x3C800;
        player->speed -= player->speed >> 5;
    }
    player->XVelocity = player->speed;
    if (player->rotation <= 0 || player->rotation >= 128) {
        if (player->rotation > 127 && player->rotation < 256) {
            player->rotation += 2;
            if (player->rotation > 255) {
                player->rotation = 0;
            }
        }
    } else {
        player->rotation -= 2;
        if (player->rotation < 1)
            player->rotation = 0;
    }
}

void ProcessDefaultGroundMovement(Player *player) {
    if ((signed int)player->frictionLoss <= 0) {
        if (player->left && player->speed > -player->stats.topSpeed) {
            if (player->speed <= 0) {
                player->speed -= player->stats.acceleration;
                player->skidding = 0;
            } else {
                if (player->speed > 0x40000)
                    player->skidding = 16;
                if (player->speed >= 0x8000) {
                    player->speed -= 0x8000;
                } else {
                    player->speed    = -0x8000;
                    player->skidding = 0;
                }
            }
        }
        if (player->right && player->speed < player->stats.topSpeed) {
            if (player->speed >= 0) {
                player->speed += player->stats.acceleration;
                player->skidding = 0;
            } else {
                if (player->speed < -0x40000)
                    player->skidding = 16;
                if (player->speed <= -0x8000) {
                    player->speed += 0x8000;
                } else {
                    player->speed    = 0x8000;
                    player->skidding = 0;
                }
            }
        }

        if (player->left && player->speed <= 0)
            player->direction = FLIP_X;
        if (player->right && player->speed >= 0)
            player->direction = FLIP_NONE;

        if (player->left || player->right) {
            switch (player->collisionMode) {
                case CMODE_FLOOR: player->speed += SinValue256[player->angle] << 13 >> 8; break;
                case CMODE_LWALL:
                    if (player->angle >= 176) {
                        player->speed += (SinValue256[player->angle] << 13 >> 8);
                    } else {
                        if (player->speed < -0x60000 || player->speed > 0x60000)
                            player->speed += SinValue256[player->angle] << 13 >> 8;
                        else
                            player->speed += 0x1400 * SinValue256[player->angle] >> 8;
                    }
                    break;
                case CMODE_ROOF:
                    if (player->speed < -0x60000 || player->speed > 0x60000)
                        player->speed += SinValue256[player->angle] << 13 >> 8;
                    else
                        player->speed += 0x1400 * SinValue256[player->angle] >> 8;
                    break;
                case CMODE_RWALL:
                    if (player->angle <= 80) {
                        player->speed += SinValue256[player->angle] << 13 >> 8;
                    } else {
                        if (player->speed < -0x60000 || player->speed > 0x60000)
                            player->speed += SinValue256[player->angle] << 13 >> 8;
                        else
                            player->speed += 0x1400 * SinValue256[player->angle] >> 8;
                    }
                    break;
                default: break;
            }

            if (player->angle > 192) {
                if (player->angle < 226 && !player->left) {
                    if (player->right && player->speed < 0x20000) {
                        if (player->speed > -0x60000)
                            player->frictionLoss = 30;
                    }
                }
            }
            if (player->angle > 30) {
                if (player->angle < 64 && player->left) {
                    if (!player->right && player->speed > -0x20000) {
                        if (player->speed < 0x60000)
                            player->frictionLoss = 30;
                    }
                }
            }
        } else {
            if (player->speed < 0) {
                player->speed += player->stats.deceleration;
                if (player->speed > 0)
                    player->speed = 0;
            }
            if (player->speed > 0) {
                player->speed -= player->stats.deceleration;
                if (player->speed < 0)
                    player->speed = 0;
            }
            if (player->speed < -0x4000 || player->speed > 0x4000)
                player->speed += SinValue256[player->angle] << 13 >> 8;
            if ((player->angle > 30 && player->angle < 64) || (player->angle > 192 && player->angle < 226)) {
                if (player->speed > -0x10000 && player->speed < 0x10000)
                    player->frictionLoss = 30;
            }
        }
    } else {
        --player->frictionLoss;
        player->speed = (SinValue256[player->angle] << 13 >> 8) + player->speed;
    }
}

void ProcessDefaultJumpAction(Player *player) {
    player->frictionLoss  = 0;
    player->gravity       = true;
    player->XVelocity     = (player->speed * CosValue256[player->angle] + player->stats.jumpStrength * SinValue256[player->angle]) >> 8;
    player->YVelocity     = (player->speed * SinValue256[player->angle] + -player->stats.jumpStrength * CosValue256[player->angle]) >> 8;
    player->speed         = player->XVelocity;
    player->trackScroll   = true;
    player->animation     = ANI_JUMPING;
    player->angle         = 0;
    player->collisionMode = CMODE_FLOOR;
    player->timer         = 1;
}

void ProcessDefaultRollingMovement(Player *player) {
    if (player->right && player->speed < 0)
        player->speed += player->stats.rollingDeceleration;
    if (player->left && player->speed > 0)
        player->speed -= player->stats.rollingDeceleration;

    if (player->speed < 0) {
        player->speed += player->stats.airDeceleration;
        if (player->speed > 0)
            player->speed = 0;
    }
    if (player->speed > 0) {
        player->speed -= player->stats.airDeceleration;
        if (player->speed < 0)
            player->speed = 0;
    }
    if ((player->angle < 12 || player->angle > 244) && !player->speed)
        player->state = 0;

    if (player->speed <= 0) {
        if (SinValue256[player->angle] >= 0) {
            player->speed += (player->stats.rollingDeceleration * SinValue256[player->angle] >> 8);
        } else {
            player->speed += 0x5000 * SinValue256[player->angle] >> 8;
        }
    } else if (SinValue256[player->angle] <= 0) {
        player->speed += (player->stats.rollingDeceleration * SinValue256[player->angle] >> 8);
    } else {
        player->speed += 0x5000 * SinValue256[player->angle] >> 8;
    }

    if (player->speed > 0x180000)
        player->speed = 0x180000;
}

void ProcessDebugMode(Player *player) {
    if (player->down || player->up || player->right || player->left) {
        if (player->speed < 0x100000) {
            player->speed += 0xC00;
            if (player->speed > 0x100000)
                player->speed = 0x100000;
        }
    } else {
        player->speed = 0;
    }

    if (GKeyDown.left)
        player->XPos -= player->speed;
    if (GKeyDown.right)
        player->XPos += player->speed;

    if (GKeyDown.up)
        player->YPos -= player->speed;
    if (GKeyDown.down)
        player->YPos += player->speed;
}

void ProcessPlayerAnimation(Player *player) {
    PlayerScript *script = &PlayerScriptList[player->type];
    if (!player->gravity) {
		int speed = (player->jumpingSpeed * abs(player->speed) / 6 >> 16) + 48;
		if (!GetGlobalVariableByName("GAME_NEXPLUS")) {
		    if (speed > 0xF0)
		        speed = 0xF0;
		    script->animations[ANI_JUMPING].speed = speed;
		}

		if (!GetGlobalVariableByName("GAME_NEXPLUS")) {
		    switch (player->animation) {
		        case ANI_WALKING: script->animations[player->animation].speed = ((uint)(player->walkingSpeed * abs(player->speed) / 6) >> 16) + 20; break;
		        case ANI_RUNNING:
		            speed = player->runningSpeed * abs(player->speed) / 6 >> 16;
		            if (speed > 0xF0)
		                speed = 0xF0;
		            script->animations[player->animation].speed = speed;
		            break;
		        case ANI_PEELOUT:
		            speed = player->runningSpeed * abs(player->speed) / 6 >> 16;
		            if (speed > 0xF0)
		                speed = 0xF0;
		            script->animations[player->animation].speed = speed;
		            break;
		    }
		} else {
			if (player->animSpeedMode != 0) {
		            speed = (player->animSpeedMode * abs(player->speed) / 6 >> 16) + player->animSpeedOffset;
		            if (player->animSpeedCap != 0) {
				        if (speed > player->animSpeedCap)
				            speed = player->animSpeedCap;
				    }
		            script->animations[player->animation].speed = speed;
		    }
		}
    }
    if (player->animationSpeed)
        player->animationTimer += player->animationSpeed;
    else
        player->animationTimer += script->animations[player->animation].speed;
    if (player->animation != player->prevAnimation) {
		if (!GetGlobalVariableByName("GAME_NEXPLUS")) {
			if (player->jumpHitboxOffset == 1)
				player->YPos += (PlayerCBoxes[0].bottom[0] - PlayerCBoxes[1].bottom[0]) << 16;
			else
				player->YPos -= (PlayerCBoxes[0].bottom[0] - PlayerCBoxes[1].bottom[0]) << 16;
		} else {
			if (player->animation == ANI_JUMPING)
				player->YPos += (PlayerCBoxes[0].bottom[0] - PlayerCBoxes[1].bottom[0]) << 16;
			if (player->prevAnimation == ANI_JUMPING)
				player->YPos -= (PlayerCBoxes[0].bottom[0] - PlayerCBoxes[1].bottom[0]) << 16;
		}
        player->prevAnimation  = player->animation;
        player->frame          = 0;
        player->animationTimer = 0;
    }
    if (player->animationTimer >= 0xF0) {
        player->animationTimer -= 0xF0;
        ++player->frame;
    }
    if (player->frame == script->animations[player->animation].frameCount) {
        player->frame = script->animations[player->animation].loopPoint;
    }
}