
// The constant buffer that holds our "per model" data
// - Each object you draw with this shader will probably have
//   slightly different data (at least for the world matrix)
cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 lightDirection;
	float4 color;
};

// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD0;
};

// Defines the output data of our vertex shader
// - At a minimum, you'll need an SV_POSITION
// - Should match the pixel shader's input
struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD0;
	float4 lightDir     : LIGHT;
	float4 color        : COLOR;
};

// The entry point for our vertex shader
VertexToPixel main( VertexShaderInput input )
{
	// Set up output
	VertexToPixel output;

	// Calculate output position
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	// Pass the normal through - will be interpolated per-pixel by the rasterizer
	output.normal = normalize(input.normal);
	//output.normal = normalize(mul((input.normal), (float3x3)worldjViewProj));

	// Constant data
	output.lightDir = lightDirection;
	output.color = color;

	// Pass the UV coordinates
	output.uv = input.uv;
	
	return output;
}