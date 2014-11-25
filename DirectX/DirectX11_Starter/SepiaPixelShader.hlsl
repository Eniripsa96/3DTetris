
Texture2D myTexture : register(t0);
SamplerState mySampler: register(s0);

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD0;
	float4 lightDir     : LIGHT;
	float4 color        : COLOR;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Return color sampled from texture
	float nDotL = dot(normalize(input.normal), -normalize((float3)input.lightDir));
	float3 result = myTexture.Sample(mySampler, input.uv).rgb * (1 - input.lightDir.w + input.lightDir.w * nDotL) * input.color.rgb;
	return float4(
		result.r * 0.393 + result.g * 0.769 + result.b * 0.189, 
		result.r * 0.349 + result.g * 0.686 + result.b * 0.168, 
		result.r * 0.272 + result.g * 0.534 + result.b * 0.131, 
		input.color.a
	);
}