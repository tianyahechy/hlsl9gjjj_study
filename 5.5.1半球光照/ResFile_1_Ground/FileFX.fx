//--------------------------------------------------------------------------------------
// File: 受光物体.fx
// 增加光线衰减的功能
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              

float    g_fTime;                   
float4x4 g_matWorld;                  
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matWorldNormalInverseTranspose; 
float4   g_vEyePosition;            // 观察者位置
float4   g_vLightDirection ;         // 灯光位置

float3   g_VectorUp = float3( 0.0 , 1.0 , 0.0 );

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
    float3 vLookAt    : TEXCOORD3;       // 模型上每个顶点上指向观察者的方向
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
    float4 Position = mul( In.Position ,g_matWorld );
    // 每个顶点上的指向观察者的方向
    Out.vLookAt = normalize( Position - g_vEyePosition );
    //
    return Out;
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    // 光线距离衰减因子
    //float fLength = length( In.vLight ) * 0.1 ;
    // 环境光
    float fAmbient = 0.2f;
    // 高光因子
    float4   fSpecularColor  = float4( 1.0 , 1.0 ,1.0 , 1.0 );
    // 漫反射光
    float fDiffuse  = 0.4 * dot( In.Normal, g_VectorUp ) ;
    // 高光，反射向量与观察向量一致时，镜面高光最强
    float fSpecular =  0.20 * pow( saturate( dot( -In.vLookAt , In.Normal )) , 120 );
    return tex2D( MeshTextureSampler ,In.TextureUV ) * ( fAmbient + fDiffuse + fSpecular )  ;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
