//受光物体
texture g_ColorTexture;
float g_fTime;
float4x4 g_matWorld;
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matWorldNormalInverseTranspose;
//观察者位置
float4 g_vEyePosition;
//灯光位置
float4 g_vLightPosition;

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
    float2 TextureUV : TEXCOORD1;
};

VS_OUTPUT RenderSceneVS(VS_INPUT In)
{
    //计算顶点的位置
    VS_OUTPUT Out = (VS_OUTPUT) 0;
    float4x4 matWorldView = mul(g_matWorld, g_matView);
    float4x4 matWorldViewProject = mul(matWorldView, g_matProject);
    Out.Position = mul(In.Position, matWorldViewProject);
    Out.TextureUV = In.TextureUV;
    return Out;
}

float4 RenderScenePS(VS_OUTPUT In ) : COLOR0
{
    return tex2D(MeshTextureSampler, In.TextureUV);
}

technique DefaultTech
{
    pass p0
    {
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcColor;
		DestBlend = InvSrcAlpha;
		AlphaTestEnable = true;
        FillMode = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS();
        PixelShader = compile ps_2_0 RenderScenePS();
    }
}





