#pragma once
#include <d3dx9.h>
#include <string>
class myMesh
{
public:
	myMesh();
	~myMesh();
	//�����Ƽ���ģ��
	HRESULT loadMeshFromModelName(LPDIRECT3DDEVICE9 g_pd3dDevice, std::string wstrModelName);
	//Ϊ���ͼ���ƣ�����Ҫ������ʣ�
	void RenderForDepth(LPDIRECT3DDEVICE9 g_pd3dDevice);
	//�������壨��Ҫ������ʣ�
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
	bool				g_bGenerateShadowMap;	//��ģ���Ƿ�Ҫ������Ӱ
	std::string			g_strModelName;
};

