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
	void OnFrameMove(IDirect3DDevice9* pd3dDevice, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice);
	//void OnFrameRender(IDirect3DDevice9* pd3dDevice, LPDIRECT3DTEXTURE9 pTexture);
	void OnLostDevice();
	void OnDestroyDevice();

private:
	//Ч���ļ�
	std::wstring _szEffectFileName;
	//ģ���ļ�
	std::wstring _szModelFileName;
	//�����ļ�
	std::wstring _szTextureFileName;
public:
	//����Ч���ļ���ģ���ļ��������ļ�
	void setFiles(std::wstring szEffectFileName, std::wstring szModelFileName, std::wstring szTextureFileName);

private:
	//����shader�����ӿ�
	LPD3DXCONSTANTTABLE			m_pContantTableVS;
	//����shader���
	LPDIRECT3DVERTEXSHADER9		m_pVertexShader;
	//������ɫ�������ӿ�
	LPD3DXCONSTANTTABLE			m_pConstantTablePS;
	//������ɫ�����
	LPDIRECT3DPIXELSHADER9		m_pPixelShader;
	std::wstring				m_pVertexShaderFileName;
	std::wstring				m_pPixelShaderFileName;
	

public:
	void setVertexShaderFile(std::wstring wstrVertexFileName);
	void setPixelShaderFile(std::wstring wstrPixelFileName);
	void setTextureName(std::wstring wstrTextureName);
};

