
#include "TriggerData.h"
#include "hookd3d.h"
#include <iostream>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

void DrawLines(vec2* point0)
{
    //if (point0.Length < 2)
    //    return;

    //for (int x = 0; x < point0.Length - 1; x++)
    //    DrawLine(point0[x], point0[x + 1], );
}


vec3 transform = { 0,0,0 };
vec3 right = { 0,0,0 };
vec3 up = { 0,0,0 };
viewMatrix view_matrix;
mat4 V_ViewMatrix;
mat4 M_WorldTransform;

bool debugme = true;
bool flipflag = true;
bool swapyz = true;
bool interpretCamDataAsTransformed = true;

bool diffupdir = true;
bool testTransform = true;

bool killme = true;
float g_hfov = 2.3212879051524583373085087220899;



float g_fAspect = g_screenHeight / g_screenWidth; // it looks like the perfect val is like 0.53, instead of 0.5625. no idea why.
bool world_to_screen6(PlayerData* playerdata, vec3 worldPos, vec2* out, ImU32& outcolor);
float g_nearHeight = 1;
float g_vertScale = 3.1415926535897932384626433832795; // it looks like the perfect val is actually like 3.3 ? nfi where that number comes from
float dot(const vec3& a, const vec3& b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

vec3 cross(const vec3& v1, const vec3& v2)
{
    vec3 crossproduct;
    crossproduct.x = v1.y * v2.z - v1.z * v2.y;
    crossproduct.y = v1.x * v2.z - v1.z * v2.x;
    crossproduct.z = v1.x * v2.y - v1.y * v2.x;
    return crossproduct;
}

vec3 normalise(const vec3& v1)
{
    float sqr = v1.x * v1.x + v1.y * v1.y + v1.z * v1.z;
    float normaliser = (1.0f / std::sqrt(sqr));
    vec3 normalisedVec = v1;
    normalisedVec = normalisedVec * normaliser;
    return normalisedVec;
}



// Pre-multiply a vector by a matrix on the left.
vec4 operator*(const mat4& m, const vec4& v)
{
    return vec4(
        m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3],
        m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3],
        m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3],
        m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3]
    );
}

// Matrix multiplication
mat4 operator*(const mat4& m2, const mat4& m1)
{
    // I swapped the parameters to get row-major 
    vec4 X = m1 * m2[0];
    vec4 Y = m1 * m2[1];
    vec4 Z = m1 * m2[2];
    vec4 W = m1 * m2[3];

    return mat4(X, Y, Z, W);
}

// Pre-multiply a vector by a matrix on the right.
vec4 operator*(const vec4& v, const mat4& m)
{
    return vec4(
        v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2] + v[3] * m[0][3],
        v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2] + v[3] * m[1][3],
        v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2] + v[3] * m[2][3],
        v[0] * m[3][0] + v[1] * m[3][1] + v[2] * m[3][2] + v[3] * m[3][3]
    );
}







int testthingy = 0;

struct RECT3D
{
    vec3 p1, p2, p3, p4;
};

struct box
{
    vec3 pos;
    vec3 size;

    RECT3D sq1() // top of the cube
    {
        RECT3D out;
        float sharedAxis = (pos.z + size.z);
        out.p1 = vec3(pos.x + size.x, pos.y + size.y, sharedAxis);
        out.p2 = vec3(pos.x - size.x, pos.y + size.y, sharedAxis);
        out.p3 = vec3(pos.x + size.x, pos.y - size.y, sharedAxis);
        out.p4 = vec3(pos.x - size.x, pos.y - size.y, sharedAxis);
        return out;
    }

    RECT3D sq2() // bottom of the cube
    {
        RECT3D out;
        float sharedAxis = (pos.z - size.z);
        out.p1 = vec3(pos.x + size.x, pos.y + size.y, sharedAxis);
        out.p2 = vec3(pos.x - size.x, pos.y + size.y, sharedAxis);
        out.p3 = vec3(pos.x + size.x, pos.y - size.y, sharedAxis);
        out.p4 = vec3(pos.x - size.x, pos.y - size.y, sharedAxis);
        return out;
    }

    RECT3D sq3() // pos x of cube
    {
        RECT3D out;
        float sharedAxis = (pos.x + size.x);
        out.p1 = vec3(sharedAxis, pos.y + size.y, pos.z + size.z);
        out.p2 = vec3(sharedAxis, pos.y + size.y, pos.z - size.z);
        out.p3 = vec3(sharedAxis, pos.y - size.y, pos.z + size.z);
        out.p4 = vec3(sharedAxis, pos.y - size.y, pos.z - size.z);
        return out;
    }

    RECT3D sq4() // neg x of cube
    {
        RECT3D out;
        float sharedAxis = (pos.x - size.x);
        out.p1 = vec3(sharedAxis, pos.y + size.y, pos.z + size.z);
        out.p2 = vec3(sharedAxis, pos.y + size.y, pos.z - size.z);
        out.p3 = vec3(sharedAxis, pos.y - size.y, pos.z + size.z);
        out.p4 = vec3(sharedAxis, pos.y - size.y, pos.z - size.z);
        return out;
    }

    RECT3D sq5() // pos y of cube
    {
        RECT3D out;
        float sharedAxis = (pos.y + size.y);
        out.p1 = vec3(pos.x + size.x, sharedAxis, pos.z + size.z);
        out.p2 = vec3(pos.x + size.x, sharedAxis, pos.z - size.z);
        out.p3 = vec3(pos.x - size.x, sharedAxis, pos.z + size.z);
        out.p4 = vec3(pos.x - size.x, sharedAxis, pos.z - size.z);
        return out;
    }

    RECT3D sq6() // neg y of cube
    {
        RECT3D out;
        float sharedAxis = (pos.y - size.y);
        out.p1 = vec3(pos.x + size.x, sharedAxis, pos.z + size.z);
        out.p2 = vec3(pos.x + size.x, sharedAxis, pos.z - size.z);
        out.p3 = vec3(pos.x - size.x, sharedAxis, pos.z + size.z);
        out.p4 = vec3(pos.x - size.x, sharedAxis, pos.z - size.z);
        return out;
    }
};


bool firstDrawSquare = true;

void DrawSquare(ImDrawList* draw_list, RECT3D sq)
{
    ImU32 color = 0;
    vec2 ssp1 = { 0, 0 };
    vec2 ssp2 = { 0, 0 };
    vec2 ssp3 = { 0, 0 };
    vec2 ssp4 = { 0, 0 };
    if (world_to_screen6(g_PlayerData, sq.p1, &ssp1, color) &&
        world_to_screen6(g_PlayerData, sq.p2, &ssp2, color) &&
        world_to_screen6(g_PlayerData, sq.p3, &ssp3, color) &&
        world_to_screen6(g_PlayerData, sq.p4, &ssp4, color))
    {
    }
    else
    {
        // don't draw if a point was outside bounds
        return;
    }


    if (firstDrawSquare)
    {
        firstDrawSquare = false;

        std::cout << "sq.p1.x: " << sq.p1.x << ", sq.p1.y: " << sq.p1.y << ", sq.p1.z: " << sq.p1.z << std::endl;
        std::cout << "sq.p2.x: " << sq.p2.x << ", sq.p2.y: " << sq.p2.y << ", sq.p2.z: " << sq.p2.z << std::endl;
        std::cout << "sq.p3.x: " << sq.p3.x << ", sq.p3.y: " << sq.p3.y << ", sq.p3.z: " << sq.p3.z << std::endl;
        std::cout << "sq.p4.x: " << sq.p4.x << ", sq.p4.y: " << sq.p4.y << ", sq.p4.z: " << sq.p4.z << std::endl;


        std::cout << "ssp1.x: " << ssp1.x << ", ssp1.y: " << ssp1.y << std::endl;
        std::cout << "ssp2.x: " << ssp2.x << ", ssp2.y: " << ssp2.y << std::endl;
        std::cout << "ssp3.x: " << ssp3.x << ", ssp3.y: " << ssp3.y << std::endl;
        std::cout << "ssp4.x: " << ssp4.x << ", ssp4.y: " << ssp4.y << std::endl;

    }
    color = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0, 0.3)); // trans red

    //draw_list->AddQuadFilled(ImVec2(ssp1.x, ssp1.y), ImVec2(ssp2.x, ssp2.y), ImVec2(ssp3.x, ssp3.y), ImVec2(ssp4.x, ssp4.y), color);
    // order of the points matters here so we don't connect them diagonally
    draw_list->AddQuadFilled(ImVec2(ssp1.x, ssp1.y), ImVec2(ssp3.x, ssp3.y), ImVec2(ssp4.x, ssp4.y), ImVec2(ssp2.x, ssp2.y), color);
}

void TestRenderBoxWithImgui()
{
    if (g_PlayerData->CanReadCameraData())
    {
        if (debugme)
        {
            debugme = false;
            std::cout << "OKAY rendering our box" << std::endl;
        }
        ImDrawList* draw_list = ImGui::GetOverlayDrawList();
        ImU32 color = 0;

        XMMATRIX viewMatrix;

        //vec2 screen3 = { 0,0 };

        //world_to_screen6(g_PlayerData, vec3(20, -95, -73), &screen3, color);

        box ourBox;
        ourBox.pos = vec3(20, -95, -73);
        ourBox.size = vec3(0.3, 0.3, 0.3);

        DrawSquare(draw_list, ourBox.sq1());
        DrawSquare(draw_list, ourBox.sq2());
        DrawSquare(draw_list, ourBox.sq3());
        DrawSquare(draw_list, ourBox.sq4());
        DrawSquare(draw_list, ourBox.sq5());
        DrawSquare(draw_list, ourBox.sq6());

        vec2 screen3 = { 0,0 };
        world_to_screen6(g_PlayerData, vec3(28, -95, -73), &screen3, color);

        //draw_list->AddCircleFilled(ImVec2(screen3.x, screen3.y), 20, color);
        //draw_list->AddRectFilled()

        if (GetKeyState('U') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
        {
            g_vertScale += 0.001;
        }

        if (GetKeyState('J') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
        {
            g_vertScale -= 0.001;
        }

        if (GetKeyState('M') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
        {
            std::cout << "g_vertScale: " << g_vertScale << std::endl;
        }



        if (GetKeyState('Y') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
        {
            g_fAspect += 0.01;
        }

        if (GetKeyState('H') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
        {
            g_fAspect -= 0.01;
        }

        if (GetKeyState('N') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
        {
            std::cout << "g_fAspect: " << g_fAspect << std::endl;
        }


    }
}



//if (GetKeyState('A') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
//{
//    flipflag = !flipflag;
//}

bool iwannadie = true;

int countdeath = 0;

bool sweetdeath = true;

int FUCK = 0;


bool hmfuck = true;

bool fuckdie = true;

bool firstwts6 = true;

bool firstwts62 = true;




bool world_to_screen6(PlayerData* playerdata, vec3 worldPos, vec2* out, ImU32& outcolor)
{
    outcolor = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 1, 0, 1)); // green
    cameraData cam = playerdata->GetCameraData();


    DirectX::XMFLOAT3 inPos(worldPos.x, worldPos.y, worldPos.z);
    DirectX::XMFLOAT3 vEyePt(cam.position.x, cam.position.y, cam.position.z);
    DirectX::XMFLOAT3 vLookatPt(cam.position.x + cam.rotation.x, cam.position.y + cam.rotation.y, cam.position.z + cam.rotation.z);
    DirectX::XMFLOAT3 lookdir(cam.rotation.x, cam.rotation.y, cam.rotation.z);
    // first, do a check if eye isn't even looking anywhere near the object, if so we return false.
    // calculate direction from cam to object
    DirectX::XMFLOAT3 vDirectionToObject(cam.position.x - worldPos.x, cam.position.y - worldPos.y, cam.position.z - worldPos.z);

    DirectX::XMVECTOR lookdirVec = DirectX::XMLoadFloat3(&lookdir); //already normalised
    DirectX::XMVECTOR vDirectionToObjectVec = DirectX::XMLoadFloat3(&vDirectionToObject);
    vDirectionToObjectVec = DirectX::XMVector3Normalize(vDirectionToObjectVec);

    // calculate the angle between vDirectionToObject & vLookatPt (where the camera is actually pointed)
    DirectX::XMVECTOR angleVec = DirectX::XMVector3AngleBetweenNormals(vDirectionToObjectVec, lookdirVec);
    float angle = DirectX::XMVectorGetX(angleVec);


    float nearClip = 0.1f;
    float farClip = 100.0f;
    float vfov = 1.832595706;
    float hfov = 2.3212879051524583373085087220899;

    if (GetKeyState('L') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
    {
        std::cout << "angle: " << angle << std::endl;
    }

    if (angle < 1.5707963267948966192313216916398) return false; // half pi. I think this should actually be based on fov but eh

    //Get Projection Matrix
    float fAspect = g_fAspect;

    //hmfuck = !hmfuck;
    //if (hmfuck)
    //{
    //    hfov = g_hfov;
    //}


    DirectX::XMMATRIX Proj = DirectX::XMMatrixPerspectiveFovRH(hfov, fAspect, nearClip, farClip);
    //DirectX::XMMATRIX testProj = DirectX::XMMatrixPerspectiveFovRH(hfov * 0.8, fAspect, nearClip, farClip);

    //fuckdie = !fuckdie;
    //if (fuckdie)
    //{
    //    outcolor = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 1, 1, 1)); // aqua

    //    if (firstwts6)
    //    {
    //        firstwts6 = false;
    //        g_nearHeight = tan(0.5f * hfov) * nearClip * 2.0f;
    //    }

    //    float nearHeight = g_nearHeight;
    //    float nearWidth = nearHeight * fAspect;


    //    Proj = DirectX::XMMatrixPerspectiveRH(nearWidth, nearHeight, nearClip, farClip);
    //}


    //Get View Matrix


    DirectX::XMFLOAT3 vUpVec(0.0f, 0.0f, 1.0f);
    DirectX::XMMATRIX View = DirectX::XMMatrixLookAtRH(DirectX::XMLoadFloat3(&vEyePt), DirectX::XMLoadFloat3(&vLookatPt), DirectX::XMLoadFloat3(&vUpVec));

    //Project our position

    //auto testVect = DirectX::XMVector3Project(DirectX::XMLoadFloat3(&inPos), 0, 0, displaySize_x, displaySize_y, nearClip, farClip, testProj, View, DirectX::XMMatrixIdentity());
    auto outVect = DirectX::XMVector3Project(DirectX::XMLoadFloat3(&inPos), 0, 0, g_screenWidth, g_screenHeight, nearClip, farClip, Proj, View, DirectX::XMMatrixIdentity());



    float outx = DirectX::XMVectorGetX(outVect);


    float adjustedy = DirectX::XMVectorGetY(outVect);
    float halfHeight = (g_screenHeight / 2);
    adjustedy = adjustedy - halfHeight;
    adjustedy = adjustedy * g_vertScale;
    adjustedy = adjustedy + halfHeight;
    float outy = adjustedy;


    if (GetKeyState('K') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
    {
        std::cout << "x1: " << outx << ", y1: " << outy << std::endl;
    }


    if (false)
    {
        return false;
        //std::cout << "screencoords outside bounds: x: " << outx << ", y: " << outy;
    }

    out->x = outx;
    out->y = outy;


    return true;

}
