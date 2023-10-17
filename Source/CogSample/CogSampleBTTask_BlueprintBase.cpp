#include "CogSampleBTTask_BlueprintBase.h"

//--------------------------------------------------------------------------------------------------------------------------
FString UCogSampleBTTask_BlueprintBase::GetStaticDescription() const
{
	FString CustomDescripton = GetCustomStaticDescription();

	if (CustomDescripton.IsEmpty() == false)
	{
		return CustomDescripton;
	}

	return Super::GetStaticDescription();
}
