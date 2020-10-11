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

void Animaker::Core::TextObject::SetRect(INT32 x, INT32 y, INT32 xLength, INT32 yLength){

	this->rect.left = x;
	this->rect.top = y;
	this->rect.right = x + xLength;
	this->rect.bottom = y + yLength;

}
