#pragma once
struct BOARD_D3DVERTEX
{
	//位置
	float x, y, z, w;
	//下面的颜色值，必须和顶点流设置的D3DDECLTYPE_D3DCOLOR对应
	DWORD color;
	//纹理坐标
	float u, v;
};
class CBoard
{
private:
	//颜色纹理
	LPDIRECT3DTEXTURE9				m_pColorTexture;
	//顶点缓冲区
	LPDIRECT3DVERTEXBUFFER9			m_pVB;
	//索引缓冲区
	LPDIRECT3DINDEXBUFFER9			m_pIB;
	//顶点索引个数
	DWORD							m_dwNumIndices;
	//顶点个数
	DWORD							m_dwNumVertices;
	//顶点声明接口
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;
	//效果接口
	LPD3DXEFFECT					m_pEffect;
public:
	CBoard();
	~CBoard();
	HRESULT CreateSquare(LPDIRECT3DDEVICE9 pd3dDevice);
	void init();
	HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice);
	HRESULT OnResetDevice(IDirect3DDevice9* pd3dDevice);
	void OnFrameMove();
	void OnFrameRender(IDirect3DDevice9* pd3dDevice, float fProgress);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice, LPDIRECT3DTEXTURE9 pTexture, int i);
	void OnFrameRender(IDirect3DDevice9* pd3dDevice, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime);
	void OnLostDevice();
	void OnDestroyDevice();
};

