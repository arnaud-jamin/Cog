#pragma once

#include "CoreMinimal.h"
#include "CogImguiHelper.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Textures/SlateShaderResource.h"
#include "UObject/WeakObjectPtr.h"

//--------------------------------------------------------------------------------------------------------------------------
class FCogImguiTextureManager
{
public:

    // Creates an empty manager.
    FCogImguiTextureManager() = default;

    // Copying is disabled to protected resource ownership.
    FCogImguiTextureManager(const FCogImguiTextureManager&) = delete;
    FCogImguiTextureManager& operator=(const FCogImguiTextureManager&) = delete;

    // Moving transfers ownership and leaves source empty.
    FCogImguiTextureManager(FCogImguiTextureManager&&) = delete;
    FCogImguiTextureManager& operator=(FCogImguiTextureManager&&) = delete;

    void InitializeErrorTexture();

    // Find texture index by name.
    // @param Name - The name of a texture to find
    // @returns The index of a texture with given name or INDEX_NONE if there is no such texture
    CogTextureIndex FindTextureIndex(const FName& Name) const
    {
        return TextureResources.IndexOfByPredicate([&](const auto& Entry) { return Entry.GetName() == Name; });
    }

    // Get the name of a texture at given index. Returns NAME_None, if index is out of range.
    // @param Index - Index of a texture
    // @returns The name of a texture at given index or NAME_None if index is out of range.
    FName GetTextureName(CogTextureIndex Index) const
    {
        return IsInRange(Index) ? TextureResources[Index].GetName() : NAME_None;
    }

    // Get the Slate Resource Handle to a texture at given index. If index is out of range or resources are not valid
    // it returns a handle to the error texture.
    // @param Index - Index of a texture
    // @returns The Slate Resource Handle for a texture at given index or to error texture, if no valid resources were
    // found at given index
    const FSlateResourceHandle& GetTextureHandle(CogTextureIndex Index) const
    {
        return IsValidTexture(Index) ? TextureResources[Index].GetResourceHandle() : ErrorTexture.GetResourceHandle();
    }

    // Create a texture from raw data.
    // @param Name - The texture name
    // @param Width - The texture width
    // @param Height - The texture height
    // @param SrcBpp - The size in bytes of one pixel
    // @param SrcData - The source data
    // @param SrcDataCleanup - Optional function called to release source data after texture is created (only needed, if data need to be released)
    // @returns The index of a texture that was created
    CogTextureIndex CreateTexture(const FName& Name, int32 Width, int32 Height, uint32 SrcBpp, uint8* SrcData, TFunction<void(uint8*)> SrcDataCleanup = [](uint8*) {});

    // Create a plain texture.
    // @param Name - The texture name
    // @param Width - The texture width
    // @param Height - The texture height
    // @param Color - The texture color
    // @returns The index of a texture that was created
    CogTextureIndex CreatePlainTexture(const FName& Name, int32 Width, int32 Height, FColor Color);

    // Create Slate resources to an existing texture, managed externally.
    // @param Name - The texture name
    // @param Texture - The texture
    // @returns The index to created/updated texture resources
    CogTextureIndex CreateTextureResources(const FName& Name, UTexture2D* Texture);

    // Release resources for given texture. Ignores invalid indices.
    // @param Index - The index of a texture resources
    void ReleaseTextureResources(CogTextureIndex Index);

private:

    // See CreateTexture for general description.
    // Internal implementations doesn't validate name or resource uniqueness. Instead it uses NAME_ErrorTexture
    // (aka NAME_None) and INDEX_ErrorTexture (aka INDEX_NONE) to identify ErrorTexture.
    CogTextureIndex CreateTextureInternal(const FName& Name, int32 Width, int32 Height, uint32 SrcBpp, uint8* SrcData, TFunction<void(uint8*)> SrcDataCleanup = [](uint8*) {});

    // See CreatePlainTexture for general description.
    // Internal implementations doesn't validate name or resource uniqueness. Instead it uses NAME_ErrorTexture
    // (aka NAME_None) and INDEX_ErrorTexture (aka INDEX_NONE) to identify ErrorTexture.
    CogTextureIndex CreatePlainTextureInternal(const FName& Name, int32 Width, int32 Height, const FColor& Color);

    // Add or reuse texture entry.
    // @param Name - The texture name
    // @param Texture - The texture
    // @param bAddToRoot - If true, we should add texture to root to prevent garbage collection (use for own textures)
    // @returns The index of the entry that we created or reused
    CogTextureIndex AddTextureEntry(const FName& Name, UTexture2D* Texture, bool bAddToRoot);

    // Check whether index is in range allocated for TextureResources (it doesn't mean that resources are valid).
    FORCEINLINE bool IsInRange(CogTextureIndex Index) const
    {
        return static_cast<uint32>(Index) < static_cast<uint32>(TextureResources.Num());
    }

    // Check whether index is in range and whether texture resources are valid (using NAME_None sentinel).
    FORCEINLINE bool IsValidTexture(CogTextureIndex Index) const
    {
        return IsInRange(Index) && TextureResources[Index].GetName() != NAME_None;
    }

    // Entry for texture resources. Only supports explicit construction.
    struct FTextureEntry
    {
        FTextureEntry() = default;
        FTextureEntry(const FName& InName, UTexture2D* InTexture, bool bAddToRoot);
        ~FTextureEntry();

        // Copying is not supported.
        FTextureEntry(const FTextureEntry&) = delete;
        FTextureEntry& operator=(const FTextureEntry&) = delete;

        // We rely on TArray and don't implement custom move constructor...
        FTextureEntry(FTextureEntry&&) = delete;
        // ... but we need move assignment to support reusing entries.
        FTextureEntry& operator=(FTextureEntry&& Other);

        const FName& GetName() const { return Name; }
        const FSlateResourceHandle& GetResourceHandle() const;

    private:

        void Reset(bool bReleaseResources);

        FName Name = NAME_None;
        mutable FSlateResourceHandle CachedResourceHandle;
        TWeakObjectPtr<UTexture2D> Texture;
        FSlateBrush Brush;
    };

    TArray<FTextureEntry> TextureResources;
    FTextureEntry ErrorTexture;

    static constexpr EName NAME_ErrorTexture = NAME_None;
    static constexpr CogTextureIndex INDEX_ErrorTexture = INDEX_NONE;
};
