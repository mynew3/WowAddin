#include "stdafx.h"
#include <sstream>
#include <list>
#include <vector>
#include <map>

BOOL CCommand_TestCommand(char const* cmd, char const* args)
{
    Console::Write("Hello from TestCommand: cmd '%s', args '%s'", INPUT_COLOR, cmd, args);

    uint64 guid = ObjectMgr::GetActivePlayerGuid();

	if (!guid)
	{
		Console::Write("TestCommand: Not in world!", ERROR_COLOR);
		return TRUE;
	}

    CGObject_C *pPlayer = ObjectMgr::GetObjectPtr(guid, TYPEMASK_PLAYER);

    C3Vector pos;
    pPlayer->GetPosition(pos);

    const char *model;
    BOOL result = pPlayer->GetModelFileName(&model);

    Console::Write("Local player: position x: %f y: %f z: %f, model '%s'", ECHO_COLOR, pos.X, pos.Y, pos.Z, result ? model : "Unknown");

    CGObject_C *pTarget = ObjectMgr::GetObjectPtr(pPlayer->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_UNIT);

    if (pTarget)
        Console::Write("Target '%s', guid 0x%016llX", ECHO_COLOR, pTarget->GetObjectName(), pTarget->GetValue<uint64>(OBJECT_FIELD_GUID));
    else
        Console::Write("No target!", ECHO_COLOR);

    return TRUE;
}

BOOL CCommand_Beastmaster(char const* cmd, char const* args)
{
    CDataStore data(CMSG_BEASTMASTER);
    int state = _strnicmp(args, "off", INT_MAX) != 0;
    data.PutInt8(state);
    ClientServices::SendPacket(&data);

    Console::Write("Beastmaster mode is %s", ECHO_COLOR, state ? "on" : "off");

    return TRUE;
}

BOOL CCommand_Invis(char const* cmd, char const* args)
{
    CDataStore data(CMSG_GM_INVIS);
    int state = _strnicmp(args, "off", INT_MAX) != 0;
    data.PutInt32(state);
    ClientServices::SendPacket(&data);

    Console::Write("GM invis mode is %s", ECHO_COLOR, state ? "on" : "off");

    return TRUE;
}

BOOL CCommand_DBLookup(char const* cmd, char const* args)
{
    CDataStore data(CMSG_DBLOOKUP);
    data.PutString(args);
    ClientServices::SendPacket(&data);

    return TRUE;
}

BOOL ShowObjectsEnumProc(uint64 objectGuid, void *param)
{
    CGObject_C *pObject = ObjectMgr::GetObjectPtr(objectGuid, TYPEMASK_OBJECT);

    if (pObject)
		Console::Write("Object '%s', guid 0x%016llX", HIGHLIGHT_COLOR, pObject->GetObjectName(), pObject->GetValue<uint64>(OBJECT_FIELD_GUID));

    return TRUE;
}

BOOL CCommand_ShowObjects(char const* cmd, char const* args)
{
	if (!ObjectMgr::IsInWorld())
	{
		Console::Write("Error: Not in world!", ERROR_COLOR);
		return TRUE;
	}

    ObjectMgr::EnumVisibleObjects(ShowObjectsEnumProc, NULL);

    return TRUE;
}

BOOL CCommand_CreateGuildCommand(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_GUILD_CREATE);
    std::string guildname = std::string(args);
    data.PutString(guildname.c_str());
    ClientServices::SendPacket(&data);

    std::ostringstream ss;
    ss << "Sent CMSG_GUILD_CREATE with guildname " << guildname.c_str() << ". Args: " << args;
    Console::Write(ss.str().c_str(), ECHO_COLOR);
    return true;
}

BOOL CCommand_SendIgnoreCommand(char const* cmd, char const* args)
{
    //! Send '<name> is ignoring you.' to target X times (this message can't be blocked).
    long floodCount = atoi(args);

    for (long i = 0; i < floodCount; ++i)
    {
        CDataStore data;
        data.PutInt32(CMSG_CHAT_IGNORED);

        if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
        {
            CGObject_C* target = ObjectMgr::GetObjectPtr(player->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_PLAYER);

            if (!target)
                target = ObjectMgr::GetObjectPtr(player->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_UNIT);

            if (target)
                data.PutInt64(target->GetValue<uint64>(OBJECT_FIELD_GUID)); // guid
            else
                data.PutInt64(ObjectMgr::GetActivePlayerGuid()); // guid (just send own, we need to send one..)
        }
        else
            data.PutInt64(ObjectMgr::GetActivePlayerGuid()); // guid (just send own, we need to send one..)

        data.PutInt8(1); // unk
        

        ClientServices::SendPacket(&data);
        Console::Write("CMSG_CHAT_IGNORED", ECHO_COLOR);
    }
    return true;
}

BOOL CCommand_LootRollCommand(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_LOOT_ROLL);

    std::string itemGuid = strtok((char*)args, " ");
    std::string lootType = strtok(NULL, " "); // loottype, 0=pass, 1=need, 2=greed

    data.PutInt64(int64(std::atof(itemGuid.c_str()))); // itemguid to roll for
    data.PutInt32(1); // itemslot, unused

    data.PutInt8(int8(std::atof(lootType.c_str())));
    
    ClientServices::SendPacket(&data);

    std::stringstream ss;
    ss << "CMSG_LOOT_ROLL with guid " << itemGuid << " and loot type " << lootType;
    Console::Write(ss.str().c_str(), ECHO_COLOR);
    return true;
}

BOOL CCommand_CharRenameCommand(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_CHAR_RENAME);
    data.PutInt64(ObjectMgr::GetActivePlayerGuid());
    std::string newName = strtok((char*)args, " ");
    data.PutString(newName.c_str());
    
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_CHAR_RENAME", ECHO_COLOR);
    return true;
}

BOOL CCommand_TeleportCommand(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_WORLD_TELEPORT);

    std::string mapid = strtok((char*)args, " ");
    std::string posX = strtok(NULL, " ");
    std::string posY = strtok(NULL, " ");
    std::string posZ = strtok(NULL, " ");

    data.PutInt32(50); // time in ms, nfi what its used for
    data.PutInt32(int32(std::atof(mapid.c_str())));
    data.PutFloat(float(std::atof(posX.c_str())));
    data.PutFloat(float(std::atof(posY.c_str())));
    data.PutFloat(float(std::atof(posZ.c_str())));
    data.PutInt32(int32(std::atof(mapid.c_str())));
    data.PutFloat(3.141593f); // orientation
    
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_WORLD_TELEPORT", ECHO_COLOR);
    return true;
}

BOOL CCommand_CreateCharCommand(char const* cmd, char const* args)
{
    if (ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
    {
        Console::Write("Can not send CMSG_CHAR_CREATE while online: go to character screen", ECHO_COLOR);
        return true; // No idea what happens if we return false...
    }

    std::string charname = strtok((char*)args, " ");
    std::string skin = strtok(NULL, " ");
    std::string face = strtok(NULL, " ");
    std::string hairStyle = strtok(NULL, " ");
    std::string hairColor = strtok(NULL, " ");
    std::string facialHair = strtok(NULL, " ");
    std::string outfitId = strtok(NULL, " ");

    for (int i = 0; i < 2000; ++i)
    {
        CDataStore data;
        data.PutInt32(CMSG_CHAR_CREATE);
        data.PutString(charname.c_str());
        data.PutInt8(1); // race, human
        data.PutInt8(1); // class, warrior
        data.PutInt8(0); // gender, male
        data.PutInt8(int8(std::atof(skin.c_str()))); // skin, guess
        data.PutInt8(int8(std::atof(face.c_str()))); // face, guess
        data.PutInt8(int8(std::atof(hairStyle.c_str()))); // hairStyle, guess
        data.PutInt8(int8(std::atof(hairColor.c_str()))); // hairColor, guess
        data.PutInt8(int8(std::atof(facialHair.c_str()))); // facialHair, guess
        data.PutInt8(int8(std::atof(outfitId.c_str()))); // outfitId, guess
        
        ClientServices::SendPacket(&data);
    }

    Console::Write("CMSG_CHAR_CREATE: %s", ECHO_COLOR, charname.c_str());
    return true;
}

BOOL CCommand_DeleteCharCommand(char const* cmd, char const* args)
{
    if (ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
    {
        Console::Write("Can not send CMSG_CHAR_DELETE while online: go to character screen", ECHO_COLOR);
        return true; // No idea what happens if we return false...
    }

    CDataStore data;
    data.PutInt32(CMSG_CHAR_DELETE);
    int guid = atoi(args);
    data.PutInt64(guid);
    
    ClientServices::SendPacket(&data);

    // not doing what we expeted
    //CDataStore data2;
    //data2.PutInt32(CMSG_CHAR_ENUM);
    //data2.Finalize();
    //ClientServices::SendPacket(&data2);

    Console::Write("CMSG_CHAR_DELETE & CMSG_CHAR_ENUM: %i", ECHO_COLOR, guid);
    return true;
}

BOOL CCommand_CharLoginCommand(char const* cmd, char const* args)
{
    if (ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
    {
        Console::Write("Can not send CMSG_PLAYER_LOGIN while online: go to character screen", ECHO_COLOR);
        return true;
    }

    CDataStore data;
    data.PutInt32(CMSG_PLAYER_LOGIN);
    int guid = atoi(args);
    data.PutInt64(guid);
    
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_PLAYER_LOGIN", ECHO_COLOR);
    return true;
}

BOOL CCommand_HeartAndResurrect(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_HEARTH_AND_RESURRECT);
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_HEARTH_AND_RESURRECT", ECHO_COLOR);
    return true;
}

BOOL CCommand_OfferPetition(char const* cmd, char const* args)
{
    if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
    {
        if (CGObject_C* target = ObjectMgr::GetObjectPtr(player->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_UNIT))
        {
            CDataStore data;
            data.PutInt32(CMSG_OFFER_PETITION);
            data.PutInt32(0); // junk
            data.PutInt64(1); // petitionguid
            data.PutInt64(target->GetValue<uint64>(OBJECT_FIELD_GUID)); // plguid
            ClientServices::SendPacket(&data);
            Console::Write("CMSG_GUILD_BANKER_ACTIVATE", ECHO_COLOR);
        }
    }
    return true;
}

BOOL CCommand_UseItem(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_USE_ITEM);
    data.PutInt8(255); //! bagIndex
    data.PutInt8(12); //! slot
    data.PutInt8(4); //! castCount
    data.PutInt32(43713); //! spellId
    data.PutInt64(4611686018427388167); //! itemGUID
    data.PutInt32(0); //! glyphIndex
    data.PutInt8(0); //! castFlags
    data.PutInt32(0); //! targetMask (TARGET_FLAG_NONE)
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_USE_ITEM", ECHO_COLOR);
    return true;
}
