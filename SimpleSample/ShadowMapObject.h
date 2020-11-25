
#pragma once
#include "common.h"
class CShadowMapObject
{
public:
	CShadowMapObject();
	~CShadowMapObject();
private:	//顶点缓冲区
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
	//渲染到纹理必须使用的变量
	//纹理大小
	int									m_TextureSize;
	//渲染到纹理的变量和接口
	//保存深度纹理
	LPDIRECT3DTEXTURE9					m_pRTTTexture;
	//控制深度纹理渲染接口
	LPD3DXRENDERTOSURFACE				m_pRTTInterface;
	//深度纹理的第0层页面
	LPDIRECT3DSURFACE9					m_pRTTSurface;
	//渲染到纹理需要使用的三个矩阵
	//渲染到纹理使用的世界矩阵
	D3DXMATRIX							m_matWorldRTT;
	//渲染到纹理使用的观察矩阵
	D3DXMATRIX							m_matViewRTT;
	//渲染到纹理使用的投影矩阵
	D3DXMATRIX							m_matProjectRTT;	//物体的重心坐标
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
public:
	//渲染到纹理
	HRESULT RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIXA16* );
	//作为渲染结果向其他模块提供参数的接口，即将渲染结果交给其他函数
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

