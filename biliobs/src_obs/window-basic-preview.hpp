#pragma once

#include <obs.hpp>
#include <graphics/vec2.h>
#include <graphics/matrix4.h>
#include "qt-display.hpp"

class OBSBasic;
class QMouseEvent;

#define ITEM_LEFT   (1<<0)
#define ITEM_RIGHT  (1<<1)
#define ITEM_TOP    (1<<2)
#define ITEM_BOTTOM (1<<3)

enum class ItemHandle : uint32_t {
	None         = 0,
	TopLeft      = ITEM_TOP | ITEM_LEFT,
	TopCenter    = ITEM_TOP,
	TopRight     = ITEM_TOP | ITEM_RIGHT,
	CenterLeft   = ITEM_LEFT,
	CenterRight  = ITEM_RIGHT,
	BottomLeft   = ITEM_BOTTOM | ITEM_LEFT,
	BottomCenter = ITEM_BOTTOM,
	BottomRight  = ITEM_BOTTOM | ITEM_RIGHT
};

class OBSBasicPreview : public OBSQTDisplay {
	Q_OBJECT

private:
	OBSSceneItem stretchItem;
	ItemHandle   stretchHandle = ItemHandle::None;
	vec2         stretchItemSize;
	matrix4      screenToItem;
	matrix4      itemToScreen;

	OBSSource    cropFilter;
	OBSData      cropSettingsOnStart;

	vec2         itemPosOnStart;
	vec2         itemBoundOnStart;

	vec2         startPos;
	vec2         lastMoveOffset;
	bool         mouseDown      = false;
	bool         mouseMoved     = false;
	bool         mouseOverItems = false;

	static vec2 GetMouseEventPos(QMouseEvent *event);
	static bool DrawSelectedItem(obs_scene_t *scene, obs_sceneitem_t *item,
		void *param);

	static OBSSceneItem GetItemAtPos(const vec2 &pos, bool selectBelow);
	static bool SelectedAtPos(const vec2 &pos);

	static void DoSelect(const vec2 &pos);
	static void DoCtrlSelect(const vec2 &pos);

	static vec3 GetScreenSnapOffset(const vec3 &tl, const vec3 &br);

	void GetStretchHandleData(const vec2 &pos);

	void SnapStretchingToScreen(vec3 &tl, vec3 &br);
	void ClampAspect(vec3 &tl, vec3 &br, vec2 &size, const vec2 &baseSize);
	vec3 CalculateStretchPos(const vec3 &tl, const vec3 &br);
	void StretchItem(const vec2 &pos);
	void CropItem(const vec2& pos);

	static void SnapItemMovement(vec2 &offset);
	void MoveItems(const vec2 &pos);

	void ProcessClick(const vec2 &pos);

	void mAddCursorShape(QMouseEvent *e);

public:
	OBSBasicPreview(QWidget *parent, Qt::WindowFlags flags = 0);

	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual bool eventFilter(QObject *obj, QEvent *e) override;

	void DrawSceneEditing();

	/* use libobs allocator for alignment because the matrices itemToScreen
	 * and screenToItem may contain SSE data, which will cause SSE
	 * instructions to crash if the data is not aligned to at least a 16
	 * byte boundry. */
	static inline void* operator new(size_t size) {return bmalloc(size);}
	static inline void operator delete(void* ptr) {bfree(ptr);}

	void moveItemByKeyboard_(vec2 moveOffset);

signals:
	void ContextMenuSignal(QPoint mousePos);

private:
	bool isIgnoreClickOnce;

public:
	void ignoreClickOnce();
};
