#include "obj.h"

MES::SceneObj::SceneObj(RE::TESObjectREFR* pRef)
	:
	pRef(pRef)
{
}

RE::TESObjectREFR* MES::SceneObj::GetRef() const
{
	return pRef;
}
