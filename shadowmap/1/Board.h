#pragma once
#include <string>

struct BOARD_D3DVERTEX
{
	//λ��
	float x, y, z, w;
	//�������ɫֵ������Ͷ��������õ�D3DDECLTYPE_D3DCOLOR��Ӧ
	DWORD color;
	//��������
	float u, v;
};
class CBoard
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
	CBoard();
	~CBoard();
	void init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice);
	void OnFrameMove();
	void OnFrameRender(IDirect3DDevice9* pd3dDevice, LPDIRECT3DTEXTURE9 pTexture, int i);
	void OnLostDevice();
	void OnDestroyDevice();

public:
	void setFile(std::wstring wstrImgName, std::wstring wstrFXName);
private:
	//ͼƬ�ļ�
	std::wstring _wstrImgName;
	//Ч���ļ�
	std::wstring _wstrFXName;
};

