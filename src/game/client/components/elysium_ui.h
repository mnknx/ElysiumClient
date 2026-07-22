/* ElysiumClient - central UI theme system.                                    */
/* One place to define every color, spacing and size token used by the         */
/* Elysium UI. Do not hardcode colors/sizes in screens - read them from here.   */
#ifndef GAME_CLIENT_COMPONENTS_ELYSIUM_UI_H
#define GAME_CLIENT_COMPONENTS_ELYSIUM_UI_H

#include <base/color.h>

class CUIRect;

namespace ElysiumUi {

// All theme tokens the client may need. Colors are ColorRGBA (0..1),
// sizes are in the menu's virtual coordinate space (same units DDNet uses).
struct STheme
{
	// Surfaces
	ColorRGBA m_BgPrimary; // main background
	ColorRGBA m_BgSecondary; // secondary panels
	ColorRGBA m_CardBg; // card / list item background
	ColorRGBA m_Hover; // hover overlay
	ColorRGBA m_Selected; // active / selected item

	// Accents & status
	ColorRGBA m_AccentPrimary;
	ColorRGBA m_AccentSecondary;
	ColorRGBA m_Success;
	ColorRGBA m_Warning;
	ColorRGBA m_Error;
	ColorRGBA m_Disabled;

	// Text & lines
	ColorRGBA m_TextPrimary;
	ColorRGBA m_TextSecondary;
	ColorRGBA m_Border;

	// Metrics
	float m_Rounding; // corner radius
	float m_SpacingSmall;
	float m_Spacing; // standard gap between elements
	float m_SpacingLarge;
	float m_ButtonHeight;
	float m_InputHeight;

	// Typography
	float m_FontSizeTitle;
	float m_FontSizeHeading;
	float m_FontSizeNormal;
	float m_FontSizeSmall;
};

// The currently active theme, derived from config (base / high-contrast).
const STheme &Theme();

// User interface scale factor (m_ClElysiumUiScale, 1.0 == 100%).
float UiScale();

// Whether panels should be drawn with transparency (m_ClElysiumTransparentPanels).
bool TransparentPanels();

// Whether short UI animations are enabled (respects reduce-animations).
bool AnimationsEnabled();

// Semantic color for a ping value: success (low) / warning (mid) / error (high).
ColorRGBA PingColor(int Ping);

// Draw an accessible signal-bar indicator for a ping value inside Rect.
// The number of lit bars encodes quality by shape (not color alone).
void DrawPingIndicator(const CUIRect &Rect, int Ping);

// Shared surface drawing helpers (self-contained via CUIRect::Draw).
void DrawCard(const CUIRect &Rect, float AlphaMul = 1.0f);
void DrawPanel(const CUIRect &Rect, float AlphaMul = 1.0f);
void DrawHover(const CUIRect &Rect);
void DrawSelected(const CUIRect &Rect);

} // namespace ElysiumUi

#endif
