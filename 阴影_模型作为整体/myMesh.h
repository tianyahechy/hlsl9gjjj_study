#pragma once
#include <d3dx9.h>
#include <string>
class myMesh
{
public:
	myMesh();
	~myMesh();
	//从名称加载模型
	HRESULT loadMeshFromModelName(LPDIRECT3DDEVICE9 g_pd3dDevice, std::string wstrModelName);
	//为深度图绘制（不需要纹理材质）
	void RenderForDepth(LPDIRECT3DDEVICE9 g_pd3dDevice);
	//绘制物体（需要纹理材质）
	void RenderToScene(LPDIRECT3DDEVICE9 g_pd3dDevice, LPD3DXEFFECT effect);
	void setMatWorld(D3DXMATRIXA16 matWorld);
	D3DXMATRIXA16 getMatWorld();
	void setGenerateShadowMap(bool generateShadowMap);
	bool getBGenerateShadowMap();
	void setModelName(std::string strModelName);
	std::string getModelName();
private:
	LPD3DXMESH          g_pMesh; // Our mesh object in sysmem
	D3DMATERIAL9*       g_pMeshMaterials; // Materials for our mesh
	LPDIRECT3DTEXTURE9* g_pMeshTextures; // Textures for our mesh
	DWORD               g_dwNumMaterials;   // Number of mesh materials

	D3DXMATRIXA16		g_matWorld;
	bool				g_bGenerateShadowMap;	//该模型是否要产生阴影
	std::string			g_strModelName;
};

