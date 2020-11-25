
#pragma once
#include "common.h"
class CShadowMapObject
{
public:
	CShadowMapObject();
	~CShadowMapObject();
private:	//���㻺����
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
	//��Ⱦ���������ʹ�õı���
	//�����С
	int									m_TextureSize;
	//��Ⱦ������ı����ͽӿ�
	//�����������
	LPDIRECT3DTEXTURE9					m_pRTTTexture;
	//�������������Ⱦ�ӿ�
	LPD3DXRENDERTOSURFACE				m_pRTTInterface;
	//�������ĵ�0��ҳ��
	LPDIRECT3DSURFACE9					m_pRTTSurface;
	//��Ⱦ��������Ҫʹ�õ���������
	//��Ⱦ������ʹ�õ��������
	D3DXMATRIX							m_matWorldRTT;
	//��Ⱦ������ʹ�õĹ۲����
	D3DXMATRIX							m_matViewRTT;
	//��Ⱦ������ʹ�õ�ͶӰ����
	D3DXMATRIX							m_matProjectRTT;	//�������������
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
public:
	//��Ⱦ������
	HRESULT RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIXA16* );
	//��Ϊ��Ⱦ���������ģ���ṩ�����Ľӿڣ�������Ⱦ���������������
	LPDIRECT3DTEXTURE9 ReturnTexture() { return m_pRTTTexture; }
	HRESULT SetLightParameter(D3DXVECTOR3*, D3DXVECTOR3*, D3DXVECTOR3*);

public:
	void Init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice, TCHAR* FileName);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice, float fAspectRatio);
	HRESULT OnResetDeviceRTT(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc);
	void OnFrameMove(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();
};

