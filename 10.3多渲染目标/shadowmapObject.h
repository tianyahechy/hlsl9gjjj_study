#pragma once
#include "common.h"
#include <string>
#include <vector>
#include "SDKmesh.h"
#include "SDKmisc.h"
#include "myMesh.h"

class CShadowMapObject
{
public:
	CShadowMapObject();
	~CShadowMapObject();

private:

	//模型的mesh接口数组
	//所有相关的绘制模型数组
	std::vector<myMesh>					_meshVector;
	//渲染到纹理所必须使用的变量
	//渲染到纹理的变量和接口
	//控制深度纹理渲染的接口
	//LPD3DXRENDERTOSURFACE				m_pRTTInterface;
	//深度纹理的第0层页面
	LPDIRECT3DSURFACE9					m_pRTTSurface;
	//第一人称坐标系中的三个矩阵
	D3DXMATRIX							m_matWorld;
	D3DXMATRIX							m_matView;
	D3DXMATRIX							m_matProject;
	//渲染到纹理需要使用的三个矩阵
	D3DXMATRIX							m_matViewRTT;
	D3DXMATRIX							m_matProjectRTT;
	//效果接口
	LPD3DXEFFECT						m_pEffect;

public:	

	//作为渲染结果向其他模块提供参数的接口，即将渲染结果交给其他函数
	LPDIRECT3DTEXTURE9 getTexture() { return m_pColorTexture; }
	LPDIRECT3DSURFACE9 getRTTSurface() { return	m_pRTTSurface; }
public:
	//初始化
	//判断是否初始化shadowmap
	bool getBInitShadowMap();
	//设置是否已经初始化shadowmap
	void setBInitShadowMap(bool bInitShadowMap);
	void initShadowMap(
		std::vector<std::string> strModelNameVec,			//模型名称数组
		std::vector<D3DXMATRIXA16> matWorldVec,				//世界坐标数组
		std::vector<std::string> strShadowModelNameVec,		//有阴影的名称数组
		std::wstring wstrFXName,
		IDirect3DDevice9 * pd3dDevice);
private:
	//设置fx文件名
	void setFXFileName( std::wstring wstrFXName);
	//初始化shader文件
	HRESULT initShaderFile(IDirect3DDevice9 * pd3dDevice);
	//创建RTT
	HRESULT createRTT(IDirect3DDevice9 * pd3dDevice);	
	HRESULT loadModelVector( 
		IDirect3DDevice9* pd3dDevice,
		std::vector<std::string> strModelNameVec,	//模型名称数组
		std::vector<D3DXMATRIXA16> matWorldVec,		//世界坐标数组
		std::vector<std::string> strShadowModelNameVec		//有阴影的名称数组
		);
public:
	//每帧	
	//渲染到纹理
	void RenderToTexture(
		LPDIRECT3DDEVICE9 pd3dDevice,
		D3DXMATRIXA16* pView,
		D3DXMATRIXA16* pProject,
		D3DXMATRIXA16* pViewRTT,
		D3DXMATRIXA16* pProjectRTT);	
	//绘制物体
	void renderObjectVector(IDirect3DDevice9* pd3dDevice);
private:
	//设置第一人称摄像机参数
	void setFirstCameraMatrixVector( D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject);
	//将灯光观察方向信息传递给遮挡物
	void setLightMatrixVector(D3DXMATRIXA16* pViewRTT, D3DXMATRIXA16* pProjectRTT);
	//渲染到纹理
	HRESULT RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice);

public:
	//销毁
	void OnLostDevice();
	void OnDestroyDevice();

private:
	//FX文件名
	std::wstring _wstrFXName;
	//是否已经生成阴影深度图
	bool _bInitShadowMap;

public:
	//设置纹理大小
	void setTextureSize(int sizeX, int sizeY);
private:
	//纹理大小
	int									m_TextureSizeX;
	int									m_TextureSizeY;

	//位置纹理
	LPDIRECT3DTEXTURE9					m_pRTTTexturePosition;
	//法线纹理
	LPDIRECT3DTEXTURE9					m_pRTTTextureNormal;
	//深度纹理
	LPDIRECT3DTEXTURE9					m_pRTTTextureDepth;

	LPDIRECT3DTEXTURE9					m_pColorTexture;

public:
	void OnFrameMove(D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime, D3DXVECTOR4* pvEye);
};

