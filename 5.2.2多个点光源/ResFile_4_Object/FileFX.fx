//--------------------------------------------------------------------------------------
// File: 受光物体.fx
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matWorldNormalInverseTranspose; 
float4   g_vEyePosition;            // 观察者位置
float4   g_vLightPosition_1 ;         // 灯光1位置
float4   g_vLightPosition_2;          // 灯光2位置
// 
float4   g_vLightColor_1 = float4( 1.0 , 0.0 , 0.0 ,1.0 );
float4   g_vLightColor_2 = float4( 0.0 , 1.0 , 0.0 ,1.0 );

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;  
    float3 Normal     : NORMAL; 
    float2 TextureUV  : TEXCOORD0;  
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float3 Normal     : TEXCOORD0; 
    float2 TextureUV  : TEXCOORD1;  
    float3 vLight_1   : TEXCOORD2;       // 模型上每个顶点上指向光源的方向    
    float3 vLookAt_1  : TEXCOORD3;       // 模型上每个顶点上指向观察者的方向
    float3 vLight_2   : TEXCOORD4;
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Position = mul( In.Position , matWorldViewProject );
    Out.TextureUV = In.TextureUV;
    // 法向量
    Out.Normal = normalize( mul( In.Normal , ( float3x3 )g_matWorldNormalInverseTranspose ));
    // 顶点位置
    float4 Position = mul( In.Position ,g_matWorld );
    // 灯光1
    Out.vLight_1 = g_vLightPosition_1 - Position ;
    Out.vLookAt_1 =  normalize( Position - g_vEyePosition );
    // 灯光2
    Out.vLight_2 = g_vLightPosition_2 - Position ;
    return Out;
  }

float4 RenderScenePS(VS_OUTPUT In ) : COLOR0
{
   // 环境光
    float fAmbient = 0.1f;
    // 高光因子
    //float4   fSpecularColor  = float4( 1.0 ,  1.0 ,1.0 , 1.0 );
    //灯光1
    float4 v4Diffuse_1 =  0.4* dot(In.Normal, normalize(In.vLight_1)) * g_vLightColor_1;
    float4 v4fSpecular_1 = pow(saturate(dot(reflect(normalize(In.vLight_1),In.Normal),In.vLookAt_1)), 4) * g_vLightColor_1;
    //灯光2
    float4 v4Diffuse_2 =  0.4* dot(In.Normal, normalize(In.vLight_2)) * g_vLightColor_2;
    float4 v4fSpecular_2 = pow(saturate(dot(reflect(normalize(In.vLight_2),In.Normal),In.vLookAt_1)), 4) * g_vLightColor_2;
    return fAmbient * tex2D(MeshTextureSampler, In.TextureUV) + v4Diffuse_1 + v4Diffuse_2 + v4fSpecular_1 + v4fSpecular_2;
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





