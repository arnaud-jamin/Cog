#pragma once

namespace CogUtilities
{
	template<typename T>
	FORCEINLINE const FWorldContext* GetWorldContext(const T* Obj);

	FORCEINLINE const FWorldContext* GetWorldContext(const UGameInstance& GameInstance)
	{
		return GameInstance.GetWorldContext();
	}

	template<typename T>
	FORCEINLINE const FWorldContext* GetWorldContext(const TWeakObjectPtr<T>& Obj)
	{
		return Obj.IsValid() ? GetWorldContext(*Obj.Get()) : nullptr;
	}

	FORCEINLINE const FWorldContext* GetWorldContext(const UGameViewportClient& GameViewportClient)
	{
		return GetWorldContext(GameViewportClient.GetGameInstance());
	}

	FORCEINLINE const FWorldContext* GetWorldContext(const UWorld& World)
	{
		return GetWorldContext(World.GetGameInstance());
	}

	template<typename T>
	FORCEINLINE const FWorldContext* GetWorldContext(const T* Obj)
	{
		return Obj ? GetWorldContext(*Obj) : nullptr;
	}

	const FWorldContext* GetWorldContextFromNetMode(ENetMode NetMode);

	// Invalid context index for parameters that cannot be resolved to a valid world.
	static constexpr int32 INVALID_CONTEXT_INDEX = -10;

	// Standalone context index.
	static constexpr int32 STANDALONE_GAME_CONTEXT_INDEX = -2;

#if WITH_EDITOR

	// Editor context index. We are lacking flexibility here, so we might need to change it somehow.
	static constexpr int32 EDITOR_CONTEXT_INDEX = -1;

	FORCEINLINE int32 GetWorldContextId(const FWorldContext& WorldContext)
	{
		switch (WorldContext.WorldType)
		{
		case EWorldType::PIE:
			return WorldContext.PIEInstance;
		case EWorldType::Game:
			return STANDALONE_GAME_CONTEXT_INDEX;
		case EWorldType::Editor:
			return EDITOR_CONTEXT_INDEX;
		default:
			return INVALID_CONTEXT_INDEX;
		}
	}

	template<typename T>
	FORCEINLINE int32 GetWorldContextId(const T& Obj)
	{
		const FWorldContext* WorldContext = GetWorldContext(Obj);
		return WorldContext ? GetWorldContextId(*WorldContext) : INVALID_CONTEXT_INDEX;
	}

	FORCEINLINE int32 GetWorldContextId(const UWorld& World)
	{
		return (World.WorldType == EWorldType::Editor) ? EDITOR_CONTEXT_INDEX : GetWorldContextId(World.GetGameInstance());
	}

	FORCEINLINE int32 GetWorldContextId(const UWorld* World)
	{
		return World ? GetWorldContextId(*World) : INVALID_CONTEXT_INDEX;
	}

#else

	template<typename T>
	constexpr int32 GetWorldContextIndex(const T&)
	{
		// The only option is standalone game with one context.
		return STANDALONE_GAME_CONTEXT_INDEX;
	}

#endif // #if WITH_EDITOR
};
