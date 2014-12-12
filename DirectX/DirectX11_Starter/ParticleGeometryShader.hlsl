cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 camPos;
};

struct VertexOutput
{
	float4 initialPos	: SV_POSITION;
	//float3 initialVel	: VELOCITY;
	float2 size			: SIZE;	// Storing age in y
	//float age : AGE;
	//unsigned int type : TYPE;
};

struct GSOutput
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
};

[maxvertexcount(4)]
void main(point VertexOutput input[1] /*: SV_POSITION*/, inout TriangleStream<GSOutput> output)
{
	// Compute world-space directions to camera
	// - Assumes you have the camera’s world position in “camPos”
	// - Assumes your VS output struct has vertex World Position
	input[0].initialPos.w = 1.0f;
	float3 pos = (float3)input[0].initialPos;
	float3 look = normalize(camPos - pos);
	float3 right = normalize(cross(float3(0, 1, 0), look));
	float3 up = cross(look, right);
	// Calculate half the width/height of the resulting quad
	// Size is just hard-coded here for simplicity
	float2 size = float2(input[0].size.x, input[0].size.y); // Could be stored in vertex
	float halfW = 0.5f * size.x;
	float halfH = 0.5f * size.y;

	// Create an array to hold the 4 new vertex positions
	// (The corners of the quad)
	float4 v[4];
	// Starting with the input world position, move each
	// position either up or down, and either left or right
	v[0] = float4(input[0].initialPos + halfW*right - halfH*up, 1);
	v[1] = float4(input[0].initialPos + halfW*right + halfH*up, 1);
	v[2] = float4(input[0].initialPos - halfW*right - halfH*up, 1);
	v[3] = float4(input[0].initialPos - halfW*right + halfH*up, 1);
	/*v[3] = float4(input[0].initialPos + halfW*right - halfH*up, 1);
	v[2] = float4(input[0].initialPos + halfW*right + halfH*up, 1);
	v[1] = float4(input[0].initialPos - halfW*right - halfH*up, 1);
	v[0] = float4(input[0].initialPos - halfW*right + halfH*up, 1);*/

	// Define an array of texture coordinates to match
	// the four corners of the quad, allowing us to loop
	// over the four uv coords easily.
	// (Could be defined outside the function)
	float2 quadUVs[4] =
	{
		float2(1, 1),
		float2(1, 0),
		float2(0, 1),
		float2(0, 0)
	};

	// Finalize the GS output by appending 4 verts worth of data
	GSOutput vert; // Holds a single vertex (just Position and UV)
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		// Already have the world position, need to multiple
		// by the view and projection matrices
		vert.position = mul(v[i], mul(view, projection));
		vert.uv = quadUVs[i]; // Copy uv from array

		output.Append(vert); // Append this vertex!
	}
}