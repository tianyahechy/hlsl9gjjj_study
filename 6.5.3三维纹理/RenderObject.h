#pragma once
#include "common.h"
#include <string>

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
	//��ɫ����
	LPDIRECT3DVOLUMETEXTURE9			m_pVolumeTexture;
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
	void Init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice, TCHAR* FileName);
	HRESULT OnResetDevice();
	void OnFrameMove(IDirect3DDevice9* pd3dDevice, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();

};

