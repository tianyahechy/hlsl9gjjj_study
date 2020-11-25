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
     float3 Normal : NORMAL;
     float2 TextureUV : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float3 Normal : TEXCOORD0;
    float2 TextureUV : TEXCOORD1;
    //模型上每个顶点上指向光源的方向
    float3 vLight : TEXCOORD2;
    //模型上每个顶点上指向观察者的方向
    float3 vLookAt : TEXCOORD3;
};

VS_OUTPUT RenderSceneVS(VS_INPUT In)
{
    //计算顶点的位置
    VS_OUTPUT Out = (VS_OUTPUT) 0;
    float4x4 matWorldView = mul(g_matWorld, g_matView);
    float4x4 matWorldViewProject = mul(matWorldView, g_matProject);
    Out.Position = mul(In.Position, matWorldViewProject);
    Out.TextureUV = In.TextureUV;

    //法向量
    Out.Normal = normalize(mul(In.Normal, (float3x3) g_matWorldNormalInverseTranspose));
    float4 Position = mul(In.Position, g_matWorld);
    Out.vLight = g_vLightPosition - Position;
    //每个顶点上的指向观察者的方向
    Out.vLookAt = normalize(Position - g_vEyePosition);
    return Out;
}

float4 RenderScenePS(VS_OUTPUT In ) : COLOR0
{
	//漫反射光,使用原来计算漫反射光强的值作为纹理坐标的索引来查找颜色值
	float fDiffuse = (dot(In.Normal, normalize(In.vLight)) + 1.0 ) / 2.0;
	return tex1D(MeshTextureSampler, fDiffuse );
	
}

technique DefaultTech
{
    pass p0
    {
         FillMode = Solid;
         VertexShader = compile vs_2_0 RenderSceneVS();
         PixelShader = compile ps_2_0 RenderScenePS();
    }
}





