#include "CogDebugDrawBlueprint.h"

#include "CogDebugDraw.h"

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawString(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FString& Text, const FVector Location, const FLinearColor Color, bool Persistent)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::String(*LogCategoryPtr, WorldContextObject, Text, Location, Color.ToFColor(true), Persistent);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawPoint(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, float Size, const FLinearColor Color, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Point(*LogCategoryPtr, WorldContextObject, Location, Size, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawSegment(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector SegmentStart, const FVector SegmentEnd, const FLinearColor Color, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Segment(*LogCategoryPtr, WorldContextObject, SegmentStart, SegmentEnd, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawArrow(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector SegmentStart, const FVector SegmentEnd, const FLinearColor Color, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Arrow(*LogCategoryPtr, WorldContextObject, SegmentStart, SegmentEnd, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawAxis(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, const FRotator Rotation, float Scale, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Axis(*LogCategoryPtr, WorldContextObject, Location, Rotation, Scale, Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawSphere(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Location, float Radius, const FLinearColor Color, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Sphere(*LogCategoryPtr, WorldContextObject, Location, Radius, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawBox(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const FVector Extent, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Box(*LogCategoryPtr, WorldContextObject, Center, Extent, Rotation, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawSolidBox(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const FVector Extent, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::SolidBox(*LogCategoryPtr, WorldContextObject, Center, Extent, Rotation, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawCapsule(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector Center, const float HalfHeight, const float Radius, const FQuat Rotation, const FLinearColor Color, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Capsule(*LogCategoryPtr, WorldContextObject, Center, HalfHeight, Radius, Rotation, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawCircle(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FMatrix& Matrix, float Radius, const FLinearColor Color, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Circle(*LogCategoryPtr, WorldContextObject, Matrix, Radius, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawCircleArc(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FMatrix& Matrix, float InnerRadius, float OuterRadius, float Angle, const FLinearColor Color, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::CircleArc(*LogCategoryPtr, WorldContextObject, Matrix, InnerRadius, OuterRadius, Angle, Color.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawPoints(const UObject* WorldContextObject, FCogLogCategory LogCategory, const TArray<FVector>& Points, float Radius, const FLinearColor StartColor, const FLinearColor EndColor, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Points(*LogCategoryPtr, WorldContextObject, Points, Radius, StartColor.ToFColor(true), EndColor.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawPath(const UObject* WorldContextObject, FCogLogCategory LogCategory, const TArray<FVector>& Points, float PointSize, const FLinearColor StartColor, const FLinearColor EndColor, bool Persistent, uint8 DepthPriority)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Path(*LogCategoryPtr, WorldContextObject, Points, PointSize, StartColor.ToFColor(true), EndColor.ToFColor(true), Persistent, DepthPriority);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawString2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FString& Text, const FVector2D Location, const FLinearColor Color, bool Persistent)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::String2D(*LogCategoryPtr, WorldContextObject, Text, Location, Color.ToFColor(true), Persistent);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawSegment2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D SegmentStart, const FVector2D SegmentEnd, const FLinearColor Color, bool Persistent)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Segment2D(*LogCategoryPtr, WorldContextObject, SegmentStart, SegmentEnd, Color.ToFColor(true), Persistent);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawCircle2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D Location, float Radius, const FLinearColor Color, bool Persistent)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Circle2D(*LogCategoryPtr, WorldContextObject, Location, Radius, Color.ToFColor(true), Persistent);
    }
#endif //ENABLE_COG
}

//--------------------------------------------------------------------------------------------------------------------------
void UCogDebugDrawBlueprint::DebugDrawRect2D(const UObject* WorldContextObject, FCogLogCategory LogCategory, const FVector2D Min, const FVector2D Max, const FLinearColor Color, bool Persistent)
{
#if ENABLE_COG
    if (const FLogCategoryBase* LogCategoryPtr = LogCategory.GetLogCategory())
    {
        FCogDebugDraw::Rect2D(*LogCategoryPtr, WorldContextObject, Min, Max, Color.ToFColor(true), Persistent);
    }
#endif //ENABLE_COG
}

