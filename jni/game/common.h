#pragma once

#define PLAYER_PED_SLOTS	140

typedef unsigned short OBJECTID;
typedef unsigned short VEHICLEID;
typedef unsigned short PLAYERID;

#define MAX_MATERIALS_PER_MODEL 16

#define MAX_MENUS 128

#define PADDING(x,y) uint8_t x[y]

#define IN_VEHICLE(x) ((x->dwStateFlags & 0x100) >> 8)
#define IS_VEHICLE_MOD(x) (x >= 1000 && x <= 1193)

//-----------------------------------------------------------
typedef struct _RECT
{
	float fLeft; 	// X1
	float fBottom; 	// Y1
	float fRight; 	// X2
	float fTop;		// Y2
} RECT, *PRECT;

struct CRect
{
public:
	float left;          // x1
	float bottom;        // y1
	float right;         // x2
	float top;           // y2
};

#pragma pack(1)
typedef struct _VECTOR 
{
	float X,Y,Z;
} VECTOR, *PVECTOR;

typedef struct _OBJECT_REMOVE {
	uint32_t dwModel;
	float fRange;
	VECTOR vecPosition;
} OBJECT_REMOVE;

#pragma pack(1)
typedef struct _MATRIX4X4 
{
	VECTOR right;		// 0-12 	; r11 r12 r13
	uint32_t  flags;	// 12-16
	VECTOR up;			// 16-28	; r21 r22 r23
	float  pad_u;		// 28-32
	VECTOR at;			// 32-44	; r31 r32 r33
	float  pad_a;		// 44-48
	VECTOR pos;			// 48-60
	float  pad_p;		// 60-64
} MATRIX4X4, *PMATRIX4X4;

//-----------------------------------------------------------
//m_pRwObject
#pragma pack(1)
typedef struct _ENTITY_TYPE
{
	uint32_t vtable;				// 000-004
	PADDING(_pad0, 12);				// 004-016
	float fRotZBeforeMat;			// 016-020
	MATRIX4X4 *mat;					// 020-024

	union {
		uintptr_t m_pRwObject;
		uintptr_t m_pRpClump;
		uintptr_t m_pRpAtomic;
		uintptr_t pdwRenderWare;	// 24-28	;rwObject
	};

	union {
		uint32_t m_nEntityFlags;	//
		struct {
			uint32_t m_bUsesCollision : 1;
			uint32_t m_bCollisionProcessed : 1;
			uint32_t m_bIsStatic : 1;
			uint32_t m_bHasContacted : 1;
			uint32_t m_bIsStuck : 1;
			uint32_t m_bIsInSafePosition : 1;
			uint32_t m_bWasPostponed : 1;
			uint32_t m_bIsVisible : 1;

			uint32_t m_bIsBIGBuilding : 1;
			uint32_t m_bRenderDamaged : 1;
			uint32_t m_bStreamingDontDelete : 1;
			uint32_t m_bRemoveFromWorld : 1;
			uint32_t m_bHasHitWall : 1;
			uint32_t m_bImBeingRendered : 1;
			uint32_t m_bDrawLast :1;
			uint32_t m_bDistanceFade : 1;

			uint32_t m_bDontCastShadowsOn : 1;
			uint32_t m_bOffscreen : 1;
			uint32_t m_bIsStaticWaitingForCollision : 1;
			uint32_t m_bDontStream : 1;
			uint32_t m_bUnderwater : 1;
			uint32_t m_bHasPreRenderEffects : 1;
			uint32_t m_bIsTempBuilding : 1;
			uint32_t m_bDontUpdateHierarchy : 1;

			uint32_t m_bHasRoadsignText : 1;
			uint32_t m_bDisplayedSuperLowLOD : 1;
			uint32_t m_bIsProcObject : 1;
			uint32_t m_bBackfaceCulled : 1;
			uint32_t m_bLightObject : 1;
			uint32_t m_bUnimportantStream : 1;
			uint32_t m_bTunnel : 1;
			uint32_t m_bTunnelTransition : 1;
		} nEntityFlags;				// 028-032
	};

	PADDING(_pad1, 2);				// 032-034
	uint16_t nModelIndex;			// 034-036
	PADDING(_pad93, 18);				// 36-54
	uint8_t nControlFlags; 				// 54-55
	PADDING(_pad95, 13);				// 55-68
	VECTOR vecMoveSpeed; 				// 68-80	;vecMoveSpeed
	VECTOR vecTurnSpeed; 				// 80-92	;vecTurnSpeed
	PADDING(_pad3, 88);				// 092-180
	uint32_t dwUnkModelRel;			// 180-184
} ENTITY_TYPE;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _WEAPON_SLOT_TYPE
{
	uint32_t dwType;
	uint32_t dwState;
	uint32_t dwAmmoInClip;
	uint32_t dwAmmo;
	PADDING(_pwep1,12);
} WEAPON_SLOT_TYPE;  // MUST BE EXACTLY ALIGNED TO 28 bytes

typedef struct
{
	char unk[0x14];
	int iNodeId;

} AnimBlendFrameData;

#pragma pack(1)
typedef struct _PED_TYPE
{	
	
	ENTITY_TYPE entity; 		// 0000-0184	;entity
	PADDING(_pad106, 174);		// 0184-0358
	uint32_t _pad107;			// 0358-0362	;dwPedType
	PADDING(_pad101, 734);		// 0362-1096
	uint32_t dwAction;			// 1096-1100	;Action
	PADDING(_pad102, 36);		// 1100-1136
	uintptr_t dwInvulFlags; 	// 1136-1140	0x1000 = can_decap
	PADDING(_pad228, 8); 		// 1140-1148
	uintptr_t Tasks; 			// 1148-1152
	uint32_t dwStateFlags; 		// 1152-1156	;StateFlags
	PADDING(_pad103, 12);		// 1156-1168
	AnimBlendFrameData* aPedBones[19];	// 1168 - 1244
	PADDING(_pad103_, 100);		// 1244-1344
	float fHealth;		 		// 1344-1348	;Health
	float fMaxHealth;			// 1348-1352	;MaxHealth
	float fArmour;				// 1352-1356	;Armour
	float fAim;
	PADDING(_pad104, 8);		// 1356-1368
	float fRotation1;			// 1368-1372	;Rotation1
	float fRotation2;			// 1372-1376	;Rotation2
	PADDING(_pad105, 44);		// 1376-1420
	uint32_t pVehicle;			// 1420-1424	;pVehicle
	PADDING(_pad108, 8);		// 1424-1432
	uint32_t dwPedType;			// 1432-1436	;dwPedType
	uint32_t dwUnk1;	 		// 1436-1440
	WEAPON_SLOT_TYPE WeaponSlots[13]; // 1440-1804
	PADDING(_pad270, 12); 		// 1804-1816
	uint8_t byteCurWeaponSlot; 	// 1816-1817
	PADDING(_pad280, 23); 		// 1817-1840
	uint32_t pFireObject;	 	// 1840-1844
	PADDING(_pad281, 44); 		// 1844-1888
	uint32_t  dwWeaponUsed; 	// 1888-1892
	uintptr_t pdwDamageEntity; 	// 1892-1896
	PADDING(_pad292, 8);		// 1896-1900
	uint32_t pContactVehicle; 	// 1904-1908
} PED_TYPE;

#pragma pack(1)
typedef struct _PLAYER_INFO
{	
	PED_TYPE* pPed;				// 000-004
	PADDING(_pad000, 42);		// 004-046
	uint32_t nMoney;			// 046-050
	uint32_t nDisplayMoney;		// 050-054
	PADDING(_pad054, 134);		// 054-188
	uint32_t nHaveMoney;		// 188-192
	PADDING(_pad188, 212);		// 192-404
} PLAYER_INFO;

typedef struct _AIM_SYNC_DATA
{
	uint8_t byteCamMode;
	VECTOR vecAimf1;
	VECTOR vecAimPos;
	float fAimZ;
	uint8_t byteCamExtZoom : 6;		// 0-63 normalized
	uint8_t byteWeaponState : 2;	// see eWeaponState
	uint8_t byteAspectRatio;
} AIM_SYNC_DATA;

typedef struct _BULLET_SYNC_DATA
{
	uint8_t byteHitType;
	uint16_t PlayerID;
	VECTOR vecOrigin;
	VECTOR vecPos;
	VECTOR vecOffset;
	uint8_t byteWeaponID;
} BULLET_SYNC_DATA;

//-----------------------------------------------------------

#pragma pack(1)
struct BULLET_SYNC
{
	uint8_t hitType;
	uint16_t hitId;
	float origin[3];
	float hitPos[3];
	float offsets[3];
	uint8_t weapId;
};

#pragma pack(1)
typedef struct _VEHICLE_TYPE
{
	ENTITY_TYPE entity;					// 0000-0184
	PADDING(_pad201, 880);				// 0184-1064

	union {
		uint8_t byteFlags;				// 1064-1072	;byteFlags
		struct {
			unsigned char bIsLawEnforcer : 1;
			unsigned char bIsAmbulanceOnDuty : 1;
			unsigned char bIsFireTruckOnDuty : 1;
			unsigned char bIsLocked : 1;
			unsigned char bEngineOn : 1;
			unsigned char bIsHandbrakeOn : 1;
			unsigned char bLightsOn : 1;
			unsigned char bFreebies : 1;

			unsigned char bIsVan : 1;
			unsigned char bIsBus : 1;
			unsigned char bIsBig : 1;
			unsigned char bLowVehicle : 1;
			unsigned char bComedyControls : 1;
			unsigned char bWarnedPeds : 1;
			unsigned char bCraneMessageDone : 1;
			unsigned char bTakeLessDamage : 1;

			unsigned char bIsDamaged : 1;
			unsigned char bHasBeenOwnedByPlayer : 1;
			unsigned char bFadeOut : 1;
			unsigned char bIsBeingCarJacked : 1;
			unsigned char bCreateRoadBlockPeds : 1;
			unsigned char bCanBeDamaged : 1;
			unsigned char bOccupantsHaveBeenGenerated : 1;
			unsigned char bGunSwitchedOff : 1;

			unsigned char bVehicleColProcessed : 1;
			unsigned char bIsCarParkVehicle : 1;
			unsigned char bHasAlreadyBeenRecorded : 1;
			unsigned char bPartOfConvoy : 1;
			unsigned char bHeliMinimumTilt : 1;
			unsigned char bAudioChangingGear : 1;
			unsigned char bIsDrowning : 1;
			unsigned char bTyresDontBurst : 1;

			unsigned char bCreatedAsPoliceVehicle : 1;
			unsigned char bRestingOnPhysical : 1;
			unsigned char bParking : 1;
			unsigned char bCanPark : 1;
			unsigned char bFireGun : 1;
			unsigned char bDriverLastFrame : 1;
			unsigned char bNeverUseSmallerRemovalRange : 1;
			unsigned char bIsRCVehicle : 1;

			unsigned char bAlwaysSkidMarks : 1;
			unsigned char bEngineBroken : 1;
			unsigned char bVehicleCanBeTargetted : 1;
			unsigned char bPartOfAttackWave : 1;
			unsigned char bWinchCanPickMeUp : 1;
			unsigned char bImpounded : 1;
			unsigned char bVehicleCanBeTargettedByHS : 1;
			unsigned char bSirenOrAlarm : 1;

			unsigned char bHasGangLeaningOn : 1;
			unsigned char bGangMembersForRoadBlock : 1;
			unsigned char bDoesProvideCover : 1;
			unsigned char bMadDriver : 1;
			unsigned char bUpgradedStereo : 1;
			unsigned char bConsideredByPlayer : 1;
			unsigned char bPetrolTankIsWeakPoint : 1;
			unsigned char bDisableParticles : 1;

			unsigned char bHasBeenResprayed : 1;
			unsigned char bUseCarCheats : 1;
			unsigned char bDontSetColourWhenRemapping : 1;
			unsigned char bUsedForReplay : 1;
		} dwFlags;
	};

	uint32_t dwCreationTime;			// 1072-1076
	uint8_t byteColor1;					// 1076-1077
	uint8_t byteColor2;					// 1077-1078
	uint8_t byteColor3;					// 1078-1079
	uint8_t byteColor4;					// 1079-1080
	PADDING(_pad203, 40);				// 1080-1120
	PED_TYPE *pDriver;					// 1120-1124
	PED_TYPE *pPassengers[8];			// 1124-1156
	PADDING(_pad206, 12);				// 1156-1168
	uint32_t pFireObject;				// 1168-1172
	uint8_t byteMoreFlags;				// 1192-1193
	PADDING(_pad275_, 31); 				// 1193-1224
	float fHealth;						// 1224-1228
	PADDING(_pad207, 4);				// 1228-1232
	uint32_t dwTractor;					// 1228-1232
	uint32_t dwTrailer;					// 1232-1236
	PADDING(_pad208, 48);				// 1236-1284
	uint32_t dwDoorsLocked;				// 1284-1288
	PADDING(_pad209, 172);				// 1288-1460
	union {
		struct {
			PADDING(_pad245, 1);		// 1460-1461
			uint8_t byteWheelStatus[4]; // 1461-1465
			uint8_t byteDoorStatus[6];	// 1465-1471
			uint8_t byteDamageUnk;		// 1471-1472
			uint32_t dwLightStatus;		// 1471-1475
			uint32_t dwPanelStatus;		// 1475-1479
		};
		struct {
			float fTrainSpeed;			// 1460-1464
			PADDING(_pad210, 16);		// 1464-1479
		};
	};
	PADDING(_pad213, 30);				// 1479-1508
	uint32_t *VehicleAttachedFront;		// 1508-1512
	uint32_t *VehicleAttachedBottom; 	// 1512-1516
	PADDING(_pad211, 128);				// 1516-1644
	uint8_t byteBikeWheelStatus[2];		// 1644-1645
} VEHICLE_TYPE;

#pragma pack(1)
typedef struct _VEHICLE_PARAMS_STATUS
{
	int bEngine;
	int bLights;
	int bAlarm;
	int bDoors;
	int bBonnet;
	int bBoot;
	int bObjective;
	//doors
	int bDriver;
	int bPassenger;
	int bBackleft;
	int bBackright;
} VEHICLE_PARAMS_STATUS;

#pragma pack(1)
typedef struct _REMOVE_BUILDING_DATA
{
    unsigned short usModelIndex;
    VECTOR vecPos;
    float fRange;
} REMOVE_BUILDING_DATA;

#pragma pack(1)
typedef struct _VEH_MODELINFO
{	
	PADDING(_pad0, 15);				// 00-15
	RpMaterial* unkMat;				// 15-19
	PADDING(_pad15, 41);			// 19-60
	RpMaterial* rpMaterialPlate; 	// 60-64
	char szTextPlate[8];			// 64-72
	char unkchar;					// 73-74
	PADDING(_pad74, 100);
} VEH_MODELINFO;

#pragma pack(1)
typedef struct _RES_ENTRY_OBJ
{
	PADDING(_pad0, 48); 	// 0-48
	uintptr_t validate; 	//48-52
	PADDING(_pad1, 4); 		//52-56
} RES_ENTRY_OBJ;
static_assert(sizeof(_RES_ENTRY_OBJ) == 56);

#pragma pack(1)
typedef struct
{
	ENTITY_TYPE* pEntity;
	float fDamage;
	uint32_t dwBodyPart;
	uint32_t dwWeapon;
	bool bSpeak;
} stPedDamageResponse;

#pragma pack(1)
typedef struct _BULLET_DATA
{
	uint32_t unk;
	VECTOR vecOrigin;
	VECTOR vecPos;
	VECTOR vecOffset;
	ENTITY_TYPE* pEntity;
} BULLET_DATA;

#pragma pack(1)
typedef struct _WIDGET_TYPE
{
	PADDING(_pad000, 77);	// 000-077
	bool bEnabled;			// 077-078
	PADDING(_pad078, 90);	// 078-168
} WIDGET_TYPE;

//-----------------------------------------------------------

#define	VEHICLE_SUBTYPE_CAR				1
#define	VEHICLE_SUBTYPE_BIKE			2
#define	VEHICLE_SUBTYPE_HELI			3
#define	VEHICLE_SUBTYPE_BOAT			4
#define	VEHICLE_SUBTYPE_PLANE			5
#define	VEHICLE_SUBTYPE_PUSHBIKE		6
#define	VEHICLE_SUBTYPE_TRAIN			7

//-----------------------------------------------------------

#define TRAIN_PASSENGER_LOCO			538
#define TRAIN_FREIGHT_LOCO				537
#define TRAIN_PASSENGER					570
#define TRAIN_FREIGHT					569
#define TRAIN_TRAM						449
#define HYDRA							520

//-----------------------------------------------------------

#define ACTION_WASTED					55
#define ACTION_DEATH					54
#define ACTION_INCAR					50
#define ACTION_NORMAL					1
#define ACTION_SCOPE					12
#define ACTION_NONE						0 

//-----------------------------------------------------------

#define WEAPON_MODEL_BRASSKNUCKLE		331 // was 332
#define WEAPON_MODEL_GOLFCLUB			333
#define WEAPON_MODEL_NITESTICK			334
#define WEAPON_MODEL_KNIFE				335
#define WEAPON_MODEL_BAT				336
#define WEAPON_MODEL_SHOVEL				337
#define WEAPON_MODEL_POOLSTICK			338
#define WEAPON_MODEL_KATANA				339
#define WEAPON_MODEL_CHAINSAW			341
#define WEAPON_MODEL_DILDO				321
#define WEAPON_MODEL_DILDO2				322
#define WEAPON_MODEL_VIBRATOR			323
#define WEAPON_MODEL_VIBRATOR2			324
#define WEAPON_MODEL_FLOWER				325
#define WEAPON_MODEL_CANE				326
#define WEAPON_MODEL_GRENADE			342 // was 327
#define WEAPON_MODEL_TEARGAS			343 // was 328
#define WEAPON_MODEL_MOLOTOV			344 // was 329
#define WEAPON_MODEL_COLT45				346
#define WEAPON_MODEL_SILENCED			347
#define WEAPON_MODEL_DEAGLE				348
#define WEAPON_MODEL_SHOTGUN			349
#define WEAPON_MODEL_SAWEDOFF			350
#define WEAPON_MODEL_SHOTGSPA			351
#define WEAPON_MODEL_UZI				352
#define WEAPON_MODEL_MP5				353
#define WEAPON_MODEL_AK47				355
#define WEAPON_MODEL_M4					356
#define WEAPON_MODEL_TEC9				372
#define WEAPON_MODEL_RIFLE				357
#define WEAPON_MODEL_SNIPER				358
#define WEAPON_MODEL_ROCKETLAUNCHER		359
#define WEAPON_MODEL_HEATSEEKER			360
#define WEAPON_MODEL_FLAMETHROWER		361
#define WEAPON_MODEL_MINIGUN			362
#define WEAPON_MODEL_SATCHEL			363
#define WEAPON_MODEL_BOMB				364
#define WEAPON_MODEL_SPRAYCAN			365
#define WEAPON_MODEL_FIREEXTINGUISHER	366
#define WEAPON_MODEL_CAMERA				367
#define WEAPON_MODEL_NIGHTVISION		368	// newly added
#define WEAPON_MODEL_INFRARED			369	// newly added
#define WEAPON_MODEL_JETPACK			370	// newly added
#define WEAPON_MODEL_PARACHUTE			371

#define WEAPON_MODEL_PARACHUTE			371
#define WEAPON_FIST                        0
#define WEAPON_BRASSKNUCKLE                1
#define WEAPON_GOLFCLUB                    2
#define WEAPON_NITESTICK                3
#define WEAPON_KNIFE                    4
#define WEAPON_BAT                        5
#define WEAPON_SHOVEL                    6
#define WEAPON_POOLSTICK                7
#define WEAPON_KATANA                    8
#define WEAPON_CHAINSAW                    9
#define WEAPON_DILDO                    10
#define WEAPON_DILDO2                    11
#define WEAPON_VIBRATOR                    12
#define WEAPON_VIBRATOR2                13
#define WEAPON_FLOWER                    14
#define WEAPON_CANE                        15
#define WEAPON_GRENADE                    16
#define WEAPON_TEARGAS                    17
#define WEAPON_MOLTOV                    18
#define WEAPON_COLT45                    22
#define WEAPON_SILENCED                    23
#define WEAPON_DEAGLE                    24
#define WEAPON_SHOTGUN                    25
#define WEAPON_SAWEDOFF                    26
#define WEAPON_SHOTGSPA                    27
#define WEAPON_UZI                        28
#define WEAPON_MP5                        29
#define WEAPON_AK47                        30
#define WEAPON_M4                        31
#define WEAPON_TEC9                        32
#define WEAPON_RIFLE                    33
#define WEAPON_SNIPER                    34
#define WEAPON_ROCKETLAUNCHER            35
#define WEAPON_HEATSEEKER                36
#define WEAPON_FLAMETHROWER                37
#define WEAPON_MINIGUN                    38
#define WEAPON_SATCHEL                    39
#define WEAPON_BOMB                        40
#define WEAPON_SPRAYCAN                    41
#define WEAPON_FIREEXTINGUISHER            42
#define WEAPON_CAMERA                    43
#define WEAPON_PARACHUTE                46
#define WEAPON_VEHICLE                    49
#define WEAPON_HELIBLADES				50
#define WEAPON_EXPLOSION				51
#define WEAPON_DROWN                    53
#define WEAPON_COLLISION                54
#define WEAPON_UNKNOWN					0xFF

#define OBJECT_PARACHUTE				3131
#define OBJECT_CJ_CIGGY					1485
#define OBJECT_DYN_BEER_1				1486
#define OBJECT_CJ_BEER_B_2				1543
#define OBJECT_CJ_PINT_GLASS			1546

// PED BONES
enum ePedBones
{
	BONE_PELVIS1 = 1,
	//BONE_PELVIS = 2,
	BONE_SPINE1 = 3,
	BONE_UPPERTORSO = 4,
	BONE_NECK = 5,
	BONE_HEAD2 = 6,
	BONE_HEAD1 = 7,
	BONE_HEAD = 8,
	BONE_RIGHTUPPERTORSO = 21,
	BONE_RIGHTSHOULDER = 22,
	BONE_RIGHTELBOW = 23,
	BONE_RIGHTWRIST = 24,
	BONE_RIGHTHAND = 25,
	BONE_RIGHTTHUMB = 26,
	BONE_LEFTUPPERTORSO = 31,
	BONE_LEFTSHOULDER = 32,
	BONE_LEFTELBOW = 33,
	BONE_LEFTWRIST = 34,
	BONE_LEFTHAND = 35,
	BONE_LEFTTHUMB = 36,
	BONE_LEFTHIP = 41,
	BONE_LEFTKNEE = 42,
	BONE_LEFTANKLE = 43,
	BONE_LEFTFOOT = 44,
	BONE_RIGHTHIP = 51,
	BONE_RIGHTKNEE = 52,
	BONE_RIGHTANKLE = 53,
	BONE_RIGHTFOOT = 54,
};
