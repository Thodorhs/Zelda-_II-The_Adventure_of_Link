#pragma once

#include "MotionQuantizer.h"
#include "../KeyFrameAnimation/AnimationFilmHolder.h"
#include "../Util/EngineDefines.h"
#include "../Grid/Grid.h"
#include "SDL.h"
#include "Clipper.h"
#include "GravityHandler.h"

class Sprite {
public:
	using Mover = std::function<void(const SDL_Rect&, int* dx, int* dy)>;

protected:
	bool directMotion = false;
	GravityHandler gravity;
	byte frameNo = 0;
	SDL_Rect frameBox; // inside the film
	int x = 0, y = 0;
	bool isVisible = false;
	AnimationFilm* currFilm = nullptr;
	//BoundingArea* boundingArea = nullptr;
	unsigned zorder = 0;
	std::string typeId, stateId;
	Mover mover;
	Dim layer_scale =1;
	MotionQuantizer quantizer;
	bool GridIgnore = false;
public:

	const bool GetGridIgnore() const { return GridIgnore; }

	void setGridIgnore(const bool value) { GridIgnore = value; }
	void ChangeFilm(const std::string& id)
	{
		if(auto prev = currFilm; prev == AnimationFilmHolder::getInstance().GetFilm(id))
			return;
		currFilm =const_cast<AnimationFilm*> (AnimationFilmHolder::getInstance().GetFilm(id));
		frameNo = currFilm->GetTotalFrames(); SetFrame(0);
	}
	GravityHandler& GetGravityHandler(void)
	{
		return gravity;
	}

	const AnimationFilm* GetFilm() { return currFilm; }
	template <typename Tfunc>
	void SetMover(const Tfunc& f)
	{
		quantizer.SetMover(mover = f);
	}

	
	const SDL_Rect GetBox(void) const
	{
		return { x, y, frameBox.w, frameBox.h };
	}
	const SDL_Rect GetScaledBox(void)const
	{
		return { x, y, frameBox.w*layer_scale, frameBox.h*layer_scale };
	}
	Sprite& Move(int dx, int dy)
	{
		if (directMotion) {
			x += dx; y += dy;
		}
		else {
			quantizer.Move(GetScaledBox(), &dx, &dy);
			gravity.Check(GetScaledBox());
		}
		return *this;
	}
	void SetPos(int _x, int _y) { x = _x; y = _y; }
	void SetZorder(unsigned z) { zorder = z; }
	unsigned GetZorder(void) { return zorder; }
	void Display(const SDL_Rect& dpyArea, const Clipper& clipper, SDL_Renderer* renderer) const;
	void SetFrame(byte i) {
		if (i != frameNo) {
			assert(i < currFilm->GetTotalFrames());
			frameBox = currFilm->GetFrameBox(frameNo = i);
		}
	}
	byte GetFrame(void) const { return frameNo; }
	
	auto GetTypeId(void) -> const std::string& { return typeId; }

	void SetVisibility(bool v) { isVisible = v; }

	bool IsVisible(void) const { return isVisible; }

	bool CollisionCheck(const Sprite* s) const;

	static void Blit(SDL_Renderer* renderer,const SDL_Rect& src_rect,const SDL_Rect& dest_rect, SDL_Texture* film_bitmap);

	Sprite& SetHasDirectMotion(const bool v) { directMotion = v; return *this; }

	bool GetHasDirectMotion(void) const { return directMotion; }

	const Mover MakeSpriteGridLayerMover(TileLayer* layer, Sprite* sprite);

	Sprite(int _x, int _y,AnimationFilm* film, std::string _typeId = "") :
		x(_x), y(_y), currFilm(film), typeId(std::move(_typeId))
	{
		frameNo = currFilm->GetTotalFrames(); SetFrame(0);
	}

};


void PrepareSpriteGravityHandler(TileLayer* gridLayer, Sprite* sprite);