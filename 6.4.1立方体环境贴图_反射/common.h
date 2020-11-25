#pragma once
#include <d3dx9.h>

#define FILEHEADTAG		"SHMFILE"

enum RESTYPE
{
	//模型文件类型
	//.x文件格式
	ENU_MODELX					= 0x00100001,
	//其他类型
	ENU_MODELOTHER				= 0x00100002,

	//纹理文件
	//颜色贴图
	ENU_TEXCOLORMAP				= 0x00200001,
	//法线贴图
	ENU_TEXNORMALMAP			= 0x00200002,
	//凹凸贴图
	ENU_TEXBUMPMAP				= 0x00200003,
	//高光贴图
	ENU_SPECULARMAP				= 0x00200004,
	ENU_CUBEMAP					= 0x00200005,
	
	//效果文件
	ENU_FXFILE					= 0x00300001,
	ENU_FXFILEBIN				= 0x00300001,
	ENU_VERTEXSHADER			= 0x00300005,
	ENU_VERTEXSHADERBIN			= 0x00300006,
	ENU_PIXLESHADER				= 0x00300009,
	ENU_PIXLESHADERBIN			= 0x00300010,
	//音乐文件
	ENU_SGT						= 0x00400001,
	ENU_DLS						= 0x00400002,
	//动画文件
	ENU_ANIMATION				= 0x00500001,

};

//.shm文件的文件头代码结构
struct SSHADERMODULEFILEHEAD
{
	//文件标识
	char		szHead[16];
	//高版本
	int			s_iVerHigh;
	//低版本
	int			s_iVerLow;
	//未使用
	int			s_iPack;
	//文件个数
	int			s_iResourceFileNum;
	//校验字节
	int			s_iCheck[2];
};

struct SRESOURCEINFO
{
	TCHAR	s_szFileName[24];
	RESTYPE	s_enumResType;
	int		s_iOffset;
};
struct RENDEROBJECT_D3DVERTEX
{
	//顶点位置
	D3DXVECTOR3	Position;
	D3DXVECTOR3 Normal;
	//纹理坐标
	float u, v;	
};

struct CBOXOBJECT_D3DVERTEX
{
	//顶点位置
	D3DXVECTOR3	Position;
	D3DXVECTOR3 Normal;
	//纹理坐标
	float u, v;
};

