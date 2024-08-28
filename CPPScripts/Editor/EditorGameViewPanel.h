#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class EditorGameViewPanel : public EditorPanel
	{
	public:
		EditorGameViewPanel() = default;
		~EditorGameViewPanel() = default;

		virtual void DrawPanel();

	protected:
		virtual void PanelSizeChangeBegin();
		virtual void PanelSizeChanging(const Vector2& size, EditorPanelEdgeFlags flags);
		virtual void PanelSizeChangeEnd(const Vector2& size);

	private:
		bool mFirstDraw = true;
		uint32_t mLastWidth = 0;
		uint32_t mLastHeight = 0;
	};
}