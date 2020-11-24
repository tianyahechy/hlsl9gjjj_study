#pragma once
#include "common.h"
#include <string>

class CRenderObjectDS
{
public:
	CRenderObjectDS();
	~CRenderObjectDS();

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

	//��Ⱦ��������Ҫʹ�õı���
	//װ�뷴������
	LPDIRECT3DTEXTURE9					m_pRTTTextureDepth;	
	//���Ʒ�������Ľӿ�
	LPD3DXRENDERTOSURFACE				m_pRTTInterfaceDepth;
	//ҳ��
	LPDIRECT3DSURFACE9					m_pRTTSurfaceDepth;

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
	//ģ�͵İ뾶
	float GetRadius() { return m_fObjectRadius; }
	HRESULT RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, float fTime);
	LPDIRECT3DTEXTURE9 returnTexture() { return m_pRTTTextureDepth;}
public:
	void Init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice, TCHAR* FileName);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC * pBackSurfaceDesc );
	void OnFrameMove(IDirect3DDevice9* pd3dDevice, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime, D3DXVECTOR4*);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();

};

