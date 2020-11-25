#pragma once
#include "common.h"
#include <string>

class CShadowMapObject
{
public:
	CShadowMapObject();
	~CShadowMapObject();

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
	//保存深度纹理
	LPDIRECT3DTEXTURE9					m_pRTTTexture;
	//控制深度纹理渲染的接口
	LPD3DXRENDERTOSURFACE				m_pRTTInterface;
	//深度纹理的第0层页面
	LPDIRECT3DSURFACE9					m_pRTTSurface;
	//第一人称坐标系中的三个矩阵
	D3DXMATRIX							m_matWorld;
	D3DXMATRIX							m_matView;
	D3DXMATRIX							m_matProject;
	//渲染到纹理需要使用的三个矩阵
	D3DXMATRIX							m_matWorldRTT;
	D3DXMATRIX							m_matViewRTT;
	D3DXMATRIX							m_matProjectRTT;
	//物体的重心坐标
	D3DXVECTOR3							m_vBaryCentric;
	//效果接口
	LPD3DXEFFECT						m_pEffect;

public:	
	//模型的半径
	float GetRadius() { return m_fObjectRadius; }	
	//渲染到纹理
	HRESULT RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice);
	//作为渲染结果向其他模块提供参数的接口，即将渲染结果交给其他函数
	LPDIRECT3DTEXTURE9 ReturnTexture() { return m_pRTTTexture; }
	//设置第一人称摄像机参数
	void setFirstCameraMatrixVector(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject);
	//将灯光观察方向信息传递给遮挡物
	void setLightMatrixVector(D3DXMATRIXA16* pWorldRTT, D3DXMATRIXA16* pViewRTT, D3DXMATRIXA16* pProjectRTT);

public:
	void Init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice, float fAspectRatio );
	void OnFrameMove( double fTime);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();

	//设置模型名，纹理图片名，和fx名
	void setFile(std::wstring wstrModelName, std::wstring wstrImgName, std::wstring wstrFXName);

private:
	//模型名
	std::wstring _wstrModelName;
	//纹理图片名
	std::wstring _wstrImgName;
	//FX文件名
	std::wstring _wstrFXName;

};

