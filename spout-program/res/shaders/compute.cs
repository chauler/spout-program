#version 430 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(rgba8, binding = 0) uniform image2D imgOutput;
layout(rgba8, binding = 1) uniform image2D imgInput; //Processed image with sobel edge data vec3(x, y, angle);

shared uint edges[64];
shared float luminance[64];

void main()
{
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec4 inValue = imageLoad(imgInput, texelCoord);
    edges[gl_LocalInvocationIndex] = 0; //Initialize to 0, overwrite
    luminance[gl_LocalInvocationIndex] = inValue.w;
    //Check for threshold
 
    if(inValue.z > 0.9)
    {
        edges[gl_LocalInvocationIndex] = 1;
    } else if(inValue.x > 0.9 && inValue.y > 0.9)
    {
        edges[gl_LocalInvocationIndex] = 2;
    } else if(inValue.x > 0.9 && inValue.y < 0.1)
    {
        edges[gl_LocalInvocationIndex] = 3;
    } else if(inValue.x < 0.1 && inValue.y > 0.9)
    {
        edges[gl_LocalInvocationIndex] = 4;
    }

    if(inValue.w < 0.9)
    {
        edges[gl_LocalInvocationIndex] = 0;
    }

    memoryBarrierShared();
    barrier();
    //First thread of each group tallies up edges and averages the luminance
    if(gl_LocalInvocationID == 0)
    {
        uint edgeTotals[5] = uint[5](0, 0, 0, 0, 0);
        float avgLum = 0.0;
        for (uint i=0; (i<64); (i++))
        {
            edgeTotals[edges[i]] += 1;
            avgLum += luminance[i];
        }
        avgLum = avgLum / 64;
        uint edge = 0;
        if (edgeTotals[1] >= edgeTotals[edge])
        {
            edge = 1;
        }
        if (edgeTotals[2] > edgeTotals[edge])
        {
            edge = 2;
        }
        if (edgeTotals[3] > edgeTotals[edge])
        {
            edge = 3;
        }
        if (edgeTotals[4] > edgeTotals[edge])
        {
            edge = 4;
        }

        if (edgeTotals[edge] < 8)
        {
            edge = 0;
        }
        vec4 color = vec4(0.0);
        if (edge == 1)
        {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
        if (edge == 2)
        {
            color = vec4(0.0, 1.0, 0.0, 1.0);
        }
        if (edge == 3)
        {
            color = vec4(0.0, 0.0, 1.0, 1.0);
        }
        if (edge == 4)
        {
            color = vec4(1.0, 1.0, 0.0, 1.0);
        }
        imageStore(imgOutput, ivec2(int(gl_WorkGroupID.x), int(gl_WorkGroupID.y)), color);
    }
}