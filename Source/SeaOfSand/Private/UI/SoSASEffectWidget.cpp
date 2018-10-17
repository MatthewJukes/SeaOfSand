// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSASEffectWidget.h"



USoSASEffectWidget::USoSASEffectWidget()
{

}

FEffectData* USoSASEffectWidget::GetEffect()
{
	return EffectReference;
}


//void USoSASEffectWidget::SetEffectReference(FASEffectData& Effect)
//{
//	EffectReference = Effect;
// }
