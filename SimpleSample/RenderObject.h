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
	//渲染到纹理所必须使用的变量
	//纹理大小
	int									m_TextureSize;
	//渲染到纹理的变量和接口
	//装入的纹理
	LPDIRECT3DTEXTURE9					m_pRTTTexture;
	//控制纹理的接口
	LPD3DXRENDERTOSURFACE				m_pRTTInterface;
	//页面
	LPDIRECT3DSURFACE9					m_pRTTSurface;
	//渲染到纹理需要使用的三个矩阵
	D3DXMATRIX							m_matWorldRTT;
	D3DXMATRIX							m_matViewRTT;
	D3DXMATRIX							m_matProjectRTT;
	//物体的重心坐标
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
	//渲染到纹理
	HRESULT RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice, D3DXVECTOR3 vPosition, float fTime);
	//作为渲染结果向其他模块提供参数的接口，即将渲染结果交给其他函数
	LPDIRECT3DTEXTURE9 ReturnTexture() { return m_pRTTTexture; }
	HRESULT SetTexture(LPDIRECT3DTEXTURE9);
	//模型的半径
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
	//效果文件
	std::wstring _szEffectFileName;
	//模型文件
	std::wstring _szModelFileName;
	//纹理文件
	std::wstring _szTextureFileName;
public:
	//设置效果文件，模型文件和纹理文件
	void setFiles(std::wstring szEffectFileName, std::wstring szModelFileName, std::wstring szTextureFileName);

private:
	//顶点shader常量接口
	LPD3DXCONSTANTTABLE			m_pContantTableVS;
	//顶点shader句柄
	LPDIRECT3DVERTEXSHADER9		m_pVertexShader;
	//像素着色器常量接口
	LPD3DXCONSTANTTABLE			m_pConstantTablePS;
	//像素着色器句柄
	LPDIRECT3DPIXELSHADER9		m_pPixelShader;
	std::wstring				m_pVertexShaderFileName;
	std::wstring				m_pPixelShaderFileName;
	

public:
	void setVertexShaderFile(std::wstring wstrVertexFileName);
	void setPixelShaderFile(std::wstring wstrPixelFileName);
	void setTextureName(std::wstring wstrTextureName);
};

