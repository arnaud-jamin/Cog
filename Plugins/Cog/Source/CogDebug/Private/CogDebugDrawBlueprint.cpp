#include "CogDebugDrawBlueprint.h"

#include "CogDebugDraw.h"
#include "CogCommonLog.h"
#include "CogDebugLog.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawString(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FString& Text, const FVector Location, const FLinearColor Color, const bool Persistent)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::String(*LogCategoryPtr, WorldContextObject, Text, Location, Color.ToFColor(true), Persistent);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawPoint(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector Location, const float Size, const FLinearColor Color, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Point(*LogCategoryPtr, WorldContextObject, Location, Size, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawSegment(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector SegmentStart, const FVector SegmentEnd, const FLinearColor Color, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Segment(*LogCategoryPtr, WorldContextObject, SegmentStart, SegmentEnd, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawArrow(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector SegmentStart, const FVector SegmentEnd, const FLinearColor Color, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Arrow(*LogCategoryPtr, WorldContextObject, SegmentStart, SegmentEnd, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawAxis(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector Location, const FRotator Rotation, const float Scale, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Axis(*LogCategoryPtr, WorldContextObject, Location, Rotation, Scale, Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawSphere(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector Location, const float Radius, const FLinearColor Color, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Sphere(*LogCategoryPtr, WorldContextObject, Location, Radius, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawBox(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector Center, const FVector Extent, const FQuat Rotation, const FLinearColor Color, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Box(*LogCategoryPtr, WorldContextObject, Center, Extent, Rotation, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawSolidBox(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector Center, const FVector Extent, const FQuat Rotation, const FLinearColor Color, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::SolidBox(*LogCategoryPtr, WorldContextObject, Center, Extent, Rotation, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawCapsule(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector Center, const float HalfHeight, const float Radius, const FQuat Rotation, const FLinearColor Color, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Capsule(*LogCategoryPtr, WorldContextObject, Center, HalfHeight, Radius, Rotation, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawCircle(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FMatrix& Matrix, const float Radius, const FLinearColor Color, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Circle(*LogCategoryPtr, WorldContextObject, Matrix, Radius, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawCircleArc(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FMatrix& Matrix, const float InnerRadius, const float OuterRadius, const float Angle, const FLinearColor Color, const bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::CircleArc(*LogCategoryPtr, WorldContextObject, Matrix, InnerRadius, OuterRadius, Angle, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawPoints(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const TArray<FVector>& Points, const float Radius, const FLinearColor StartColor, const FLinearColor EndColor, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Points(*LogCategoryPtr, WorldContextObject, Points, Radius, StartColor.ToFColor(true), EndColor.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawPath(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const TArray<FVector>& Points, const float PointSize, const FLinearColor StartColor, const FLinearColor EndColor, const bool Persistent, const uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Path(*LogCategoryPtr, WorldContextObject, Points, PointSize, StartColor.ToFColor(true), EndColor.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawString2D(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FString& Text, const FVector2D Location, const FLinearColor Color, const bool Persistent)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::String2D(*LogCategoryPtr, WorldContextObject, Text, Location, Color.ToFColor(true), Persistent);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawSegment2D(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector2D SegmentStart, const FVector2D SegmentEnd, const FLinearColor Color, const bool Persistent)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Segment2D(*LogCategoryPtr, WorldContextObject, SegmentStart, SegmentEnd, Color.ToFColor(true), Persistent);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawCircle2D(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector2D Location, const float Radius, const FLinearColor Color, const bool Persistent)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Circle2D(*LogCategoryPtr, WorldContextObject, Location, Radius, Color.ToFColor(true), Persistent);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawRect2D(const UObject* WorldContextObject, const FCogLogCategory LogCategory, const FVector2D Min, const FVector2D Max, const FLinearColor Color, const bool Persistent)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = FCogDebugLog::GetLogCategoryBase(LogCategory))
    {
        FCogDebugDraw::Rect2D(*LogCategoryPtr, WorldContextObject, Min, Max, Color.ToFColor(true), Persistent);
    }
#endif //ENABLE_COG
}

