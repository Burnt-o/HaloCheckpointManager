#include "pch.h"
#include "StaticHookWrapper.h"


template <typename T> StaticHookWrapper_MidHook<T>* StaticHookWrapper_MidHook<T>::mInstance = nullptr;
template <typename T> void StaticHookWrapper_MidHook<T>::staticFunc(SafetyHookContext ctx) { return mFunc(ctx); }



template <typename T, typename Params, typename ...Args> StaticHookWrapper_InlineHook<T, Params, Args...>* StaticHookWrapper_InlineHook<T, Params, Args...>::mInstance = nullptr;
template <typename T, typename Params, typename ...Args> Params StaticHookWrapper_InlineHook<T, Params, Args...>::staticFunc(Args...) { return mFunc(Args...); }