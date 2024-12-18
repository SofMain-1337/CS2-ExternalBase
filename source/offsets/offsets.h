#pragma once

#include <locale>
#include <json.hpp>

using json = nlohmann::json;

namespace offsets {
    // Game offsets
    inline std::ptrdiff_t dwLocalPlayerPawn;
    inline std::ptrdiff_t dwLocalPlayerController;
    inline std::ptrdiff_t dwEntityList;
    inline std::ptrdiff_t dwViewMatrix;
    inline std::ptrdiff_t dwViewAngles;

    // Player details
    inline std::ptrdiff_t m_pCameraServices;
    inline std::ptrdiff_t m_glowColorOverride;
    inline std::ptrdiff_t m_iFOV;
    inline std::ptrdiff_t m_bGlowing;
    inline std::ptrdiff_t m_bIsScoped;
    inline std::ptrdiff_t attack;
    inline std::ptrdiff_t m_iIDEntIndex;
    inline std::ptrdiff_t flFlashDuration;

    // Entity details
    inline std::ptrdiff_t m_hPawn;
    inline std::ptrdiff_t m_hPlayerPawn;
    inline std::ptrdiff_t m_Glow;
    inline std::ptrdiff_t m_iHealth;
    inline std::ptrdiff_t m_iTeamNum;
    inline std::ptrdiff_t m_vOldOrigin;
    inline std::ptrdiff_t m_entitySpottedState;
    inline std::ptrdiff_t m_vecViewOffset;
    inline std::ptrdiff_t m_fFlags;

    //aimbot offsets
    //inline std::ptrdiff_t dwLocalPlayerPawn = 25517288;  // Example; verify this for CS2
   // inline std::ptrdiff_t dwViewAngles = 27649616;  // Example; verify this for CS2

    bool UpdateOffset();
}
