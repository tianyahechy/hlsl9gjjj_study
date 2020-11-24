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
	IDirect3DTexture9*					m_pColorTexture;
	
	//Ч���ӿ�
	LPD3DXEFFECT						m_pEffect;

public:	
	//ģ�͵İ뾶
	float GetRadius() { return m_fObjectRadius; }	

	//���õ�һ�˳����������
	void setFirstCameraMatrixVector(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject);

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
	//��������ϵ
	D3DXMATRIXA16 m_matWorld;
	//���������ϵ
	D3DXMATRIXA16 m_matView;
	//ͶӰ����ϵ
	D3DXMATRIXA16 m_matProject;

};

