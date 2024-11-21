#include "CogWorldUtility.h"

namespace CogUtilities
{
	const FWorldContext* GetWorldContextFromNetMode(ENetMode NetMode)
	{
		checkf(GEngine, TEXT("GEngine required to get list of worlds."));

		for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
		{
			if (WorldContext.World() && WorldContext.World()->GetNetMode() == NetMode)
			{
				return &WorldContext;
			}
		}

		return nullptr;
	}
}