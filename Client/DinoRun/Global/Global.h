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


//Max,Min Force amount
#define MAX_FORCE 2000
#define MIDDLE_FORCE 1300
#define MIN_FORCE 600


//단위 유닛 vs Meter or Kilometer
#define METER_PER_UNIT 0.1
#define UNIT_PER_METER 10
#define UNIT_PER_KILOMETER 10000


//CheckPoint max
#define CHECKPOINT_GOAL 362
enum SceneType { Default_Scene = 0, Start_Scene, Lobby_Scene, Room_Scene, Game_Scene,ItemGame_Scene, End_Scene };
enum ModelType { Default = 0, CheckPoint, Player, Fence, Item_Box, Item_Banana, 
	Item_Meat, Item_Oil, Item_Stone,Item_Mugen
};


#define MAX_ROTATE_RADIAN 0.26   //약 15도
#define MIN_ROTATE_RADIAN -0.26  //약 -15도

#define ROTATE_ACCELERATING 0.086   //각 가속력 초당 5도   2초 15도