//--------------------------------------------------------------------------------------
// File: �ܹ�����.fx
// ���ӹ���˥���Ĺ���
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matWorldNormalInverseTranspose; 
float4   g_vEyePosition;            // �۲���λ��
float4   g_vLightPosition ;         // �ƹ�λ��
float4   g_vLightDirect;            // �ƹ����䷽��

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
    float3 vLight     : TEXCOORD2;        // ģ����ÿ��������ָ���Դ�ķ���    
    float3 vLookAt    : TEXCOORD3;        // ģ����ÿ��������ָ��۲��ߵķ���
    float3 pos        : TEXCOORD4;        // ��Ļ��ÿ�����ص�Ŀռ�λ��
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Position = mul( In.Position , matWorldViewProject );
    Out.TextureUV = In.TextureUV;
    // ������
    Out.Normal = normalize( mul( In.Normal , ( float3x3 )g_matWorldNormalInverseTranspose ));
    float4 Position = mul( In.Position ,g_matWorld );
    Out.vLight = g_vLightPosition - Position ;
    // ÿ�������ϵ�ָ��۲��ߵķ���
    Out.vLookAt = normalize( Position - g_vEyePosition );
    //
    Out.pos = Position;
    return Out;
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
    //
    float  MIN = 0.85f;
    float  MAX = 0.9f;
    // ��׶��С 
    //float fCone = 0.9f;
    // ���߾���˥������
    float fLength = length( In.vLight ) * 0.10;
    // ������
    float fAmbient = 0.1f;
    // �߹�����
    float4   fSpecularColor  = float4( 1.0 ,  1.0 ,1.0 , 1.0 );
    // ����ƹ⵽���������
    float3 fDirectInVertex = normalize( In.pos - g_vLightPosition.xyz );
    // ����нǣ�    
    float fDiffuse = 0;
    float fSpecular = 0;
    float fTensity = 0;
	fTensity = dot(fDirectInVertex, normalize(g_vLightDirect));
    if( fTensity > MIN )
      {
        // ����ǿ��
        fTensity = clamp( fTensity ,MIN ,MAX ) - MIN ;
        fTensity = 30 * fTensity ;
        // �������
        fDiffuse  = fTensity * 0.4 * dot( In.Normal, normalize( In.vLight )) / fLength;
        // �߹�
        fSpecular = fTensity * pow( saturate( dot( reflect( normalize( In.vLight ) , In.Normal ), In.vLookAt )), 10 ) / fLength;
      }
    else 
      {
        fDiffuse = 0.0f;
        fSpecular = 0.0f;
      }
    return tex2D( MeshTextureSampler ,In.TextureUV ) * ( fAmbient + fDiffuse + fSpecular * fSpecularColor );
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
