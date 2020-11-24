#pragma once
#include <string>
#include <d3dx9.h>

struct FULLWINDOW_D3DVERTEX
{
	//λ��
	float x, y, z, w;
	//�������ɫֵ������Ͷ��������õ�D3DDECLTYPE_D3DCOLOR��Ӧ
	DWORD color;
	//��������
	float u, v;
};
class CFullWindow
{
private:
	//��ɫ����
	LPDIRECT3DTEXTURE9				m_pColorTexture;
	//���㻺����
	LPDIRECT3DVERTEXBUFFER9			m_pVB;
	//����������
	LPDIRECT3DINDEXBUFFER9			m_pIB;
	//������������
	DWORD							m_dwNumIndices;
	//�������
	DWORD							m_dwNumVertices;
	//���������ӿ�
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;
	//Ч���ӿ�
	LPD3DXEFFECT					m_pEffect;
	HRESULT CreateSquare(LPDIRECT3DDEVICE9 pd3dDevice);
public:
	CFullWindow();
	~CFullWindow();
	void init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice, D3DXVECTOR4 );
	void OnFrameMove(D3DXMATRIXA16 * pmat, float fTime );
	void OnFrameRender(IDirect3DDevice9* pd3dDevice, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice);
	void OnLostDevice();
	void OnDestroyDevice();
};

