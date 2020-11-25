#pragma once
#include "common.h"
#include <string>

class CShadowMapObject
{
public:
	CShadowMapObject();
	~CShadowMapObject();

private:
	//���㻺����
	LPDIRECT3DVERTEXBUFFER9				m_pVB;
	//����������
	LPDIRECT3DINDEXBUFFER9				m_pIB;
	//������������
	DWORD								m_dwNumIndices;
	//�������
	DWORD								m_dwNumVertices;
	//���������ӿ�
	LPDIRECT3DVERTEXDECLARATION9		m_pDecl;
	//ģ�͵�mesh�ӿ�
	LPD3DXMESH							m_pMeshSysMemory;
	//ģ�͵�����
	D3DXVECTOR3							m_vCenter;
	//ģ�͵İ뾶
	FLOAT								m_fObjectRadius;
	//����
	IDirect3DTexture9*					m_pColorTexture;
	//��Ⱦ������������ʹ�õı���
	//�����С
	int									m_TextureSize;
	//��Ⱦ������ı����ͽӿ�
	//�����������
	LPDIRECT3DTEXTURE9					m_pRTTTexture;
	//�������������Ⱦ�Ľӿ�
	LPD3DXRENDERTOSURFACE				m_pRTTInterface;
	//�������ĵ�0��ҳ��
	LPDIRECT3DSURFACE9					m_pRTTSurface;
	//��һ�˳�����ϵ�е���������
	D3DXMATRIX							m_matWorld;
	D3DXMATRIX							m_matView;
	D3DXMATRIX							m_matProject;
	//��Ⱦ��������Ҫʹ�õ���������
	D3DXMATRIX							m_matWorldRTT;
	D3DXMATRIX							m_matViewRTT;
	D3DXMATRIX							m_matProjectRTT;
	//�������������
	D3DXVECTOR3							m_vBaryCentric;
	//Ч���ӿ�
	LPD3DXEFFECT						m_pEffect;

public:	
	//ģ�͵İ뾶
	float GetRadius() { return m_fObjectRadius; }	
	//��Ⱦ������
	HRESULT RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice);
	//��Ϊ��Ⱦ���������ģ���ṩ�����Ľӿڣ�������Ⱦ���������������
	LPDIRECT3DTEXTURE9 ReturnTexture() { return m_pRTTTexture; }
	//���õ�һ�˳����������
	void setFirstCameraMatrixVector(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject);
	//���ƹ�۲췽����Ϣ���ݸ��ڵ���
	void setLightMatrixVector(D3DXMATRIXA16* pWorldRTT, D3DXMATRIXA16* pViewRTT, D3DXMATRIXA16* pProjectRTT);

public:
	void Init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice, float fAspectRatio );
	void OnFrameMove( double fTime);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();

	//����ģ����������ͼƬ������fx��
	void setFile(std::wstring wstrModelName, std::wstring wstrImgName, std::wstring wstrFXName);

private:
	//ģ����
	std::wstring _wstrModelName;
	//����ͼƬ��
	std::wstring _wstrImgName;
	//FX�ļ���
	std::wstring _wstrFXName;

};

