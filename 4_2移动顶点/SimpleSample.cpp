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
#include "RenderObject.h"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

//����
D3DXMATRIXA16				g_matWorld;
D3DXMATRIXA16				g_matView;
D3DXMATRIXA16				g_matProject;
D3DXMATRIXA16				g_matWorldRTT;
D3DXMATRIXA16				g_matViewRTT;
D3DXMATRIXA16				g_matProjectRTT;
//�ڵ�����
CRenderObject				g_OccluderObject;

IDirect3DDevice9*			g_device = NULL;

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

	std::wstring wstrModelName1 = L"D:\\lifei\\test\\hlsl�߼�����\\��Ӱ\\ResFile_1_Occluder\\model.x";
	std::wstring wstrImgName1 = L"D:\\lifei\\test\\hlsl�߼�����\\��Ӱ\\ResFile_1_Occluder\\colormap.jpg";
	std::wstring wstrFXName1 = L"D:\\lifei\\test\\hlsl�߼�����\\��Ӱ\\ResFile_1_Occluder\\filefx.fx";

	g_OccluderObject.setFile(wstrModelName1, wstrImgName1, wstrFXName1);
	hr = g_OccluderObject.OnCreateDevice(pd3dDevice);
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
	D3DXMatrixPerspectiveFovLH(&g_matProject, D3DX_PI / 4, fAspectRatio, 1.0f, 100.0f);
	g_matProjectRTT = g_matProject;

	g_OccluderObject.OnResetDevice(pd3dDevice, fAspectRatio);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime,  void* pUserContext)
{
	D3DXMatrixIdentity(&g_matWorld);
	D3DXMatrixIdentity(&g_matWorldRTT);
	//����Ĺ۲����
	D3DXVECTOR3 vEyePt(20.0 * sin(fTime / 3.0), 12, 20 * cos(fTime / 3.0));
	D3DXVECTOR3 vLookatPt(0, 0, 0);
	D3DXVECTOR3 vUpVec(0, 1, 0);
	D3DXMatrixLookAtLH(&g_matView, &vEyePt, &vLookatPt, &vUpVec);
	//���õ�һ�˳����������
	g_OccluderObject.setFirstCameraMatrixVector(&g_matWorld, &g_matView, &g_matProject);

	//�ڵ���ͽ�������˶�
	g_OccluderObject.OnFrameMove(fTime);
	

}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
	
    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		//��Ⱦ����
		g_OccluderObject.OnFrameRender(pd3dDevice);
        V( pd3dDevice->EndScene() );
    }
}

//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
	g_OccluderObject.OnLostDevice();
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
	g_OccluderObject.OnDestroyDevice();
}


