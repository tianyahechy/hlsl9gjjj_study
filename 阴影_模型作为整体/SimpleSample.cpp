//--------------------------------------------------------------------------------------
// File: SimpleSample.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTCamera.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"
#include "Board.h"
//#include "RenderObject.h"
#include "ShadowMapObject.h"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

//矩阵
D3DXMATRIXA16				g_matView;
D3DXMATRIXA16				g_matProject;
D3DXMATRIXA16				g_matViewRTT;
D3DXMATRIXA16				g_matProjectRTT;
//遮挡物体
CShadowMapObject			g_OccluderObject;

//调试物体
CBoard						g_TextureDebug;
IDirect3DDevice9*			g_device = NULL;


std::wstring wstrFXName1 = L"D:\\lifei\\test\\hlsl高级精讲\\阴影\\ResFile_1_Occluder\\filefx.fx";
std::string strModelName1 = "D:\\lifei\\test\\hlsl高级精讲\\阴影\\ResFile_1_Occluder\\model.x";
std::string strModelName2 = "D:\\lifei\\test\\hlsl高级精讲\\DirectXSamples\\Meshes\\tiger.x";
std::string strModelName3 = "D:\\lifei\\test\\hlsl高级精讲\\DirectXSamples\\ShadowMap\\Media\\Misc\\car.x";
std::string strModelName4 = "D:\\lifei\\test\\hlsl高级精讲\\DirectXSamples\\ShadowMap\\Media\\UI\\arrow.x";
std::string strModelName5 = "D:\\lifei\\test\\hlsl高级精讲\\DirectXSamples\\ShadowMap\\Media\\Airplane\\airplane 2.x";

//所有要单独绘制的模型数组
int size = 3;
std::vector<std::string> strModelNameVec;	//模型名称数组
std::vector<D3DXMATRIXA16> matWorldVec;		//世界坐标数组
std::vector<std::string> strShadowModelVec; //有阴影的模型数组
//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext);

HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext );
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext );
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnD3D9LostDevice( void* pUserContext );
void CALLBACK OnD3D9DestroyDevice( void* pUserContext );

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Set DXUT callbacks
    DXUTSetCallbackFrameMove( OnFrameMove );

    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );

    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"SimpleSample" );
    DXUTCreateDevice( true, 640, 480 );
    DXUTMainLoop(); // Enter into the DXUT render loop

    return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
	g_device = pd3dDevice;
    HRESULT hr;

	strModelNameVec.clear();
	matWorldVec.clear();
	strModelNameVec.resize(size);
	matWorldVec.resize(size);
	strShadowModelVec.clear();

	D3DXMATRIXA16 theMatrix;
	D3DXMatrixIdentity(&theMatrix);

	strModelNameVec[0] = strModelName3;
	strModelNameVec[1] = strModelName4;
	strModelNameVec[2] = strModelName5;
	for (int i = 0; i < size; i++)
	{
		matWorldVec[i] = theMatrix;
	}
	strShadowModelVec.push_back(strModelName5);

	std::wstring wstrImgName3 = L"D:\\lifei\\test\\hlsl高级精讲\\阴影\\ResFile_Debug\\colormap.bmp";
	std::wstring wstrFXName3 = L"D:\\lifei\\test\\hlsl高级精讲\\阴影_模型作为整体\\ResFile_Debug\\filefx.fx";
	g_TextureDebug.setFile(wstrImgName3, wstrFXName3);
	hr = g_TextureDebug.OnCreateDevice(pd3dDevice);
	if (FAILED(hr))
	{
		return hr;
	}
    return S_OK;
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice,
                                    const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;
    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
	D3DXMatrixPerspectiveFovLH(&g_matProject, D3DX_PI / 4, fAspectRatio, 1.0f, 1000.0f);
	g_matProjectRTT = g_matProject;

	g_TextureDebug.OnResetDevice(pd3dDevice);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime,  void* pUserContext)
{
	//物体的观察矩阵
	D3DXVECTOR3 vEyePt(10.0 * sin(fTime / 3.0), 10, 10 * cos(fTime / 3.0));
	D3DXVECTOR3 vLookatPt(0, 0, 0);
	D3DXVECTOR3 vUpVec(0, 1, 0);
	D3DXMatrixLookAtLH(&g_matView, &vEyePt, &vLookatPt, &vUpVec);
	//设置灯光位置，照射方向
	D3DXVECTOR3 vLightEyePt(0 , 0, 5 );
	D3DXVECTOR3 vLightLookatPt(0, 0, 0);
	D3DXVECTOR3 vLightUpVec(0, 1, 0);
	D3DXMatrixLookAtLH(&g_matViewRTT, &vLightEyePt, &vLightLookatPt, &vLightUpVec);
	

}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

	bool bInitShadowMap = g_OccluderObject.getBInitShadowMap();
	if (!bInitShadowMap)
	{
		g_OccluderObject.initShadowMap(strModelNameVec, matWorldVec, strShadowModelVec, wstrFXName1, pd3dDevice);
		g_OccluderObject.setBInitShadowMap(true);
	}
	g_OccluderObject.RenderToTexture(pd3dDevice, &g_matView, &g_matProject,&g_matViewRTT, &g_matProjectRTT);
    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		//渲染物体
		g_OccluderObject.renderObjectVector(pd3dDevice);
		g_TextureDebug.OnFrameRender(pd3dDevice, g_OccluderObject.ReturnTexture(), 0);
        V( pd3dDevice->EndScene() );
    }
}

//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
	g_OccluderObject.OnLostDevice();
	g_TextureDebug.OnLostDevice();
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
	g_OccluderObject.OnDestroyDevice();
	g_TextureDebug.OnDestroyDevice();
}


