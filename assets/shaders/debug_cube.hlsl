cbuffer CameraCB : register(b0)
{
    float4x4 viewProj;
};

struct VSInput
{
    float3 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_Position;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = mul(viewProj, float4(input.position, 1.0));
    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    return float4(1.0, 0.0, 0.0, 1.0); // red lines
}
