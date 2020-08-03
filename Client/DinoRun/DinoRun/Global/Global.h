#pragma once

//PSO 
#define PSO_CUBE_MAP 0
#define PSO_SKIN_MESH 1
#define PSO_MODEL_INSTANCING 2
#define PSO_BILLBOARD 3
#define PSO_TERRAIN 4
#define PSO_SKIN_MESH_INSTANCING 5

#define PSO_WIRE 6
#define PSO_WIRE_INSTANCING 7

#define PSO_MINIMAP 21
//PSO ui
#define PSO_UI 8
#define PSO_UI_GAUGE 9
#define PSO_UI_NUMBER 10

#define PSO_PARTICLE 11

#define PSO_EFFECT 22

//PSO shadow
#define PSO_SHADOW_SKIN_MESH 12
#define PSO_SHADOW_MODEL_INSTANCING 13
#define PSO_SHADOW_BILLBOARD 14
#define PSO_SHADOW_TERRAIN 15
#define PSO_SHADOW_SKINED_INSTANCING 16

//PSO pont
#define PSO_PONT 17

//PSO use computeShader
#define PSO_HORZ_BLUR 18
#define PSO_VERT_BLUR 19
#define PSO_PARTICLE_CALC 20
#define PSO_MOTION_BLUR 28
//PSO Draw VelocityMap 
#define PSO_VELOCITY_SKIN_MESH 23
#define PSO_VELOCITY_MODEL_INSTANCING 24
#define PSO_VELOCITY_BILLBOARD 25
#define PSO_VELOCITY_TERRAIN 26
#define PSO_VELOCITY_SKINED_INSTANCING 27
#define PSO_VELOCITY_CUBEMAP 29

//Max,Min Force amount
#define MAX_FORCE 3200
#define MIDDLE_FORCE 1300
#define MIN_FORCE 600
#define MIN_VELOCITY 25.0f
#define MAX_VELOCITY 50.0f

//단위 유닛 vs Meter or Kilometer
#define METER_PER_UNIT 0.1
#define UNIT_PER_METER 10
#define UNIT_PER_KILOMETER 10000
#define KILOMETER_PER_METER 0.001

//CheckPoint max  ,   2 track
#define CHECKPOINT_GOAL 363

enum SceneType { Default_Scene = 0, Start_Scene, Lobby_Scene, Room_Scene, Game_Scene,ItemGame_Scene, End_Scene };
enum ModelType { Default = 0, CheckPoint, Player, Fence, Item_Box, Item_Banana, 
	Item_Meat, Item_Mud, Item_Stone, Item_Meteorite,Item_Mound};
enum BodyType {BodyType_Dynamic = 0, BodyType_Kinematic, BodyType_Static};

#define MAX_ROTATE_RADIAN 0.26   //약 15도
#define MIN_ROTATE_RADIAN -0.26  //약 -15도

#define ROTATE_ACCELERATING 0.086   //각 가속력 초당 5도   2초 15도


//#define isDebug

//#define isConnectedToServer

//shaderName
#define _MODEL_SHADER 0
#define _BILLBOARD_SHADER 1
#define _TREE_SHADER 2
#define _BLOCK_SHADER 3
#define _FENCE_SHADER 4
#define _ITEM_SHADER 5
#define _MEAT_SHADER 6
#define _BANANA_SHADER 7
#define _METEORITE_SHADER 8
#define _MOUND_SHADER 9
#define _MUD_SHADER 10
#define _STONE_SHADER 11
#define _SKINED_SHADER 12
#define _PLAYER_SHADER 13

#define _OBJECTS_SHADER 99

//------EVENT HANDLER INSTRUCTION

#define _ADD_OBJECT 0
#define _DELETE_OBJECT 1
#define _ADD_PARTICLE 2
#define _DISENABLE_OBJECT 3


//-----particle pattern
#define SPAWN 0
#define HEAT_EFFECT 1
#define BOX_PARTICLE 2
#define MEAT_PARTICLE 3
#define DUST_PARTICLE 4
#define STONE_PARTICLE 5 