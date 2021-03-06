#include "stdafx.h"
#include <sstream>
#include <list>
#include <vector>
#include <map>

//! useless
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

//! useless
BOOL CCommand_Beastmaster(char const* cmd, char const* args)
{
    CDataStore data(CMSG_BEASTMASTER);
    int state = _strnicmp(args, "off", INT_MAX) != 0;
	data.PutInt8(state);
	data.Finalize();
    ClientServices::SendPacket(&data);

    Console::Write("Beastmaster mode is %s", ECHO_COLOR, state ? "on" : "off");

    return TRUE;
}

//! useless
BOOL CCommand_Invis(char const* cmd, char const* args)
{
    CDataStore data(CMSG_GM_INVIS);
    int state = _strnicmp(args, "off", INT_MAX) != 0;
	data.PutInt32(state);
	data.Finalize();
    ClientServices::SendPacket(&data);

    Console::Write("GM invis mode is %s", ECHO_COLOR, state ? "on" : "off");

    return TRUE;
}

//! useless
BOOL CCommand_DBLookup(char const* cmd, char const* args)
{
    CDataStore data(CMSG_DBLOOKUP);
	data.PutString(args);
	data.Finalize();
    ClientServices::SendPacket(&data);

    return TRUE;
}

//! useless
BOOL ShowObjectsEnumProc(uint64 objectGuid, void *param)
{
    CGObject_C *pObject = ObjectMgr::GetObjectPtr(objectGuid, TYPEMASK_OBJECT);

    if (pObject)
		Console::Write("Object '%s', guid 0x%016llX", HIGHLIGHT_COLOR, pObject->GetObjectName(), pObject->GetValue<uint64>(OBJECT_FIELD_GUID));

    return TRUE;
}

//! useless
BOOL CCommand_ShowObjects(char const* cmd, char const* args)
{
	if (!ObjectMgr::GetActivePlayerGuid())
	{
		Console::Write("Error: Not in world!", ERROR_COLOR);
		return TRUE;
	}

    ObjectMgr::EnumVisibleObjects(ShowObjectsEnumProc, NULL);

    return TRUE;
}

//! useless
BOOL CCommand_TaxiBenchmarkServer(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_SET_TAXI_BENCHMARK_MODE);
	data.PutInt8(1);
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_SET_TAXI_BENCHMARK_MODE", ECHO_COLOR);
    return true;
}

//! Create guild with any name. If used in a smart way, allows you to crash other players' clients without a trace.
BOOL CCommand_CreateGuildCommand(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_GUILD_CREATE);
    std::string guildname = std::string(args);
	data.PutString(guildname.c_str());
	data.Finalize();
    ClientServices::SendPacket(&data);

    std::ostringstream ss;
    ss << "Sent CMSG_GUILD_CREATE with guildname " << guildname.c_str() << ". Args: " << args;
    Console::Write(ss.str().c_str(), ECHO_COLOR);
    return true;
}

//! Flood a target with ignore messages
BOOL CCommand_SendIgnoreCommand(char const* cmd, char const* args)
{
    //! Send '<name> is ignoring you.' to target X times (this message can't be blocked).
    uint64 targetGuid = -1;

    if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
        if (CGObject_C* target = ObjectMgr::GetObjectPtr(player->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_PLAYER))
            targetGuid = target->GetValue<uint64>(OBJECT_FIELD_GUID);

    if (targetGuid == -1)
        return true;

    long floodCount = atoi(args);

    for (long i = 0; i < floodCount; ++i)
    {
        CDataStore data;
        data.PutInt32(CMSG_CHAT_IGNORED);
        data.PutInt64(targetGuid);
        data.PutInt8(1); // unk
		data.Finalize();
        ClientServices::SendPacket(&data);
    }

    std::stringstream ss;
    ss << "CMSG_CHAT_IGNORED sent " << floodCount << " times";
    Console::Write(ss.str().c_str(), ECHO_COLOR);
    return true;
}

//! useless?
BOOL CCommand_LootRollCommand(char const* cmd, char const* args)
{
    std::string itemGuid = strtok((char*)args, " ");
    std::string lootType = strtok(NULL, " "); // loottype, 0=pass, 1=need, 2=greed

    CDataStore data;
    data.PutInt32(CMSG_LOOT_ROLL);
    data.PutInt64(int64(std::atof(itemGuid.c_str()))); // itemguid to roll for
    data.PutInt32(1); // itemslot, unused
	data.PutInt8(int8(std::atof(lootType.c_str())));
	data.Finalize();
    ClientServices::SendPacket(&data);

    std::stringstream ss;
    ss << "CMSG_LOOT_ROLL with guid " << itemGuid << " and loot type " << lootType;
    Console::Write(ss.str().c_str(), ECHO_COLOR);
    return true;
}

//! useless?
BOOL CCommand_CharRenameCommand(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_CHAR_RENAME);
    data.PutInt64(ObjectMgr::GetActivePlayerGuid());
    std::string newName = strtok((char*)args, " ");
	data.PutString(newName.c_str());
	data.Finalize();
    ClientServices::SendPacket(&data);

    Console::Write("CMSG_CHAR_RENAME", ECHO_COLOR);
    return true;
}

//! useless
BOOL CCommand_TeleportCommand(char const* cmd, char const* args)
{
    std::string mapid = strtok((char*)args, " ");
    std::string posX = strtok(NULL, " ");
    std::string posY = strtok(NULL, " ");
    std::string posZ = strtok(NULL, " ");

    CDataStore data;
    data.PutInt32(CMSG_WORLD_TELEPORT);
    data.PutInt32(50); // time in ms, nfi what its used for
    data.PutInt32(int32(std::atof(mapid.c_str())));
    data.PutFloat(float(std::atof(posX.c_str())));
    data.PutFloat(float(std::atof(posY.c_str())));
    data.PutFloat(float(std::atof(posZ.c_str())));
    data.PutInt32(int32(std::atof(mapid.c_str())));
    data.PutFloat(3.141593f); // orientation
	data.Finalize();
    ClientServices::SendPacket(&data);

    Console::Write("CMSG_WORLD_TELEPORT", ECHO_COLOR);
    return true;
}

//! useless
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
	data.Finalize();

    for (int i = 0; i < 2000; ++i)
        ClientServices::SendPacket(&data);

    Console::Write("CMSG_CHAR_CREATE: %s", ECHO_COLOR, charname.c_str());
    return true;
}

//! useless
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
	data.Finalize();
    ClientServices::SendPacket(&data);

    Console::Write("CMSG_CHAR_DELETE & CMSG_CHAR_ENUM: %i", ECHO_COLOR, guid);
    return true;
}

//! useless
BOOL CCommand_CharLoginCommand(char const* cmd, char const* args)
{
    if (ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
    {
        Console::Write("Can not send CMSG_PLAYER_LOGIN while online: go to character screen", ECHO_COLOR);
        return true;
    }

    int guid = atoi(args);

    CDataStore data;
    data.PutInt32(CMSG_PLAYER_LOGIN);
	data.PutInt64(guid);
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_PLAYER_LOGIN", ECHO_COLOR);
    return true;
}

//! useless
BOOL CCommand_HeartAndResurrect(char const* cmd, char const* args)
{
    CDataStore data;
	data.PutInt32(CMSG_HEARTH_AND_RESURRECT);
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_HEARTH_AND_RESURRECT", ECHO_COLOR);
    return true;
}

//! useless
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
			data.Finalize();
            ClientServices::SendPacket(&data);
            Console::Write("CMSG_GUILD_BANKER_ACTIVATE", ECHO_COLOR);
        }
    }
    return true;
}

//! Use item, currently useless
BOOL CCommand_UseItem(char const* cmd, char const* args)
{
	std::string bagIndex = strtok((char*)args, " "); // 127
	std::string slot = strtok(NULL, " "); // 12
	std::string castCount = strtok(NULL, " "); // 4
	std::string spellId = strtok(NULL, " "); // 43713
	std::string itemGUID = strtok(NULL, " "); // 4611686018427388167
	std::string glyphIndex = strtok(NULL, " "); // 0
	std::string castFlags = strtok(NULL, " "); // 0
	std::string targetMask = strtok(NULL, " "); // 0

    CDataStore data;
    data.PutInt32(CMSG_USE_ITEM);
	data.PutInt8(int32(std::atof(bagIndex.c_str())));
	data.PutInt8(int32(std::atof(slot.c_str())));
	data.PutInt8(int32(std::atof(castCount.c_str())));
	data.PutInt32(int32(std::atof(spellId.c_str())));
	data.PutInt64(int32(std::atof(itemGUID.c_str())));
	data.PutInt32(int32(std::atof(glyphIndex.c_str())));
	data.PutInt8(int32(std::atof(castFlags.c_str())));
	data.PutInt32(int32(std::atof(targetMask.c_str())));
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_USE_ITEM", ECHO_COLOR);
    return true;
}

//! Inspect your mouseover target's honor tab
BOOL CCommand_HonorInspectCommand(char const* cmd, char const* args)
{
    uint64 guid = *(uint64*)0x00BD07A0;
    Console::Write("Mouseover GUID: %ul", ECHO_COLOR, guid);

    if (guid)
    {
        CDataStore data;
        data.PutInt32(MSG_INSPECT_HONOR_STATS);
		data.PutInt64(guid);
		data.Finalize();
        ClientServices::SendPacket(&data);
        Console::Write("MSG_INSPECT_HONOR_STATS", ECHO_COLOR);
    }

    return true;
}

//! Visually sets your ammo pouch slot to an item even if you don't have it
BOOL CCommand_SetAmmoCommand(char const* cmd, char const* args)
{
    int ammoItemId = atoi(args);

    CDataStore data;
    data.PutInt32(CMSG_SET_AMMO);
	data.PutInt32(ammoItemId);
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_SET_AMMO: %u", ECHO_COLOR, ammoItemId);
    return true;
}

//! use gameobject guid
BOOL CCommand_UseGameobjectGuidCommand(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_GAMEOBJ_USE);
    int64 goGuid = _atoi64(args);
    data.PutInt64(goGuid);
    data.Finalize();
    ClientServices::SendGamePacket(&data);
    Console::Write("CMSG_GAMEOBJ_USE: %u", ECHO_COLOR, goGuid);
    return true;
}

//! Flood!
BOOL CCommand_ReportBug(char const* cmd, char const* args)
{
    uint64 timesToSend = atoi(args);
    std::string header = "Header bug report - CMSG_BUG";
    std::string message = "Message bug report - CMSG_BUG";

    for (int i = 0; i < timesToSend; ++i)
    {
        CDataStore data;
        data.PutInt32(CMSG_BUG);
        data.PutInt32(1);
        data.PutInt32(message.length());
        data.PutString(message.c_str());
        data.PutInt32(header.length());
		data.PutString(header.c_str());
		data.Finalize();
        ClientServices::SendPacket(&data);
    }

    Console::Write("CMSG_BUG", ECHO_COLOR);
    return true;
}

//! useless?
BOOL CCommand_LootGuidCommand(char const* cmd, char const* args)
{
    int lootGuid = atoi(args);

    CDataStore data;
    data.PutInt32(CMSG_LOOT);
	data.PutInt64(lootGuid);
	data.Finalize();
    ClientServices::SendPacket(&data);

    Console::Write("CMSG_LOOT: %u", ECHO_COLOR, lootGuid);
    return true;
}

//! useless?
BOOL CCommand_TextEmoteCommand(char const* cmd, char const* args)
{
    std::string text_emote = strtok((char*)args, " "); // Dance = 34
    std::string emoteNum = strtok(NULL, " ");          // Dance = 4294967295
    std::string guid = strtok(NULL, " ");              // Dance = 0

    CDataStore data;
    data.PutInt32(CMSG_TEXT_EMOTE);
    data.PutInt32(int32(std::atof(text_emote.c_str())));
    data.PutInt32(int32(std::atof(emoteNum.c_str())));
	data.PutInt64(int64(std::atof(guid.c_str())));
	data.Finalize();
    ClientServices::SendPacket(&data);

    Console::Write("CMSG_TEXT_EMOTE", ECHO_COLOR);
    return true;
}

//! useless (?)
BOOL CCommand_RefundItem(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_ITEM_REFUND);
    data.PutInt64(atoi(args)); // guid
    data.Finalize();
    ClientServices::SendGamePacket(&data);
    return true;
}

//! reset talents at skill/profession trainer
BOOL CCommand_TalentWipeConfirm(char const* cmd, char const* args)
{
    if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
    {
        if (CGObject_C* target = ObjectMgr::GetObjectPtr(player->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_UNIT))
        {
            CDataStore data;
            data.PutInt32(MSG_TALENT_WIPE_CONFIRM);
            data.PutInt64(target->GetValue<uint64>(OBJECT_FIELD_GUID));
            data.Finalize();
            ClientServices::SendGamePacket(&data);
            Console::Write("MSG_TALENT_WIPE_CONFIRM", ECHO_COLOR);
        }
    }

    return true;
}

//! useless
BOOL CCommand_SpiritHealerActivate(char const* cmd, char const* args)
{
    uint64 guid = *(uint64*)0x00BD07A0;
    Console::Write("Mouseover GUID: %ul", ECHO_COLOR, guid);

    if (guid)
    {
        CDataStore data;
        data.PutInt32(CMSG_SPIRIT_HEALER_ACTIVATE);
        data.PutInt64(guid);
        data.Finalize();
        ClientServices::SendGamePacket(&data);
        Console::Write("CMSG_SPIRIT_HEALER_ACTIVATE", ECHO_COLOR);
    }

    return true;
}

//! Chat in universal language
BOOL CCommand_Chat(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_MESSAGECHAT);

    std::string type = strtok((char*)args, " ");
    std::string lang = strtok(NULL, " ");
    std::string message = strtok(NULL, " ");
    data.PutInt32(atoi(type.c_str()));
    data.PutInt32(atoi(lang.c_str()));
	data.PutString(message.c_str());
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_MESSAGECHAT", ECHO_COLOR);
    return true;
}

//! Say in universal language
BOOL CCommand_Say(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_MESSAGECHAT);
    data.PutInt32(1); // say
    data.PutInt32(0); // universal
	data.PutString(args);
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_MESSAGECHAT say", ECHO_COLOR);
    return true;
}

//! Yell in universal language
BOOL CCommand_Yell(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_MESSAGECHAT);
    data.PutInt32(6); // yell
    data.PutInt32(0); // universal
	data.PutString(args);
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_MESSAGECHAT yell", ECHO_COLOR);
    return true;
}

//! Emote in universal language
BOOL CCommand_Emote(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_MESSAGECHAT);
    data.PutInt32(10); // emote
    data.PutInt32(0); // universal
	data.PutString(args);
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_MESSAGECHAT emote", ECHO_COLOR);
    return true;
}

BOOL CCommand_ChannelInviteCommand(char const* cmd, char const* args)
{
    std::string channelName = strtok((char*)args, " ");
    std::string targetName = strtok(NULL, " ");
    std::string floodCountStr = strtok(NULL, " ");
    long floodCount = floodCountStr != "" ? atoi(floodCountStr.c_str()) : 1;

    Console::Write("args: %s", ECHO_COLOR, (char*)args);
    Console::Write("channelName: %s", ECHO_COLOR, channelName.c_str());
    Console::Write("targetName: %s", ECHO_COLOR, targetName.c_str());
    Console::Write("floodCount: %s", ECHO_COLOR, floodCountStr.c_str());

    if (targetName == "")
        if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
            if (CGObject_C* target = ObjectMgr::GetObjectPtr(player->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_PLAYER))
                targetName = target->GetObjectName();

    for (long i = 0; i < floodCount; ++i)
    {
        CDataStore data;
        data.PutInt32(CMSG_CHANNEL_INVITE);
        data.PutString(channelName.c_str()); //! Channel name
        data.PutString(targetName.c_str()); //! Target name
        data.Finalize();
        ClientServices::SendPacket(&data);
    }

    std::ostringstream ss;
    ss << "Sent CMSG_CHANNEL_INVITE " << floodCount << " times";
    Console::Write(ss.str().c_str(), ECHO_COLOR);
    return true;
}

// 124cffff8000\124TInterface\\Icons\\temp.blp:21:21:0:0\124t
BOOL CCommand_ChannelJoinCommand(char const* cmd, char const* args)
{
    std::string channelIdStr = strtok((char*)args, " ");
    Console::Write("channelId: %s", ECHO_COLOR, channelIdStr.c_str());
    std::string unk1Str = strtok(NULL, " ");
    Console::Write("unk1Str: %s", ECHO_COLOR, unk1Str.c_str());
    std::string unk2Str = strtok(NULL, " ");
    Console::Write("unk2Str: %s", ECHO_COLOR, unk2Str.c_str());
    std::string channelName = strtok(NULL, " ");
    Console::Write("channelName: %s", ECHO_COLOR, channelName.c_str());
    std::string password = strtok(NULL, " ");
    Console::Write("password: %s", ECHO_COLOR, password.c_str());
    long channelId = channelIdStr != "" ? atoi(channelIdStr.c_str()) : 1;
    long unk1 = unk1Str != "" ? atoi(unk1Str.c_str()) : 1;
    long unk2 = unk2Str != "" ? atoi(unk2Str.c_str()) : 1;

    Console::Write("args: %s", ECHO_COLOR, (char*)args);

    CDataStore data;
    data.PutInt32(CMSG_JOIN_CHANNEL);
    data.PutInt32(channelId); //! channelid
    data.PutInt32(unk1); //! unk1
    data.PutInt32(unk2); //! unk2
    data.PutString(channelName.c_str()); //! channelName
    data.PutString(password.c_str()); //! password
    data.Finalize();
    ClientServices::SendPacket(&data);

    Console::Write("Sent CMSG_CHANNEL_INVITE", ECHO_COLOR);
    return true;
}

//! useless
BOOL CCommand_UnlearnSkil(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_UNLEARN_SKILL);
    data.PutInt32(atoi(args)); //! Skill id
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_UNLEARN_SKILL", ECHO_COLOR);
    return true;
}

//! useless
BOOL CCommand_ActiveGuildBank(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_GUILD_BANKER_ACTIVATE);
    data.PutInt64(17370386905627141050); //! Stormwind bank
    data.PutInt8(1); //! sendallSlots
	data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_GUILD_BANKER_ACTIVATE", ECHO_COLOR);
    return true;
}

//! Activate taxi path you don't even know. Can also be abused to fly to programmers isle.
BOOL CCommand_ActivateTaxi(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_ACTIVATETAXI);

    if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
    {
        if (CGObject_C* target = ObjectMgr::GetObjectPtr(player->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_UNIT))
            data.PutInt64(target->GetValue<uint64>(OBJECT_FIELD_GUID));
        else
            return true;
    }
    else
        return true;

    std::string takeOffNode = strtok((char*)args, " ");
    std::string lastNode = strtok(NULL, " ");
    data.PutInt32(atoi(takeOffNode.c_str()));
    data.PutInt32(atoi(lastNode.c_str()));//172); // 198 is programmer's island
    data.Finalize();
    ClientServices::SendPacket(&data);

    CDataStore data2;
    data2.PutInt32(CMSG_MOVE_SPLINE_DONE);
    data2.PutInt64(0);
    data2.PutInt32(0);
    data2.PutInt16(0);
    data2.PutInt32(0);
    data2.PutFloat(0);
    data2.PutFloat(0);
    data2.PutFloat(0);
    data2.PutFloat(0);
    data2.PutInt32(0);
    data2.PutInt32(0);
    data2.Finalize();
    ClientServices::SendGamePacket(&data2);
    Console::Write("CMSG_ACTIVATETAXI + CMSG_MOVE_SPLINE_DONE. Try with 23, 198 for orgrimmar -> programmer's island", ECHO_COLOR);
    return true;
}

//! Instantly finish your current taxi path.
BOOL CCommand_FinishTaxi(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_MOVE_SPLINE_DONE);
    data.PutInt64(0);
    data.PutInt32(0);
    data.PutInt16(0);
    data.PutInt32(0);
    data.PutFloat(0);
    data.PutFloat(0);
    data.PutFloat(0);
    data.PutFloat(0);
    data.PutInt32(0);
    data.PutInt32(0);
    data.Finalize();
    ClientServices::SendGamePacket(&data);
    return true;
}

//! Literally uses the gameobject your mouse is mousing over at when the command is sent. Can be
//! abused on WSG/EOTS flags; WSG flags can now be picked up from any distance and EOTS flags can
//! be picked up instantly and while, for example, mounted or stealthed.
BOOL CCommand_UseMouseOverGameobject(char const* cmd, char const* args)
{
    uint64 guid = *(uint64*)0x00BD07A0;
    Console::Write("Mouseover GUID: %ul", ECHO_COLOR, guid);

    if (guid)
    {
        CDataStore data;
        data.PutInt32(CMSG_GAMEOBJ_USE);
        data.PutInt64(guid);
        data.Finalize();
        ClientServices::SendGamePacket(&data);
        Console::Write("CMSG_GAMEOBJ_USE", ECHO_COLOR);
    }

    return true;
}

BOOL CCommand_MoveKnockbackAck(char const* cmd, char const* args)
{
	if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
	{
		CDataStore data;
		data.PutInt32(CMSG_MOVE_KNOCK_BACK_ACK);
		int64 guid = player->GetValue<uint64>(OBJECT_FIELD_GUID);
		data.PutPackedGUID(guid);
		data.PutInt32(0);
		data.PutInt16(0);
		data.PutInt32(0);
		data.PutFloat(0);
		data.PutFloat(0);
		data.PutFloat(0);
		data.PutFloat(0);
		data.PutInt32(0);
		data.PutInt32(0);
		data.Finalize();
		ClientServices::SendGamePacket(&data);

		Console::Write("CMSG_MOVE_KNOCK_BACK_ACK", ECHO_COLOR);
	}

	return true;
}

BOOL CCommand_StopFall(char const* cmd, char const* args)
{
	if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
	{
		CDataStore data;
		data.PutInt32(CMSG_MOVE_FALL_RESET);
		int64 guid = player->GetValue<uint64>(OBJECT_FIELD_GUID);
		data.PutPackedGUID(guid);
		data.PutInt64(0);
		data.PutInt32(0);
		data.PutInt16(0);
		data.PutInt32(0);
		data.PutFloat(0);
		data.PutFloat(0);
		data.PutFloat(0);
		data.PutFloat(0);
		data.PutInt32(0);
		data.PutInt32(0);
		data.Finalize();
		ClientServices::SendGamePacket(&data);

		Console::Write("CMSG_MOVE_FALL_RESET", ECHO_COLOR);
	}

	return true;
}

//! Can be abused to loot an item that is not visible to you as often as you want because it is blocked by conditions.
//! To get a list of items, select all rows from the `conditions` table in your TDB with SourceTypeOrReferenceId on 1
//! and send this command with the loot slot. Loot slot is the index of the item, so if there's three items in there,
//! they all have the slots (respectively) 0, 1 and 2.
BOOL CCommand_AutoStoreLoot(char const* cmd, char const* args)
{
    if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
    {
        CDataStore data;
        data.PutInt32(CMSG_AUTOSTORE_LOOT_ITEM);
        data.PutInt32(atoi(args)); //! lootSlot
        data.Finalize();
        ClientServices::SendGamePacket(&data);
        Console::Write("CMSG_AUTOSTORE_LOOT_ITEM", ECHO_COLOR);
    }

    return true;
}

//! Can be abused to set the master looter to someone not in the group or not eligible to actually become a master looter.
//! Also possible to set the loot method beyond the max amount of methods, unknown result.
BOOL CCommand_SetLootMethod(char const* cmd, char const* args)
{
    if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
    {
        std::string lootMethod = strtok((char*)args, " ");
        std::string lootMaster = strtok(NULL, " ");
        std::string lootTheshold = strtok(NULL, " ");

        CDataStore data;
        data.PutInt32(CMSG_LOOT_METHOD);
        data.PutInt32(atoi(lootMethod.c_str())); //! lootMethod
        data.PutInt32(atoi(lootMaster.c_str())); //! lootMaster
        data.PutInt32(atoi(lootTheshold.c_str())); //! lootTheshold
        data.Finalize();
        ClientServices::SendGamePacket(&data);
        Console::Write("CMSG_LOOT_METHOD", ECHO_COLOR);
    }

    return true;
}

BOOL CCommand_CancelMountAura(char const* cmd, char const* args)
{
    if (CGObject_C* player = ObjectMgr::GetObjectPtr(ObjectMgr::GetActivePlayerGuid(), TYPEMASK_PLAYER))
    {
        CDataStore data;
        data.PutInt32(CMSG_CANCEL_MOUNT_AURA);
        data.Finalize();
        ClientServices::SendGamePacket(&data);
        Console::Write("CMSG_CANCEL_MOUNT_AURA", ECHO_COLOR);
    }

    return true;
}

BOOL CCommand_SetTradeGold(char const* cmd, char const* args)
{
    CDataStore data;
    data.PutInt32(CMSG_SET_TRADE_GOLD);
    data.PutInt32(atoi(args)); //! Skill id
    data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_SET_TRADE_GOLD", ECHO_COLOR);
    return true;
}

static uint64 lastMouseOverGuid = 0;

uint64 GetLastMouseOverGuid()
{
    uint64 x = lastMouseOverGuid;
    lastMouseOverGuid = 0;
    return x;
}

BOOL CCommand_SellItem(char const* cmd, char const* args)
{
    uint64 vendorGuid = *(uint64*)0x00BD07A0;
    uint64 lastMouseOverGuid = GetLastMouseOverGuid();

    CDataStore data;
    data.PutInt32(CMSG_SELL_ITEM);
    data.PutInt64(vendorGuid); //! vendor guid
    data.PutInt64(4611686018898920923); //! item guid
    data.PutInt32(1); //! count
    data.Finalize();
    ClientServices::SendPacket(&data);
    Console::Write("CMSG_SELL_ITEM", ECHO_COLOR);

    std::ostringstream ss;
    ss << "Called CMSG_SELL_ITEM. vendorGuid = " << vendorGuid << " lastMouseOverGuid = " << lastMouseOverGuid;
    Console::Write(ss.str().c_str(), ECHO_COLOR);
    return true;
}

BOOL CCommand_StoreMouseOverGuid(char const* cmd, char const* args)
{
    lastMouseOverGuid = *(uint64*)0x00BD07A0;
    std::ostringstream ss;
    ss << "Stored guid " << lastMouseOverGuid << " - " << *(uint64*)0x00BD07A0;
    Console::Write(ss.str().c_str(), ECHO_COLOR);
    return true;
}
