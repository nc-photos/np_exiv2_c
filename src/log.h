#pragma once

#ifdef NP_DEBUG
#define LOG(...) printf(__VA_ARGS__)
#define LOGE(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...)
#define LOGE(...)
#endif
