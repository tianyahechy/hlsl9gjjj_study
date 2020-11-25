//--------------------------------------------------------------------------------------
// File: ShadowMap.cpp
//
// Starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTsettingsdlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 


#define ShadowMap_SIZE 512

#define HELPTEXTCOLOR D3DXCOLOR( 0.0f, 1.0f, 0.3f, 1.0f )

LPCWSTR g_aszMeshFile[] =
{
    L"room.x",
    L"airplane\\airplane 2.x",
    L"misc\\car.x",
    L"misc\\sphere.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"ring.x",
    L"ring.x",
};

#define NUM_OBJ (sizeof(g_aszMeshFile)/sizeof(g_aszMeshFile[0]))

D3DXMATRIXA16 g_amInitObjWorld[NUM_OBJ] =
{
    D3DXMATRIXA16( 3.5f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.5f, 0.0f, 0.0f, 0.0f, 0.0f,
                   1.0f ),
    D3DXMATRIXA16( 0.43301f, 0.25f, 0.0f, 0.0f, -0.25f, 0.43301f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 5.0f,
                   1.33975f, 0.0f, 1.0f ),
    D3DXMATRIXA16( 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, -14.5f, -7.1f, 0.0f,
                   1.0f ),
    D3DXMATRIXA16( 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, -7.0f, 0.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, 5.0f, 0.2f, 5.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, 5.0f, 0.2f, -5.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, -5.0f, 0.2f, 5.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, -5.0f, 0.2f, -5.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, 14.0f, 0.2f, 14.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, 14.0f, 0.2f, -14.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, -14.0f, 0.2f, 14.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, -14.0f, 0.2f, -14.0f,
                   1.0f ),
    D3DXMATRIXA16( 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.9f, 0.0f, -14.5f, -9.0f, 0.0f,
                   1.0f ),
    D3DXMATRIXA16( 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.9f, 0.0f, 14.5f, -9.0f, 0.0f,
                   1.0f ),
};


D3DVERTEXELEMENT9 g_aVertDecl[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};


//-----------------------------------------------------------------------------
// Name: class CObj
// Desc: Encapsulates a mesh object in the scene by grouping its world matrix
//       with the mesh.
//-----------------------------------------------------------------------------
#pragma warning( disable : 4324 )
struct CObj
{
    CDXUTXFileMesh m_Mesh;
    D3DXMATRIXA16 m_mWorld;
};

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

ID3DXEffect*                    g_pEffect = NULL;       // D3DX effect interface
CDXUTDialogResourceManager      g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg                 g_SettingsDlg;          // Device settings dialog
CDXUTDialog                     g_HUD;                  // dialog for standard controls
CFirstPersonCamera              g_VCamera;              // View camera
CFirstPersonCamera              g_LCamera;              // Camera obj to help adjust light
CObj g_Obj[NUM_OBJ];         // Scene object meshes
LPDIRECT3DVERTEXDECLARATION9    g_pVertDecl = NULL;// Vertex decl for the sample
LPDIRECT3DTEXTURE9              g_pTexDef = NULL;       // Default texture for objects
D3DLIGHT9                       g_Light;                // The spot light in the scene
LPDIRECT3DTEXTURE9              g_pShadowMap = NULL;    // Texture to which the shadow map is rendered
LPDIRECT3DSURFACE9              g_pDSShadow = NULL;     // Depth-stencil buffer for rendering to shadow map
float                           g_fLightFov;            // FOV of the spot light (in radian)
D3DXMATRIXA16                   g_mShadowProj;          // Projection matrix for shadow map

bool                            g_bCameraPerspective    // Indicates whether we should render view from
    = true;               // the camera's or the light's perspective


//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
void InitializeDialogs();
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                 void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );

void CALLBACK OnLostDevice( void* pUserContext );
void CALLBACK OnDestroyDevice( void* pUserContext );
void RenderScene( IDirect3DDevice9* pd3dDevice, bool bRenderShadow, float fElapsedTime, const D3DXMATRIX* pmView,
                  const D3DXMATRIX* pmProj );


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Initialize the camera
    g_VCamera.SetScalers( 0.01f, 15.0f );
    g_LCamera.SetScalers( 0.01f, 8.0f );
    g_VCamera.SetRotateButtons( true, false, false );
    g_LCamera.SetRotateButtons( false, false, true );

    // Set up the view parameters for the camera
    D3DXVECTOR3 vFromPt = D3DXVECTOR3( 0.0f, 5.0f, -18.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
    g_VCamera.SetViewParams( &vFromPt, &vLookatPt );

    vFromPt = D3DXVECTOR3( 0.0f, 0.0f, -12.0f );
    vLookatPt = D3DXVECTOR3( 0.0f, -2.0f, 1.0f );
    g_LCamera.SetViewParams( &vFromPt, &vLookatPt );

    // Initialize the spot light
    g_fLightFov = D3DX_PI / 2.0f;

    g_Light.Diffuse.r = 1.0f;
    g_Light.Diffuse.g = 1.0f;
    g_Light.Diffuse.b = 1.0f;
    g_Light.Diffuse.a = 1.0f;
    g_Light.Position = D3DXVECTOR3( -8.0f, -8.0f, 0.0f );
    g_Light.Direction = D3DXVECTOR3( 1.0f, -1.0f, 0.0f );
    D3DXVec3Normalize( ( D3DXVECTOR3* )&g_Light.Direction, ( D3DXVECTOR3* )&g_Light.Direction );
    g_Light.Range = 10.0f;
    g_Light.Theta = g_fLightFov / 2.0f;
    g_Light.Phi = g_fLightFov / 2.0f;

    DXUTSetCallbackD3D9DeviceCreated( OnCreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnFrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnLostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackFrameMove( OnFrameMove );

    InitializeDialogs();

    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );

    // Initialize DXUT and create the desired Win32 window and Direct3D 
    // device for the application. Calling each of these functions is optional, but they
    // allow you to set several options which control the behavior of the framework.
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTCreateWindow( L"ShadowMap" );
    DXUTCreateDevice( true, 640, 480 );

    // Pass control to DXUT for handling the message pump and 
    // dispatching render calls. DXUT will call your FrameMove 
    // and FrameRender callback when there is idle time between handling window messages.
    DXUTMainLoop();

    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Sets up the dialogs
//--------------------------------------------------------------------------------------
void InitializeDialogs()
{
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );

}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnDestroyDevice callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                 void* pUserContext )
{
    HRESULT hr;


    V_RETURN( g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnD3D9CreateDevice( pd3dDevice ) );
  
    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

    // Read the D3DX effect file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"ShadowMap.fx" ) );

    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
    V_RETURN( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, dwShaderFlags,
                                        NULL, &g_pEffect, NULL ) );

    // Create vertex declaration
    V_RETURN( pd3dDevice->CreateVertexDeclaration( g_aVertDecl, &g_pVertDecl ) );

    // Initialize the meshes
    for( int i = 0; i < NUM_OBJ; ++i )
    {
        V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, g_aszMeshFile[i] ) );
        if( FAILED( g_Obj[i].m_Mesh.Create( pd3dDevice, str ) ) )
            return DXUTERR_MEDIANOTFOUND;
        V_RETURN( g_Obj[i].m_Mesh.SetVertexDecl( pd3dDevice, g_aVertDecl ) );
        g_Obj[i].m_mWorld = g_amInitObjWorld[i];
    }

    // Initialize the light mesh
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"spotlight.x" ) );
  
    // World transform to identity
    D3DXMATRIXA16 mIdent;
    D3DXMatrixIdentity( &mIdent );
    V_RETURN( pd3dDevice->SetTransform( D3DTS_WORLD, &mIdent ) );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// reset, which will happen after a lost device scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the device is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice,
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9ResetDevice() );
    V_RETURN( g_SettingsDlg.OnD3D9ResetDevice() );

    if( g_pEffect )
        V_RETURN( g_pEffect->OnResetDevice() );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
    g_VCamera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 100.0f );
    g_LCamera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 100.0f );

    // Create the default texture (used when a triangle does not use a texture)
    V_RETURN( pd3dDevice->CreateTexture( 1, 1, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTexDef,
                                         NULL ) );
    D3DLOCKED_RECT lr;
    V_RETURN( g_pTexDef->LockRect( 0, &lr, NULL, 0 ) );
    *( LPDWORD )lr.pBits = D3DCOLOR_RGBA( 255, 255, 255, 255 );
    V_RETURN( g_pTexDef->UnlockRect( 0 ) );

    // Restore the scene objects
    for( int i = 0; i < NUM_OBJ; ++i )
        V_RETURN( g_Obj[i].m_Mesh.RestoreDeviceObjects( pd3dDevice ) );
  
    // Restore the effect variables
    V_RETURN( g_pEffect->SetVector( "g_vLightDiffuse", ( D3DXVECTOR4* )&g_Light.Diffuse ) );
    V_RETURN( g_pEffect->SetFloat( "g_fCosTheta", cosf( g_Light.Theta ) ) );

    // Create the shadow map texture
    V_RETURN( pd3dDevice->CreateTexture( ShadowMap_SIZE, ShadowMap_SIZE,
                                         1, D3DUSAGE_RENDERTARGET,
                                         D3DFMT_R32F,
                                         D3DPOOL_DEFAULT,
                                         &g_pShadowMap,
                                         NULL ) );

    // Create the depth-stencil buffer to be used with the shadow map
    // We do this to ensure that the depth-stencil buffer is large
    // enough and has correct multisample type/quality when rendering
    // the shadow map.  The default depth-stencil buffer created during
    // device creation will not be large enough if the user resizes the
    // window to a very small size.  Furthermore, if the device is created
    // with multisampling, the default depth-stencil buffer will not
    // work with the shadow map texture because texture render targets
    // do not support multisample.
    DXUTDeviceSettings d3dSettings = DXUTGetDeviceSettings();
    V_RETURN( pd3dDevice->CreateDepthStencilSurface( ShadowMap_SIZE,
                                                     ShadowMap_SIZE,
                                                     d3dSettings.d3d9.pp.AutoDepthStencilFormat,
                                                     D3DMULTISAMPLE_NONE,
                                                     0,
                                                     TRUE,
                                                     &g_pDSShadow,
                                                     NULL ) );

    // Initialize the shadow projection matrix
    D3DXMatrixPerspectiveFovLH( &g_mShadowProj, g_fLightFov, 1, 0.01f, 100.0f );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    // Update the camera's position based on user input 
    g_VCamera.FrameMove( fElapsedTime );
    g_LCamera.FrameMove( fElapsedTime );

}


//--------------------------------------------------------------------------------------
// Renders the scene onto the current render target using the current
// technique in the effect.
//--------------------------------------------------------------------------------------
void RenderScene( IDirect3DDevice9* pd3dDevice, bool bRenderShadow, float fElapsedTime, const D3DXMATRIX* pmView,
                  const D3DXMATRIX* pmProj )
{
    HRESULT hr;

    // Set the projection matrix
    V( g_pEffect->SetMatrix( "g_mProj", pmProj ) );

    // Update the light parameters in the effect
    {
        // Freely moveable light. Get light parameter
        // from the light camera.
        D3DXVECTOR3 v = *g_LCamera.GetEyePt();
        D3DXVECTOR4 v4;
        D3DXVec3Transform( &v4, &v, pmView );
        V( g_pEffect->SetVector( "g_vLightPos", &v4 ) );
        *( D3DXVECTOR3* )&v4 = *g_LCamera.GetWorldAhead();
        v4.w = 0.0f;  // Set w 0 so that the translation part doesn't come to play
        D3DXVec4Transform( &v4, &v4, pmView );  // Direction in view space
        D3DXVec3Normalize( ( D3DXVECTOR3* )&v4, ( D3DXVECTOR3* )&v4 );
        V( g_pEffect->SetVector( "g_vLightDir", &v4 ) );
    }
  

    // Clear the render buffers
    V( pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                          0x000000ff, 1.0f, 0L ) );

    if( bRenderShadow )
        V( g_pEffect->SetTechnique( "RenderShadow" ) );

    // Begin the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        if( !bRenderShadow )
            V( g_pEffect->SetTechnique( "RenderScene" ) );

        // Render the objects
        for( int obj = 0; obj < NUM_OBJ; ++obj )
        {
            D3DXMATRIXA16 mWorldView = g_Obj[obj].m_mWorld;
            D3DXMatrixMultiply( &mWorldView, &mWorldView, pmView );
            V( g_pEffect->SetMatrix( "g_mWorldView", &mWorldView ) );

            LPD3DXMESH pMesh = g_Obj[obj].m_Mesh.GetMesh();
            UINT cPass;
            V( g_pEffect->Begin( &cPass, 0 ) );
            for( UINT p = 0; p < cPass; ++p )
            {
                V( g_pEffect->BeginPass( p ) );

                for( DWORD i = 0; i < g_Obj[obj].m_Mesh.m_dwNumMaterials; ++i )
                {
                    D3DXVECTOR4 vDif( g_Obj[obj].m_Mesh.m_pMaterials[i].Diffuse.r,
                                      g_Obj[obj].m_Mesh.m_pMaterials[i].Diffuse.g,
                                      g_Obj[obj].m_Mesh.m_pMaterials[i].Diffuse.b,
                                      g_Obj[obj].m_Mesh.m_pMaterials[i].Diffuse.a );
                    V( g_pEffect->SetVector( "g_vMaterial", &vDif ) );
                    if( g_Obj[obj].m_Mesh.m_pTextures[i] )
                        V( g_pEffect->SetTexture( "g_txScene", g_Obj[obj].m_Mesh.m_pTextures[i] ) )
                    else
                        V( g_pEffect->SetTexture( "g_txScene", g_pTexDef ) )
                    V( g_pEffect->CommitChanges() );
                    V( pMesh->DrawSubset( i ) );
                }
                V( g_pEffect->EndPass() );
            }
            V( g_pEffect->End() );
        }
  
        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, DXUT will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

    //
    // Compute the view matrix for the light
    // This changes depending on the light mode
    // (free movement or attached)
    //
    D3DXMATRIXA16 mLightView = *g_LCamera.GetViewMatrix();
  
    //
    // Render the shadow map
    //
    LPDIRECT3DSURFACE9 pOldRT = NULL;
    V( pd3dDevice->GetRenderTarget( 0, &pOldRT ) );
    LPDIRECT3DSURFACE9 pShadowSurf;
    if( SUCCEEDED( g_pShadowMap->GetSurfaceLevel( 0, &pShadowSurf ) ) )
    {
        pd3dDevice->SetRenderTarget( 0, pShadowSurf );
        SAFE_RELEASE( pShadowSurf );
    }
    LPDIRECT3DSURFACE9 pOldDS = NULL;
    if( SUCCEEDED( pd3dDevice->GetDepthStencilSurface( &pOldDS ) ) )
        pd3dDevice->SetDepthStencilSurface( g_pDSShadow );

    {
        RenderScene( pd3dDevice, true, fElapsedTime, &mLightView, &g_mShadowProj );
    }

    if( pOldDS )
    {
        pd3dDevice->SetDepthStencilSurface( pOldDS );
        pOldDS->Release();
    }
    pd3dDevice->SetRenderTarget( 0, pOldRT );
    SAFE_RELEASE( pOldRT );

    //
    // Now that we have the shadow map, render the scene.
    //
    const D3DXMATRIX* pmView = g_bCameraPerspective ? g_VCamera.GetViewMatrix() :
        &mLightView;

    // Initialize required parameter
    V( g_pEffect->SetTexture( "g_txShadow", g_pShadowMap ) );
    // Compute the matrix to transform from view space to
    // light projection space.  This consists of
    // the inverse of view matrix * view matrix of light * light projection matrix
    D3DXMATRIXA16 mViewToLightProj;
    mViewToLightProj = *pmView;
    D3DXMatrixInverse( &mViewToLightProj, NULL, &mViewToLightProj );
    D3DXMatrixMultiply( &mViewToLightProj, &mViewToLightProj, &mLightView );
    D3DXMatrixMultiply( &mViewToLightProj, &mViewToLightProj, &g_mShadowProj );
    V( g_pEffect->SetMatrix( "g_mViewToLightProj", &mViewToLightProj ) );

    {
        CDXUTPerfEventGenerator g( DXUT_PERFEVENTCOLOR, L"Scene" );
        RenderScene( pd3dDevice, false, fElapsedTime, pmView, g_VCamera.GetProjMatrix() );
    }
    g_pEffect->SetTexture( "g_txShadow", NULL );
}

void CALLBACK OnLostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9LostDevice();
    g_SettingsDlg.OnD3D9LostDevice();
  
    if( g_pEffect )
        g_pEffect->OnLostDevice();
  
    SAFE_RELEASE( g_pDSShadow );
    SAFE_RELEASE( g_pShadowMap );
    SAFE_RELEASE( g_pTexDef );

    for( int i = 0; i < NUM_OBJ; ++i )
        g_Obj[i].m_Mesh.InvalidateDeviceObjects();
   
}

void CALLBACK OnDestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9DestroyDevice();
    g_SettingsDlg.OnD3D9DestroyDevice();
    SAFE_RELEASE( g_pEffect );
 
    SAFE_RELEASE( g_pVertDecl );

    SAFE_RELEASE( g_pEffect );

    for( int i = 0; i < NUM_OBJ; ++i )
        g_Obj[i].m_Mesh.Destroy();
}
