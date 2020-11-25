#pragma once
#include <d3dx9.h>

#define FILEHEADTAG		"SHMFILE"

enum RESTYPE
{
	//ģ���ļ�����
	//.x�ļ���ʽ
	ENU_MODELX					= 0x00100001,
	//��������
	ENU_MODELOTHER				= 0x00100002,

	//�����ļ�
	//��ɫ��ͼ
	ENU_TEXCOLORMAP				= 0x00200001,
	//������ͼ
	ENU_TEXNORMALMAP			= 0x00200002,
	//��͹��ͼ
	ENU_TEXBUMPMAP				= 0x00200003,
	//�߹���ͼ
	ENU_SPECULARMAP				= 0x00200004,
	
	//Ч���ļ�
	ENU_FXFILE					= 0x00300001,
	//�����ļ�
	ENU_SGT						= 0x00400001,
	ENU_DLS						= 0x00400002,
	//�����ļ�
	ENU_ANIMATION				= 0x00500001,

};

//.shm�ļ����ļ�ͷ����ṹ
struct SSHADERMODULEFILEHEAD
{
	//�ļ���ʶ
	char		szHead[16];
	//�߰汾
	int			s_iVerHigh;
	//�Ͱ汾
	int			s_iVerLow;
	//δʹ��
	int			s_iPack;
	//�ļ�����
	int			s_iResourceFileNum;
	//У���ֽ�
	int			s_iCheck[2];
};

struct SRESOURCEINFO
{
	TCHAR	s_szFileName[24];
	RESTYPE	s_enumResType;
	int		s_iOffset;
};
//��Ⱦ����
struct RENDEROBJECT_D3DVERTEX
{
	//����λ��
	D3DXVECTOR3	position;
	//��������
	float u, v;
};

//��Ӱ�����Ӱ������Ľṹ
struct SHADOWMAPOBJECT_D3DVERTEX
{
	//����λ��
	D3DXVECTOR3	position;
	//��������
	float u, v;
};
