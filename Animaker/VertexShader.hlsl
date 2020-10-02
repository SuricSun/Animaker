//

cbuffer ConstantBuffer : register(b0)
{
    float4x4 world;      // world matrix for object
    //float4x4 view;        // view matrix
    //float4x4 projection;  // projection matrix
};

struct VS_INPUT
{
    float4 pos   : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;  // interpolated vertex position (system value)
    float4 color    : COLOR;       // interpolated diffuse color
};

PS_INPUT main(VS_INPUT inVert)
{
    PS_INPUT outVert;

    float4x4 projection = { float4(1 / 1.7777777778,0,0,0),float4(0,1,0,0),float4(0,0,1,0),float4(0,0,0,1) };

    inVert.pos = mul(inVert.pos, world);
    //inVert.pos = mul(inVert.pos, view);
    inVert.pos = mul(inVert.pos, projection);
   // inVert.color = float4(world._11, world._11, world._11, world._11);

    outVert.pos = inVert.pos;
    outVert.color = inVert.color;

    return outVert;
}