struct GSOutput
{
	float4 pos	: SV_POSITION;
	float3 norm : NORMAL;
	float2 uv	: TEXCOORD0;
};

[maxvertexcount(3)]
void main(triangle GSOutput input[3] /*: SV_POSITION*/, inout TriangleStream<GSOutput> output)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = input[i].pos;
		element.norm = input[i].norm;
		element.uv = input[i].uv;
		output.Append(element);
	}
}