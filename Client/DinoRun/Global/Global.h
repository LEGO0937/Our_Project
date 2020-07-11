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

//Max,Min Force amount
#define MAX_FORCE 2000
#define MIDDLE_FORCE 1300
#define MIN_FORCE 600
#define MIN_VELOCITY 15.0f
#define MAX_VELOCITY 50.f

//���� ���� vs Meter or Kilometer
#define METER_PER_UNIT 0.1
#define UNIT_PER_METER 10
#define UNIT_PER_KILOMETER 10000

//CheckPoint max  ,   2 track
#define CHECKPOINT_GOAL 363

enum SceneType { Default_Scene = 0, Start_Scene, Lobby_Scene, Room_Scene, Game_Scene,ItemGame_Scene, End_Scene };
enum ModelType { Default = 0, CheckPoint, Player, Fence, Item_Box, Item_Banana, 
	Item_Meat, Item_Mud, Item_Stone,Item_Mugen
};
enum BodyType {BodyType_Dynamic = 0, BodyType_Kinematic, BodyType_Static};

#define MAX_ROTATE_RADIAN 0.26   //�� 15��
#define MIN_ROTATE_RADIAN -0.26  //�� -15��

#define ROTATE_ACCELERATING 0.086   //�� ���ӷ� �ʴ� 5��   2�� 15��