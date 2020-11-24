#pragma once
#include "common.h"
#include <string>

class CRenderObject
{
public:
	CRenderObject();
	~CRenderObject();

private:
	//顶点缓冲区
	LPDIRECT3DVERTEXBUFFER9				m_pVB;
	//索引缓冲区
	LPDIRECT3DINDEXBUFFER9				m_pIB;
	//顶点索引个数
	DWORD								m_dwNumIndices;
	//顶点个数
	DWORD								m_dwNumVertices;
	//顶点声明接口
	LPDIRECT3DVERTEXDECLARATION9		m_pDecl;
	//模型的mesh接口
	LPD3DXMESH							m_pMeshSysMemory;
	//模型的中心
	D3DXVECTOR3							m_vCenter;
	//模型的半径
	FLOAT								m_fObjectRadius;
	//纹理
	IDirect3DTexture9*					m_pColorTexture;
	DWORD								m_dwMaterial;
	//效果接口
	LPD3DXEFFECT						m_pEffect;
	//本渲染对象的资源个数
	int									m_iResNum;
	//保存资源名字	
	SRESOURCEINFO *						m_pResourceInfo;
public:
	//装入资源管理文件
	HRESULT LoadResource(TCHAR* FileName);
	//通过资源类型获取资源名
	TCHAR* GetResourceName(RESTYPE enuResType);
	//模型的半径
	float GetRadius() { return m_fObjectRadius; }
	void setLightParament(D3DXVECTOR4 * vPosition, D3DXVECTOR4 * vpDirect);

public:
	void Init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice, TCHAR* FileName);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice );
	void OnFrameMove(IDirect3DDevice9* pd3dDevice, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime, D3DXVECTOR4* pvEye);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();

};

