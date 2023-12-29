#pragma once

#include "CoreMinimal.h"

namespace EDrawDebugTrace { enum Type : int; }

class UWorld;
struct FHitResult;
class AActor;

class COGDEBUG_API FCogDebugDrawHelper
{
public:

    static void DrawArc(const UWorld* InWorld, const FMatrix& Matrix, const float InnterRadius, const float OuterRadius, const float AngleStart, const float AngleEnd, const int32 Segments, const FColor& Color, const bool bPersistentLines = false, const float LifeTime = -1.f, const uint8 DepthPriority = 0U, const float Thickness = 0.0f);

    static void DrawArc(const UWorld* InWorld, const FMatrix& Matrix, const float InnterRadius, const float OuterRadius, const float ArcAngle, const int32 Segments, const FColor& Color, const bool bPersistentLines = false, const float LifeTime = -1.f, const uint8 DepthPriority = 0U, const float Thickness = 0.0f);

    static void DrawSphere(const UWorld* InWorld, const FVector& Center, const float Radius, const int32 Segments, const FColor& Color, const bool bPersistentLines = false, const float LifeTime = -1.f, const uint8 DepthPriority = 0U, const float Thickness = 0.0f);
    
    static void DrawFrustum(const UWorld* World, const FMatrix& Matrix, const float Angle, const float AspectRatio, const float NearPlane, const float FarPlane, const FColor& Color, const bool bPersistentLines = false, const float LifeTime = -1.f, const uint8 DepthPriority = 0U, const float Thickness = 0.0f);
    
    static void DrawFlatCapsule(const UWorld* InWorld, const FVector2D& Start, const FVector2D& End, const float Radius, const float Z, const float Segments, const FColor& Color, const bool bPersistentLines = false, const float LifeTime = -1.0f, const uint8 DepthPriority = 0, const float Thickness = 0.0f);
    
    static void DrawRaycastSingle(const UWorld* World, const FVector& Start, const FVector& End, const EDrawDebugTrace::Type DrawType, const bool bHit, const FHitResult& Hit, const float HitSize, const FLinearColor DrawColor, const FLinearColor DrawHitColor, const float DrawDuration, const uint8 DepthPriority = 0);
    
    static void DrawSphereOverlapMulti(const UWorld* World, const FVector& Position, const float Radius, const EDrawDebugTrace::Type DrawType, const bool bOverlap, const TArray<AActor*>& OutActors, const FLinearColor DrawColor, const FLinearColor DrawHitColor, const float DrawDuration = 0); 
    
    static void DrawSphereOverlapSingle(const UWorld* World, const FVector& Position, const float Radius, const FColor& DrawColor, const bool DrawPersistent, const float DrawDuration = -1.f, const uint8 DepthPriority = 0);
    
    static void DrawCapsuleCastMulti(const UWorld* World, const FVector& Start, const FVector& End, const FQuat& Rotation, const float HalfHeight, const float Radius, const EDrawDebugTrace::Type DrawType, const bool bHit, const TArray<FHitResult>& OutHits, const FLinearColor DrawColor, const FLinearColor DrawHitColor, const float DrawDuration = 0);
    
    static void DrawCapsuleCastSingle(const UWorld* World, const FVector& Start, const FVector& End, const FQuat& Rotation, const float HalfHeight, const float Radius, const FColor& DrawColor, const bool DrawPersistent, const float DrawDuration = -1.f, const uint8 DepthPriority = 0);
    
    static void DrawSphereCastMulti(const UWorld* World, const FVector& Start, const FVector& End, const float Radius, const EDrawDebugTrace::Type DrawType, const bool bHit, const TArray<FHitResult>& OutHits, const FLinearColor DrawColor, const FLinearColor DrawHitColor, const float DrawDuration = 0); 
    
    static void DrawSphereCastSingle(const UWorld* World, const FVector& Start, const FVector& End, const float Radius, const FColor& DrawColor, const bool DrawPersistent, const float LifeTime = -1.f, const uint8 DepthPriority = 0); 
    
    static void DrawHitResults(const UWorld* World, const TArray<FHitResult>& OutHits, const EDrawDebugTrace::Type DrawType, const bool ShowHitIndex, const float HitSize, const FLinearColor HitColor, const float DrawDuration, const uint8 DepthPriority = 0);
    
    static void DrawHitResultsDiscarded(const UWorld* World, const TArray<FHitResult>& AllHits, const TArray<FHitResult>& KeptHits, const EDrawDebugTrace::Type DrawType, const float HitSize, const FLinearColor DrawColor, const float DrawDuration, const uint8 DepthPriority = 0); 
    
    static void DrawHitResult(const UWorld* World, const FHitResult& Hit, const int HitIndex, const EDrawDebugTrace::Type DrawType, const bool ShowHitIndex, const float HitSize, const FLinearColor HitColor, const float DrawDuration, const uint8 DepthPriority = 0); 

    static void DrawQuad(const UWorld* World, const FVector& Position, const FQuat& Rotation, const FVector2D& Extents, const FColor& Color, bool bPersistent = false, float LifeTime = -1, uint8 DepthPriority = 0, const float Thickness = 0.0f);

    static void DrawSolidQuad(const UWorld* World, const FVector& Position, const FQuat& Rotation, const FVector2D& Extents, const FColor& Color, bool bPersistent = false, float LifeTime = -1, uint8 DepthPriority = 0);

};
