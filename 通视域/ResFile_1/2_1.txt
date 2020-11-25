texture g_ColorTexture;
float g_fTime;
float4x4 g_matWorld;
float4x4 g_matView;
float4x4 g_matProject;

sampler MeshTextureSampler = sampler_state
{
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

struct VS_INPUT
{
    float4 Position : POSITION;
    float2 TextureUV : TEXCOORD0;
};
struct VS_OUTPUT
{
    float4 Position : POSITION;
    float2 TextureUV: TEXCOORD0;
};
VS_OUTPUT RenderSceneVS(VS_INPUT IN)
{
    VS_OUTPUT Output;
    float4x4 matWorldView = mul(g_matWorld, g_matView);
    float4x4 matWorldViewProject = mul(matWorldView, g_matProject);
    Output.Position = mul(IN.Position, matWorldViewProject);
    Output.TextureUV = IN.TextureUV;
    return Output;
}

float4 RenderScenePS(VS_OUTPUT In) : COLOR0
{
    float4 vColor = tex2D(MeshTextureSampler, In.TextureUV);
    return vColor;
}

technique DefaultTech
{
    pass P0
     {
	FillMode = Solid;
	VertexShader = compile vs_2_0 RenderSceneVS();
	PixelShader = compile ps_2_0 RenderScenePS();
    }
}
