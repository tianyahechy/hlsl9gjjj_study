#pragma once
#include "common.h"

class CRenderObject
{
public:
	CRenderObject();
	~CRenderObject();

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
	//װ�������
	LPDIRECT3DTEXTURE9					m_pRTTTexture;
	//��������Ľӿ�
	LPD3DXRENDERTOSURFACE				m_pRTTInterface;
	//ҳ��
	LPDIRECT3DSURFACE9					m_pRTTSurface;
	//��Ⱦ��������Ҫʹ�õ���������
	D3DXMATRIX							m_matWorldRTT;
	D3DXMATRIX							m_matViewRTT;
	D3DXMATRIX							m_matProjectRTT;
	//�������������
	D3DXVECTOR3							m_vBaryCentric;
	//Ч���ӿ�
	LPD3DXEFFECT						m_pEffect;
	//����Ⱦ�������Դ����
	int									m_iResNum;
	//������Դ����	
	SRESOURCEINFO *						m_pResourceInfo;
public:
	//װ����Դ�����ļ�
	HRESULT LoadResource(TCHAR* FileName);
	//ͨ����Դ���ͻ�ȡ��Դ��
	TCHAR* GetResourceName(RESTYPE enuResType);
	//��Ⱦ������
	HRESULT RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice, D3DXVECTOR3 vPosition, float fTime);
	//��Ϊ��Ⱦ���������ģ���ṩ�����Ľӿڣ�������Ⱦ���������������
	LPDIRECT3DTEXTURE9 ReturnTexture() { return m_pRTTTexture; }
	HRESULT SetTexture(LPDIRECT3DTEXTURE9);
	//ģ�͵İ뾶
	float GetRadius() { return m_fObjectRadius; }
	HRESULT LoadMesh(IDirect3DDevice9* pd3dDevice, WCHAR* wstrFileName, ID3DXMesh** ppMesh);

public:
	void Init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice, TCHAR* FileName);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice );
	HRESULT OnResetDeviceRTT(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc);
	void OnFrameMove(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice, LPDIRECT3DTEXTURE9 pTexture);
	void OnLostDevice();
	void OnDestroyDevice();

	/*
	//��Ⱦ������
private:
	//��Ⱦ������������ʹ�õı���
	//�����С
	int  m_TextureSize;
	//��Ⱦ������ı����ͽӿ�
	//λ������
	LPDIRECT3DTEXTURE9 m_pRTTTexturePosition;
	//��������
	LPDIRECT3DTEXTURE9 m_pRTTTextureNormal;
	//�������
	LPDIRECT3DTEXTURE9 m_pRTTTextureDepth;
	//���Ʒ�������Ľӿ�
	LPD3DXRENDERTOSURFACE m_pRTTInterface;
	//ҳ��
	LPDIRECT3DSURFACE9 m_pRTTSurface;
public:
	//��Ⱦ��������ָ�����ĸ�����
	HRESULT RenderToTexture(
		LPDIRECT3DDEVICE9 pd3dDevice, 
		D3DXMATRIXA16* pWorld,
		D3DXMATRIXA16* pView,
		D3DXMATRIXA16* pProject,
		double fTime
		);

		*/

};

