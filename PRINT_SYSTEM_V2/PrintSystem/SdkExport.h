#ifndef SDKEXPORT_H
#define SDKEXPORT_H

#pragma once

#ifdef SDK_EXPORTS
#define SDK_API __declspec(dllexport)
#else
#define SDK_API __declspec(dllimport)
#endif

#endif // SDKEXPORT_H
