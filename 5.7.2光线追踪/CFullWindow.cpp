//-----------------------------------------------------------------------------
// �ļ���CFULLWINDOW.Cpp
// ���ܣ�Board ����ʵ��
//-----------------------------------------------------------------------------
//#include "dxstdafx.h"
#include "DXUT.h"
#include "CFullWindow.h"

CFULLWINDOW::CFULLWINDOW( )
  {
    m_pVB            = NULL;
    m_pIB            = NULL;
    m_dwNumIndices   = 0;
    m_dwNumVertices  = 0;
    m_pDecl          = NULL;
    m_pEffect        = NULL;
  }

CFULLWINDOW::~CFULLWINDOW( )
  {
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    m_dwNumIndices   = 0;
    m_dwNumVertices  = 0;
    SAFE_RELEASE( m_pDecl );
    SAFE_RELEASE( m_pEffect );
    
  }
//
void CFULLWINDOW::Init( )
  {
  }

HRESULT CFULLWINDOW::OnCreateDevice( IDirect3DDevice9* pd3dDevice )
  {
    HRESULT hr = S_OK;
    TCHAR szAppPath[MAX_PATH];
    GetCurrentDirectory( MAX_PATH, szAppPath );
    // װ������
    if( FAILED( hr = D3DXCreateTextureFromFileEx( pd3dDevice,  L"ColorMap.bmp", D3DX_DEFAULT, D3DX_DEFAULT, 
      D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 
      NULL, NULL, &m_pColorTexture ) )) return hr;  
    // ------------------------ ���µķ�ʽ���������� -------------------------------
    LPD3DXMESH           pMeshSysMemClone = NULL;
    D3DVERTEXELEMENT9 decl[] =
      {
        { 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0   }, 
        { 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0   },
        { 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0   },
        D3DDECL_END()
      };
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &m_pDecl ))) return hr;
    // ---------------------------------------------------------------------------
    LPD3DXBUFFER pCode = NULL;
    DWORD dwFlag = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION ;
    if( FAILED( hr = D3DXCreateEffectFromFile( pd3dDevice , L"FileFx.fx" , NULL , NULL , 
      dwFlag , NULL , &m_pEffect , &pCode )))    
      {
        LPVOID pBufErr = pCode->GetBufferPointer();
        char* ErrInfo = ( char* ) pBufErr;
        pCode->Release( );
        return hr;
      }
    return hr;
  }

HRESULT CFULLWINDOW::OnResetDevice( IDirect3DDevice9* pd3dDevice , D3DXVECTOR4 d3dViewPort )
  {
    HRESULT hr = S_OK;
    if( FAILED( hr = CreateSquare( pd3dDevice ))) return hr;
	D3DXHANDLE texHandle = m_pEffect->GetParameterByName(0, "texColorTexture");
    hr = m_pEffect->SetTexture(texHandle,m_pColorTexture );
	D3DXHANDLE viewPortHandle = m_pEffect->GetParameterByName(0, "g_v4Viewport");
    hr = m_pEffect->SetVector(viewPortHandle, &d3dViewPort );
    return hr;
  }

void CFULLWINDOW::OnFrameMove( D3DXMATRIXA16* pmatInvView , float fTime )
  {
    HRESULT hr ;
	D3DXHANDLE timeHandle = m_pEffect->GetParameterByName(0,"g_fTime" );
    hr = m_pEffect->SetFloat(timeHandle, fTime );
	D3DXHANDLE matViewHandle = m_pEffect->GetParameterByName(0, "g_matInvView" );
    hr = m_pEffect->SetMatrix(matViewHandle, pmatInvView );
  }

void CFULLWINDOW::OnFrameRender( IDirect3DDevice9* pd3dDevice ,D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime )
  {
    HRESULT hr;
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( FULLWINDOW_D3DVERTEX )); 
    hr = pd3dDevice->SetIndices( m_pIB ); 
    UINT iPass, cPasses;
    // ------------------------- ScreenShader ---------------------------
	D3DXHANDLE techHandle = m_pEffect->GetTechniqueByName("Default");
    hr = m_pEffect->SetTechnique(techHandle);
    //hr = m_pEffect->Begin( &uPasses, D3DXFX_DONOTSAVESTATE );
    hr = m_pEffect->Begin( &cPasses, 0 );
    for( iPass = 0; iPass < cPasses; iPass++ )
      {
        hr = m_pEffect->BeginPass( iPass );
        hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP , 0, 0, m_dwNumVertices, 0, 2 );
        hr = m_pEffect->EndPass( );
      }
    hr = m_pEffect->End();
  }

void CFULLWINDOW::OnFrameRender( IDirect3DDevice9* pd3dDevice  )
  {
    HRESULT hr;
    //hr = m_pEffect->SetTexture( "texColorTextureIn" ,pTexture );
    hr = pd3dDevice->SetVertexDeclaration( m_pDecl );
    hr = pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( FULLWINDOW_D3DVERTEX )); 
    hr = pd3dDevice->SetIndices( m_pIB ); 
    UINT iPass, cPasses;
    //
    hr = m_pEffect->Begin( &cPasses, 0 );
    for( iPass = 0; iPass < cPasses; iPass++ )
      {
        hr = m_pEffect->BeginPass( iPass );
        hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP , 0, 0, m_dwNumVertices, 0, 2 );
        hr = m_pEffect->EndPass( );
      }
    hr = m_pEffect->End();
  }

void CFULLWINDOW::OnLostDevice()
  {
    if( m_pEffect )     m_pEffect->OnLostDevice();
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pVB );
  }

void CFULLWINDOW::OnDestroyDevice()
  {
    SAFE_RELEASE( m_pEffect );
    SAFE_RELEASE( m_pDecl );
    //
    SAFE_RELEASE( m_pVB );
    SAFE_RELEASE( m_pIB );
    SAFE_RELEASE( m_pColorTexture );
  }


//----------------------------------------------------------------------------
// ������CreateSquare
// ���ܣ�����ģ��
//----------------------------------------------------------------------------
HRESULT CFULLWINDOW::CreateSquare( LPDIRECT3DDEVICE9 pd3dDevice )
  {
    HRESULT hr      = S_OK;
    m_dwNumIndices  = 4;        // ���������ι�4����
    m_dwNumVertices = 4;                            // 4  
    // ---------------------------- �������������� ---------------------------
      {
        WORD* pIndices;
        if( FAILED( hr = pd3dDevice->CreateIndexBuffer( m_dwNumIndices * sizeof( WORD ),
          0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL )))     return hr;
        if( FAILED( hr = m_pIB->Lock( 0, 0, ( void** )&pIndices, 0 )))     return hr;
        // 
        pIndices[ 0 ] = 0;
        pIndices[ 1 ] = 1;
        pIndices[ 2 ] = 2;
        pIndices[ 3 ] = 3;
        if( FAILED( hr = m_pIB->Unlock( )))  return hr;  // д��϶�Ҫ����
      }
    // ---------------------------- �������㻺���� ---------------------------
      {
        FULLWINDOW_D3DVERTEX *pVertices;
        // 
        if( FAILED( hr = pd3dDevice->CreateVertexBuffer( m_dwNumVertices * sizeof( FULLWINDOW_D3DVERTEX ),
          0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL )))          return hr;
        if( FAILED( hr = m_pVB->Lock( 0, 0, ( void** )&pVertices, 0 )))       return hr;
        // �� X Z ƽ��
        // ��
        pVertices[ 0 ].x = -10;
        pVertices[ 0 ].y =  10;
        pVertices[ 0 ].z =  0;
        pVertices[ 0 ].w =  1;
        pVertices[ 0 ].color =  D3DCOLOR_ARGB( 0x70 ,0xFF ,0 ,0 );  // ͸������
        pVertices[ 0 ].u =  0;
        pVertices[ 0 ].v =  0;
        // ��
        pVertices[ 1 ].x =  10;
        pVertices[ 1 ].y =  10;
        pVertices[ 1 ].z =  0;
        pVertices[ 1 ].w =  1;
        pVertices[ 1 ].color =  D3DCOLOR_ARGB( 0x70 ,0x0 ,0xFF ,0 );  // ͸������
        pVertices[ 1 ].u =  1;
        pVertices[ 1 ].v =  0;
        // ��
        pVertices[ 2 ].x = -10;
        pVertices[ 2 ].y = -10;
        pVertices[ 2 ].z = 0;
        pVertices[ 2 ].w =  1;
        pVertices[ 2 ].color =  D3DCOLOR_ARGB( 0x70 ,0x0 ,0 ,0xFF );
        pVertices[ 2 ].u =  0;
        pVertices[ 2 ].v =  1;
        // ��
        pVertices[ 3 ].x =  10;
        pVertices[ 3 ].y = -10;
        pVertices[ 3 ].z =  0;
        pVertices[ 3 ].w =  1;
        pVertices[ 3 ].color =  D3DCOLOR_ARGB( 0x70 ,0x0FF ,0xFF ,0 );
        pVertices[ 3 ].u =  1;
        pVertices[ 3 ].v =  1;
        //
        if( FAILED( hr = m_pVB->Unlock( )))      return hr;
      }
    return hr;
  }

