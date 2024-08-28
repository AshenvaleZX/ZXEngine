#include "EditorPanel.h"
#include "EditorGUIManager.h"

namespace ZXEngine
{
    void EditorPanel::CheckManualResize()
	{
        bool isPressing = ImGui::IsMouseDown(0);

        // ����
        if (isPressing && !mPressing)
        {
            ImVec2 currentWindowPos = ImGui::GetWindowPos();
            mPos.x = currentWindowPos.x;
            mPos.y = currentWindowPos.y;
            mDragPos = mPos;

            ImVec2 currentWindowSize = ImGui::GetWindowSize();
            mSize.x = currentWindowSize.x;
            mSize.y = currentWindowSize.y;
            mDragSize = mSize;

            PanelSizeChangeBegin();
        }
        // �ɿ�
        else if (!isPressing && mPressing)
        {
            if (mSize != mDragSize)
            {
                PanelSizeChangeEnd(mDragSize);
            }
        }
        // ��ס
        else if (isPressing)
        {
            ImVec2 currentWindowPos = ImGui::GetWindowPos();
            ImVec2 currentWindowSize = ImGui::GetWindowSize();

            EditorPanelEdgeFlags flags = ZX_EDITOR_PANEL_EDGE_NONE;
            if (mDragSize.x != currentWindowSize.x)
            {
                if (mDragPos.x == currentWindowPos.x)
                    flags |= ZX_EDITOR_PANEL_EDGE_RIGHT;
                else
                    flags |= ZX_EDITOR_PANEL_EDGE_LEFT;
            }
            if (mDragSize.y != currentWindowSize.y)
            {
                if (mDragPos.y == currentWindowPos.y)
                    flags |= ZX_EDITOR_PANEL_EDGE_BOTTOM;
                else
                    flags |= ZX_EDITOR_PANEL_EDGE_TOP;
            }

            mDragPos.x = currentWindowPos.x;
            mDragPos.y = currentWindowPos.y;

            mDragSize.x = currentWindowSize.x;
            mDragSize.y = currentWindowSize.y;

            PanelSizeChanging(mDragSize, flags);
		}

        mPressing = isPressing;
    }
}