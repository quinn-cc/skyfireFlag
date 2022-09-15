/*
 Custom flag: SkyFire (+SF)
 Firing triggers a hail of missiles from the sky falling in front of your tank.
 
 Server variables:
 _skyfireHeight - the height at which the missiles start
 _skyfireRadius - the radius of which the missiles spawn in
 _skyfireCount - the number of missiles
 _skyfireVertSpeedAdVel - multiplied by normal shot speed to determine speed the missiles come down at
 _skyfireHorizSpeedAdVel - multiplied by tank speed to determine the speed at which the missiles travel at
 
 Extra notes:
 - world weapon player shots make use of metadata "owner" and "type". Owner is the
   playerID and type is "SF".
 
 Copyright 2022 Quinn Carmack
 May be redistributed under either the LGPL or MIT licenses.
 
 ./configure --enable-custom-plugins=skyfireFlag
*/

#include "bzfsAPI.h"
#include <math.h>
#include <cstdlib>
#include <ctime>

using namespace std;

class SkyfireFlag : public bz_Plugin {

    virtual const char* Name()
    {
        return "Skyfire Flag";
    }

    virtual void Init(const char*);
    virtual void Event(bz_EventData*);
    ~SkyfireFlag();

    virtual void Cleanup(void)
    {
        Flush();
    }
};

BZ_PLUGIN(SkyfireFlag)

void SkyfireFlag::Init(const char*)
{
    bz_RegisterCustomFlag("SF", "Skyfire", "Firing triggers a hail of missiles from the sky falling in front of your tank.", 0, eGoodFlag);
    bz_registerCustomBZDBDouble("_skyfireHeight", 50);		
    bz_registerCustomBZDBDouble("_skyfireRadius", 30);	
    bz_registerCustomBZDBInt("_skyfireCount", 10);		
    bz_registerCustomBZDBDouble("_skyfireVertSpeedAdVel", 1);
    bz_registerCustomBZDBDouble("_skyfireHorizSpeedAdVel", 5);
    Register(bz_eShotFiredEvent);
    Register(bz_ePlayerDieEvent);
}

SkyfireFlag::~SkyfireFlag() {}

float randomFloat(float a, float b)
{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void SkyfireFlag::Event(bz_EventData *eventData)
{
    if (eventData->eventType == bz_eShotFiredEvent)
    {
        bz_ShotFiredEventData_V1* data = (bz_ShotFiredEventData_V1*) eventData;
        bz_BasePlayerRecord* playerRecord = bz_getPlayerByIndex(data->playerID);
        
        if (playerRecord && playerRecord->currentFlag == "SkyFire (+SF)")
        {
        	unsigned int seed = (unsigned) data->shotID;
        	std::srand(seed);
        
		    for (int i = 0; i < bz_getBZDBInt("_skyfireCount"); i++)
		    {		    		    	
		    	double r = randomFloat(0, bz_getBZDBDouble("_skyfireRadius"));		// radius from the center
		    	double ang = randomFloat(0, 6.283); // 6.283 = 2pi					// angle
		    	
		    	float pos[3];
		    	pos[0] = r*cos(ang) + playerRecord->lastKnownState.pos[0];
		    	pos[1] = r*sin(ang) + playerRecord->lastKnownState.pos[1];
		    	pos[2] = bz_getBZDBDouble("_skyfireHeight");
		    	
		    	float vel[3];
		    	vel[0] = (cos(playerRecord->lastKnownState.rotation) + 2*playerRecord->lastKnownState.velocity[0]/bz_getBZDBDouble("_tankSpeed"))*bz_getBZDBDouble("_skyfireHorizSpeedAdVel");
		    	vel[1] = (sin(playerRecord->lastKnownState.rotation) + 2*playerRecord->lastKnownState.velocity[1]/bz_getBZDBDouble("_tankSpeed"))*bz_getBZDBDouble("_skyfireHorizSpeedAdVel");
		    	vel[2] = (-1) * bz_getBZDBDouble("_skyfireVertSpeedAdVel");
		    	
		    	uint32_t shotGUID = bz_fireServerShot("GM", pos, vel, playerRecord->team);
		    	bz_setShotMetaData(shotGUID, "type", "SF");
				bz_setShotMetaData(shotGUID, "owner", data->playerID);
		    }
        }
        
        bz_freePlayerRecord(playerRecord);
    }
    else if (eventData->eventType == bz_ePlayerDieEvent)
    {
    	bz_PlayerDieEventData_V1* data = (bz_PlayerDieEventData_V1*) eventData;
		uint32_t shotGUID = bz_getShotGUID(data->killerID, data->shotID);

		if (bz_shotHasMetaData(shotGUID, "type") && bz_shotHasMetaData(shotGUID, "owner"))
		{
		    std::string flagType = bz_getShotMetaDataS(shotGUID, "type");

		    if (flagType == "SF")
		    {
		        data->killerID = bz_getShotMetaDataI(shotGUID, "owner");
		        data->killerTeam = bz_getPlayerTeam(data->killerID);
		    }
		}
    }
}

