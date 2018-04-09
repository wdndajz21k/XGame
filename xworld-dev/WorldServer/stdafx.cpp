#include "stdafx.h"
#include "Player.h"

#ifdef _WDEBUG
#define new DEBUG_NEW
#endif

size_t GetPlayerConnId(XPlayer *player)
{
    return player->ConnId;
}