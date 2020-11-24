//灯光物体
texture g_ColorTexture;
float g_fTime;
float4x4 g_matWorld;
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matWorldNormalInverseTranspose;
//观察者位置
float4 g_vEyePosition;

float4 	 g_vLightLookAt;			//灯光观察方向
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
    float2 TextureUV : TEXCOORD0;
};

VS_OUTPUT RenderSceneVS(VS_INPUT In)
{
    //计算顶点的位置
    VS_OUTPUT Out = (VS_OUTPUT) 0;

	//根据以上两个全局向量和向上向量来计算此矩阵，首先计算观察方向向量
	//再计算向右向量，再反过来计算向上向量，这样就构成此矩阵
	float3 v3Look = normalize(g_vLightPosition.xyz - g_vLightLookAt.xyz);
	float3 v3Right = normalize(cross(float3(0.0f,1.0f,0.0f), v3Look));
	float3 v3Up = cross(v3Look,v3Right);
	//通过几个向量建立矩阵
	float4x4 matLookAt;
	matLookAt[0] = float4(v3Right.xyz, 0.0 );
	matLookAt[1] = float4(v3Up.xyz, 0.0);
	matLookAt[2] = float4(v3Look.xyz, 0.0 );
	matLookAt[3] = float4(g_vLightPosition.xyz, 1.0);	
	//此矩阵构造完毕后就可以在世界矩阵前按此矩阵变换此物体
	float4x4 matLocal = mul(matLookAt, g_matWorld);
    float4x4 matWorldView = mul(g_matWorld, g_matView);
    float4x4 matWorldViewProject = mul(matWorldView, g_matProject);
    Out.Position = mul(In.Position, matWorldViewProject);
    Out.TextureUV = In.TextureUV;

    return Out;
}

float4 RenderScenePS(VS_OUTPUT In ) : COLOR0
{
    return tex2D(MeshTextureSampler, In.TextureUV) ;
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





