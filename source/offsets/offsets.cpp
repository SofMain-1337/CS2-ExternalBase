#include "offsets.h"
#include <string>
#include <Windows.h>
#include <array>
#include <regex>

bool Get(std::string url, std::string& response)
{
    response = "";
    std::string cmd = "curl -s -X GET " + url;

    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);

    if (!pipe)
    {
        return false;
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
    {
        response += buffer.data();
    }

    std::regex pattern("\\d{3}:");
    if (std::regex_search(response, pattern))
    {
        return false;
    }

    return true;
}

bool offsets::UpdateOffset()
{
    const std::string offsetsUrl = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json";
    const std::string buttonsUrl = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/buttons.json";
    const std::string client_dllUrl = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.json";
    std::string offsetsData, buttonsData, client_dllData;

    if (!Get(offsetsUrl, offsetsData) || !Get(buttonsUrl, buttonsData) || !Get(client_dllUrl, client_dllData))
        return 0;

    json offsetsJson = json::parse(offsetsData);
    json client_dllJson = json::parse(client_dllData)["client.dll"]["classes"];
    json buttonsJson = json::parse(buttonsData);

    // Game offsets
    dwLocalPlayerPawn= offsetsJson["client.dll"]["dwLocalPlayerPawn"];
    dwLocalPlayerController= offsetsJson["client.dll"]["dwLocalPlayerController"];
    dwEntityList= offsetsJson["client.dll"]["dwEntityList"];
    dwViewMatrix= offsetsJson["client.dll"]["dwViewMatrix"];
    dwViewAngles= offsetsJson["client.dll"]["dwViewAngles"];

    // Player details
    m_pCameraServices= client_dllJson["C_BasePlayerPawn"]["fields"]["m_pCameraServices"];
    m_glowColorOverride= client_dllJson["CGlowProperty"]["fields"]["m_glowColorOverride"];
    m_iFOV= client_dllJson["CCSPlayerBase_CameraServices"]["fields"]["m_iFOV"];
    m_bGlowing= client_dllJson["CGlowProperty"]["fields"]["m_bGlowing"];
    m_bIsScoped= client_dllJson["C_CSPlayerPawn"]["fields"]["m_bIsScoped"];
    attack= buttonsJson["client.dll"]["attack"];
    m_iIDEntIndex= client_dllJson["C_CSPlayerPawnBase"]["fields"]["m_iIDEntIndex"];;
    flFlashDuration= client_dllJson["C_CSPlayerPawnBase"]["fields"]["m_flFlashDuration"];

    // Entity details
    m_hPawn= client_dllJson["CBasePlayerController"]["fields"]["m_hPawn"];
    m_hPlayerPawn= client_dllJson["CCSPlayerController"]["fields"]["m_hPlayerPawn"];
    m_Glow= client_dllJson["C_BaseModelEntity"]["fields"]["m_Glow"];
    m_iHealth= client_dllJson["C_BaseEntity"]["fields"]["m_iHealth"];
    m_iTeamNum= client_dllJson["C_BaseEntity"]["fields"]["m_iTeamNum"];;
    m_vOldOrigin= client_dllJson["C_BasePlayerPawn"]["fields"]["m_vOldOrigin"];
    m_entitySpottedState= client_dllJson["C_CSPlayerPawn"]["fields"]["m_entitySpottedState"];
    m_vecViewOffset= client_dllJson["C_BaseModelEntity"]["fields"]["m_vecViewOffset"];
    m_fFlags= client_dllJson["C_BaseEntity"]["fields"]["m_fFlags"];

    return 1;

}