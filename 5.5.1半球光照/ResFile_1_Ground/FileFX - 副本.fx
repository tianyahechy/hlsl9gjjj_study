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
float4   g_vLightDirect;            // �ƹ����䷽��

float3 g_VectorUp = float3(0.0,1.0,0.0);

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
    float4 Position   : POSITION;  	//����λ��
    float3 Normal     : NORMAL; 	//�����ϵķ�����
    float2 TextureUV  : TEXCOORD0;  //��������
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   		//����λ��
    float3 Normal     : TEXCOORD0; 			//�����ϵķ�����
    float2 TextureUV  : TEXCOORD1;  		//ģ���ϵ���������
    float3 vLookAt    : TEXCOORD2;        	//ģ����ÿ��������ָ��۲��ߵķ���
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
    // ÿ�������ϵ�ָ��۲��ߵķ���
    Out.vLookAt = normalize( Position - g_vEyePosition );
   
    return Out;
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
  
    // ������
    float fAmbient = 0.1f;
    // �߹�����
    float4   fSpecularColor  = float4( 1.0 ,  1.0 ,1.0 , 1.0 );
    // �������
    float     fDiffuse  =  0;
	fDiffuse = 0.4 * dot( In.Normal, g_VectorUp);
    // �߹�,����������۲�����һ��ʱ������߹�����
    float    fSpecular =  0.3 * pow( saturate( dot( -In.vLookAt, In.Normal )), 20 ) ;
    
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
