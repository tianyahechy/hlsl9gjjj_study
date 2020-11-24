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
	
	//效果接口
	LPD3DXEFFECT						m_pEffect;

public:	
	//模型的半径
	float GetRadius() { return m_fObjectRadius; }	

	//设置第一人称摄像机参数
	void setFirstCameraMatrixVector(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject);

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
	//世界坐标系
	D3DXMATRIXA16 m_matWorld;
	//摄像机坐标系
	D3DXMATRIXA16 m_matView;
	//投影坐标系
	D3DXMATRIXA16 m_matProject;

};

