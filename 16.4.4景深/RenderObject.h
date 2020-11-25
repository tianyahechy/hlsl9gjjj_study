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

	//渲染到纹理需要使用的变量
	//装入反射纹理
	LPDIRECT3DTEXTURE9					m_pRTTTexture;	
	//控制反射纹理的接口
	LPD3DXRENDERTOSURFACE				m_pRTTInterface;
	//页面
	LPDIRECT3DSURFACE9					m_pRTTSurface;

	//物体的中心坐标
	D3DXVECTOR3							m_vBaryCentric;
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
	HRESULT RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice, D3DXVECTOR3 vPosition, float fTime);
	LPDIRECT3DTEXTURE9 returnTexture() { return m_pRTTTexture;}
	HRESULT setTexture(LPDIRECT3DTEXTURE9);

public:
	void Init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice, TCHAR* FileName);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC * pBackSurfaceDesc );
	void OnFrameMove(IDirect3DDevice9* pd3dDevice, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();

};

