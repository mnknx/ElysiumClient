/* ElysiumClient - central UI theme system implementation. */
#include "elysium_ui.h"

#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <game/client/ui_rect.h>

#include <algorithm>

namespace ElysiumUi {

// Base Elysium palette: deep navy / grey surfaces with a blue-violet primary
// accent and a turquoise secondary accent.
static STheme BaseTheme()
{
	STheme t;
	t.m_BgPrimary = ColorRGBA(0.055f, 0.071f, 0.125f, 1.0f);
	t.m_BgSecondary = ColorRGBA(0.082f, 0.106f, 0.180f, 1.0f);
	t.m_CardBg = ColorRGBA(0.110f, 0.141f, 0.224f, 0.92f);
	t.m_Hover = ColorRGBA(0.420f, 0.490f, 1.000f, 0.14f);
	t.m_Selected = ColorRGBA(0.420f, 0.490f, 1.000f, 0.28f);

	t.m_AccentPrimary = ColorRGBA(0.420f, 0.490f, 1.000f, 1.0f);
	t.m_AccentSecondary = ColorRGBA(0.180f, 0.880f, 0.750f, 1.0f);
	t.m_Success = ColorRGBA(0.204f, 0.827f, 0.600f, 1.0f);
	t.m_Warning = ColorRGBA(0.984f, 0.749f, 0.141f, 1.0f);
	t.m_Error = ColorRGBA(0.973f, 0.443f, 0.443f, 1.0f);
	t.m_Disabled = ColorRGBA(0.353f, 0.384f, 0.447f, 1.0f);

	t.m_TextPrimary = ColorRGBA(0.949f, 0.961f, 0.980f, 1.0f);
	t.m_TextSecondary = ColorRGBA(0.651f, 0.682f, 0.752f, 1.0f);
	t.m_Border = ColorRGBA(0.165f, 0.200f, 0.314f, 1.0f);

	t.m_Rounding = 6.0f;
	t.m_SpacingSmall = 5.0f;
	t.m_Spacing = 10.0f;
	t.m_SpacingLarge = 20.0f;
	t.m_ButtonHeight = 28.0f;
	t.m_InputHeight = 26.0f;

	t.m_FontSizeTitle = 24.0f;
	t.m_FontSizeHeading = 20.0f;
	t.m_FontSizeNormal = 14.0f;
	t.m_FontSizeSmall = 10.0f;
	return t;
}

// High-contrast variant: opaque surfaces, brighter text, stronger borders.
static STheme HighContrastTheme()
{
	STheme t = BaseTheme();
	t.m_BgPrimary = ColorRGBA(0.020f, 0.027f, 0.055f, 1.0f);
	t.m_BgSecondary = ColorRGBA(0.047f, 0.063f, 0.118f, 1.0f);
	t.m_CardBg = ColorRGBA(0.078f, 0.102f, 0.180f, 1.0f);
	t.m_Selected = ColorRGBA(0.420f, 0.490f, 1.000f, 0.45f);
	t.m_TextPrimary = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	t.m_TextSecondary = ColorRGBA(0.820f, 0.850f, 0.910f, 1.0f);
	t.m_Border = ColorRGBA(0.320f, 0.380f, 0.540f, 1.0f);
	return t;
}

const STheme &Theme()
{
	static const STheme s_Base = BaseTheme();
	static const STheme s_HighContrast = HighContrastTheme();
	return g_Config.m_ClElysiumHighContrast ? s_HighContrast : s_Base;
}

float UiScale()
{
	return std::clamp(g_Config.m_ClElysiumUiScale, 50, 200) / 100.0f;
}

bool TransparentPanels()
{
	return g_Config.m_ClElysiumTransparentPanels != 0 && g_Config.m_ClElysiumHighContrast == 0;
}

bool AnimationsEnabled()
{
	return g_Config.m_ClElysiumAnimations != 0 && g_Config.m_ClElysiumReduceAnimations == 0;
}

ColorRGBA PingColor(int Ping)
{
	const STheme &t = Theme();
	if(Ping < 0)
		return t.m_Disabled;
	if(Ping <= 80)
		return t.m_Success;
	if(Ping <= 180)
		return t.m_Warning;
	return t.m_Error;
}

void DrawPingIndicator(const CUIRect &Rect, int Ping)
{
	const STheme &t = Theme();
	const ColorRGBA On = PingColor(Ping);
	const ColorRGBA Off = t.m_Disabled.WithMultipliedAlpha(0.35f);

	int Level; // number of lit bars, 0..4
	if(Ping < 0)
		Level = 0;
	else if(Ping <= 40)
		Level = 4;
	else if(Ping <= 80)
		Level = 3;
	else if(Ping <= 150)
		Level = 2;
	else
		Level = 1;

	const int NumBars = 4;
	const float BarW = 2.5f;
	const float Gap = 1.5f;
	const float TotalW = NumBars * BarW + (NumBars - 1) * Gap;
	const float MaxH = std::min(Rect.h * 0.7f, 10.0f);
	const float BaseY = Rect.y + (Rect.h + MaxH) / 2.0f; // bottom of the bars
	float x = Rect.x + std::max(0.0f, (Rect.w - TotalW) / 2.0f);

	for(int i = 0; i < NumBars; i++)
	{
		const float BarH = MaxH * (i + 1) / NumBars;
		CUIRect Bar;
		Bar.x = x;
		Bar.w = BarW;
		Bar.h = BarH;
		Bar.y = BaseY - BarH;
		Bar.Draw(i < Level ? On : Off, IGraphics::CORNER_ALL, 0.5f);
		x += BarW + Gap;
	}
}

void DrawCard(const CUIRect &Rect, float AlphaMul)
{
	ColorRGBA Color = Theme().m_CardBg;
	if(!TransparentPanels())
		Color.a = 1.0f;
	Color.a *= AlphaMul;
	Rect.Draw(Color, IGraphics::CORNER_ALL, Theme().m_Rounding);
}

void DrawPanel(const CUIRect &Rect, float AlphaMul)
{
	ColorRGBA Color = Theme().m_BgSecondary;
	if(TransparentPanels())
		Color.a = 0.90f;
	Color.a *= AlphaMul;
	Rect.Draw(Color, IGraphics::CORNER_ALL, Theme().m_Rounding);
}

void DrawHover(const CUIRect &Rect)
{
	Rect.Draw(Theme().m_Hover, IGraphics::CORNER_ALL, Theme().m_Rounding);
}

void DrawSelected(const CUIRect &Rect)
{
	Rect.Draw(Theme().m_Selected, IGraphics::CORNER_ALL, Theme().m_Rounding);
}

} // namespace ElysiumUi
