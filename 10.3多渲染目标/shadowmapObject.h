#pragma once
#include "common.h"
#include <string>
#include <vector>
#include "SDKmesh.h"
#include "SDKmisc.h"
#include "myMesh.h"

class CShadowMapObject
{
public:
	CShadowMapObject();
	~CShadowMapObject();

private:

	//ģ�͵�mesh�ӿ�����
	//������صĻ���ģ������
	std::vector<myMesh>					_meshVector;
	//��Ⱦ������������ʹ�õı���
	//��Ⱦ������ı����ͽӿ�
	//�������������Ⱦ�Ľӿ�
	//LPD3DXRENDERTOSURFACE				m_pRTTInterface;
	//�������ĵ�0��ҳ��
	LPDIRECT3DSURFACE9					m_pRTTSurface;
	//��һ�˳�����ϵ�е���������
	D3DXMATRIX							m_matWorld;
	D3DXMATRIX							m_matView;
	D3DXMATRIX							m_matProject;
	//��Ⱦ��������Ҫʹ�õ���������
	D3DXMATRIX							m_matViewRTT;
	D3DXMATRIX							m_matProjectRTT;
	//Ч���ӿ�
	LPD3DXEFFECT						m_pEffect;

public:	

	//��Ϊ��Ⱦ���������ģ���ṩ�����Ľӿڣ�������Ⱦ���������������
	LPDIRECT3DTEXTURE9 getTexture() { return m_pColorTexture; }
	LPDIRECT3DSURFACE9 getRTTSurface() { return	m_pRTTSurface; }
public:
	//��ʼ��
	//�ж��Ƿ��ʼ��shadowmap
	bool getBInitShadowMap();
	//�����Ƿ��Ѿ���ʼ��shadowmap
	void setBInitShadowMap(bool bInitShadowMap);
	void initShadowMap(
		std::vector<std::string> strModelNameVec,			//ģ����������
		std::vector<D3DXMATRIXA16> matWorldVec,				//������������
		std::vector<std::string> strShadowModelNameVec,		//����Ӱ����������
		std::wstring wstrFXName,
		IDirect3DDevice9 * pd3dDevice);
private:
	//����fx�ļ���
	void setFXFileName( std::wstring wstrFXName);
	//��ʼ��shader�ļ�
	HRESULT initShaderFile(IDirect3DDevice9 * pd3dDevice);
	//����RTT
	HRESULT createRTT(IDirect3DDevice9 * pd3dDevice);	
	HRESULT loadModelVector( 
		IDirect3DDevice9* pd3dDevice,
		std::vector<std::string> strModelNameVec,	//ģ����������
		std::vector<D3DXMATRIXA16> matWorldVec,		//������������
		std::vector<std::string> strShadowModelNameVec		//����Ӱ����������
		);
public:
	//ÿ֡	
	//��Ⱦ������
	void RenderToTexture(
		LPDIRECT3DDEVICE9 pd3dDevice,
		D3DXMATRIXA16* pView,
		D3DXMATRIXA16* pProject,
		D3DXMATRIXA16* pViewRTT,
		D3DXMATRIXA16* pProjectRTT);	
	//��������
	void renderObjectVector(IDirect3DDevice9* pd3dDevice);
private:
	//���õ�һ�˳����������
	void setFirstCameraMatrixVector( D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject);
	//���ƹ�۲췽����Ϣ���ݸ��ڵ���
	void setLightMatrixVector(D3DXMATRIXA16* pViewRTT, D3DXMATRIXA16* pProjectRTT);
	//��Ⱦ������
	HRESULT RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice);

public:
	//����
	void OnLostDevice();
	void OnDestroyDevice();

private:
	//FX�ļ���
	std::wstring _wstrFXName;
	//�Ƿ��Ѿ�������Ӱ���ͼ
	bool _bInitShadowMap;

public:
	//���������С
	void setTextureSize(int sizeX, int sizeY);
private:
	//�����С
	int									m_TextureSizeX;
	int									m_TextureSizeY;

	//λ������
	LPDIRECT3DTEXTURE9					m_pRTTTexturePosition;
	//��������
	LPDIRECT3DTEXTURE9					m_pRTTTextureNormal;
	//�������
	LPDIRECT3DTEXTURE9					m_pRTTTextureDepth;

	LPDIRECT3DTEXTURE9					m_pColorTexture;

public:
	void OnFrameMove(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime, D3DXVECTOR4* pvEye);
};

