#include"Animaker.h"

using namespace Animaker::Core;
using namespace Animaker::Animation;
using namespace Animaker::Math;

Animaker::Core::TextObject::TextObject(){
	this->text = nullptr;
	this->pc_textFormat = nullptr;
}

void Animaker::Core::TextObject::SetText(const WCHAR* text){
	this->text = (WCHAR*)text;
	this->textLength = (INT32)wcslen(this->text);
}

void Animaker::Core::TextObject::SetRect(float x, float y, float xLength, float yLength){
	this->x = x;
	this->y = y;
	this->xLength = xLength;
	this->yLength = yLength;
}

void Animaker::Core::TextObject::Move(Math::Float4 from, Math::Float4 to, float t){

	Math::Float4 temp;
	temp = from * (1.0f - t) + to * t;

	this->rect.left = temp.x;
	this->rect.top = temp.y;
	this->rect.right = this->rect.left + this->xLength;
	this->rect.bottom = this->rect.top + this->yLength;

}
