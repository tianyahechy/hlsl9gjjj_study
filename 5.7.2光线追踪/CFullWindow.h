//-----------------------------------------------------------------------------
// �ļ���CFULLWINDOW.h
// ���ܣ���Ⱦ����ͷ�ļ�
//-----------------------------------------------------------------------------
//#include "common.h"
//#include "dxstdafx.h"

#ifndef FULLWINDOW_H
#define FULLWINDOW_H

struct FULLWINDOW_D3DVERTEX
  {
    float x ,  y  , z  ,w;      // λ��
    // �������ɫֵ������Ͷ��������õ� D3DDECLTYPE_D3DCOLOR ��Ӧ
    DWORD color;                // �ں�����ʼ��ʱ���� RGBA ��ʼ���������� D3D����������ʽΪ D3DDECLTYPE_D3DCOLOR
    float u ,  v;               // ��������
  };

class CFULLWINDOW
  {
    private:
      LPDIRECT3DTEXTURE9            m_pColorTexture;      // ��ɫ����
      LPDIRECT3DVERTEXBUFFER9       m_pVB;                // ���㻺����
      LPDIRECT3DINDEXBUFFER9        m_pIB;                // ���㻺����ָ��
      DWORD                         m_dwNumIndices;       // �������������
      DWORD                         m_dwNumVertices;      // �������
      // 
      LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // ���������ӿ�
      LPD3DXEFFECT                  m_pEffect;            // Ч���ӿ�
      //
      HRESULT CreateSquare( IDirect3DDevice9* pd3dDevice );
    public:
      CFULLWINDOW( );
      ~CFULLWINDOW( );
      //
      void    Init( );
      HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice );
      HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice , D3DXVECTOR4 );
      void    OnFrameMove( D3DXMATRIXA16* pmat , float fTime);
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice ,D3DXMATRIXA16* pWorld ,D3DXMATRIXA16* pView ,D3DXMATRIXA16* pProject ,double fTime );
      void    OnFrameRender( IDirect3DDevice9* pd3dDevice );
      void    OnLostDevice();
      void    OnDestroyDevice();
  };

#endif
