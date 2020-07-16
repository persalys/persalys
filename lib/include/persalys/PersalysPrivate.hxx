
#ifndef PERSALYS_PRIVATE_HXX
#define PERSALYS_PRIVATE_HXX

/* From http://gcc.gnu.org/wiki/Visibility */
/* Generic helper definitions for shared library support */
#if defined _WIN32 || defined __CYGWIN__
#define PERSALYS_HELPER_DLL_IMPORT __declspec(dllimport)
#define PERSALYS_HELPER_DLL_EXPORT __declspec(dllexport)
#define PERSALYS_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define PERSALYS_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define PERSALYS_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define PERSALYS_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define PERSALYS_HELPER_DLL_IMPORT
#define PERSALYS_HELPER_DLL_EXPORT
#define PERSALYS_HELPER_DLL_LOCAL
#endif
#endif

/* Now we use the generic helper definitions above to define PERSALYS_<LIB>_API and PERSALYS_<LIB>_LOCAL.
 * PERSALYS_<LIB>_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
 * PERSALYS_<LIB>_LOCAL is used for non-api symbols. */

#ifndef PERSALYS_BASE_STATIC /* defined if Persalys is compiled as a DLL */
#ifdef PERSALYS_BASE_DLL_EXPORTS /* defined if we are building the Persalys DLLs (instead of using it) */
#define PERSALYS_BASE_API PERSALYS_HELPER_DLL_EXPORT
#else
#define PERSALYS_BASE_API PERSALYS_HELPER_DLL_IMPORT
#endif /* PERSALYS_BASE_DLL_EXPORTS */
#define PERSALYS_BASE_LOCAL PERSALYS_HELPER_DLL_LOCAL
#else /* PERSALYS_BASE_STATIC is defined: this means Persalys is compiled as static lib. */
#define PERSALYS_BASE_API
#define PERSALYS_BASE_LOCAL
#endif /* !PERSALYS_BASE_STATIC */

#ifndef PERSALYS_MODEL_STATIC /* defined if Persalys is compiled as a DLL */
#ifdef PERSALYS_MODEL_DLL_EXPORTS /* defined if we are building the Persalys DLLs (instead of using it) */
#define PERSALYS_MODEL_API PERSALYS_HELPER_DLL_EXPORT
#else
#define PERSALYS_MODEL_API PERSALYS_HELPER_DLL_IMPORT
#endif /* PERSALYS_MODEL_DLL_EXPORTS */
#define PERSALYS_MODEL_LOCAL PERSALYS_HELPER_DLL_LOCAL
#else /* PERSALYS_MODEL_STATIC is defined: this means Persalys is compiled as static lib. */
#define PERSALYS_MODEL_API
#define PERSALYS_MODEL_LOCAL
#endif /* !PERSALYS_MODEL_STATIC */

#ifndef PERSALYS_UTILS_STATIC /* defined if Persalys is compiled as a DLL */
#ifdef PERSALYS_UTILS_DLL_EXPORTS /* defined if we are building the Persalys DLLs (instead of using it) */
#define PERSALYS_UTILS_API PERSALYS_HELPER_DLL_EXPORT
#else
#define PERSALYS_UTILS_API PERSALYS_HELPER_DLL_IMPORT
#endif /* PERSALYS_UTILS_DLL_EXPORTS */
#define PERSALYS_UTILS_LOCAL PERSALYS_HELPER_DLL_LOCAL
#else /* PERSALYS_UTILS_STATIC is defined: this means Persalys is compiled as static lib. */
#define PERSALYS_UTILS_API
#define PERSALYS_UTILS_LOCAL
#endif /* !PERSALYS_UTILS_STATIC */

#ifndef PERSALYS_PLOT_STATIC /* defined if Persalys is compiled as a DLL */
#ifdef PERSALYS_PLOT_DLL_EXPORTS /* defined if we are building the Persalys DLLs (instead of using it) */
#define PERSALYS_PLOT_API PERSALYS_HELPER_DLL_EXPORT
#else
#define PERSALYS_PLOT_API PERSALYS_HELPER_DLL_IMPORT
#endif /* PERSALYS_PLOT_DLL_EXPORTS */
#define PERSALYS_PLOT_LOCAL PERSALYS_HELPER_DLL_LOCAL
#else /* PERSALYS_PLOT_STATIC is defined: this means Persalys is compiled as static lib. */
#define PERSALYS_PLOT_API
#define PERSALYS_PLOT_LOCAL
#endif /* !PERSALYS_PLOT_STATIC */

#ifndef PERSALYS_PLOTPV_STATIC /* defined if Persalys is compiled as a DLL */
#ifdef PERSALYS_PLOTPV_DLL_EXPORTS /* defined if we are building the Persalys DLLs (instead of using it) */
#define PERSALYS_PLOTPV_API PERSALYS_HELPER_DLL_EXPORT
#else
#define PERSALYS_PLOTPV_API PERSALYS_HELPER_DLL_IMPORT
#endif /* PERSALYS_PLOTPV_DLL_EXPORTS */
#define PERSALYS_PLOTPV_LOCAL PERSALYS_HELPER_DLL_LOCAL
#else /* PERSALYS_PLOTPV_STATIC is defined: this means Persalys is compiled as static lib. */
#define PERSALYS_PLOTPV_API
#define PERSALYS_PLOTPV_LOCAL
#endif /* !PERSALYS_PLOTPV_STATIC */

#ifndef PERSALYS_VIEW_STATIC /* defined if Persalys is compiled as a DLL */
#ifdef PERSALYS_VIEW_DLL_EXPORTS /* defined if we are building the Persalys DLLs (instead of using it) */
#define PERSALYS_VIEW_API PERSALYS_HELPER_DLL_EXPORT
#else
#define PERSALYS_VIEW_API PERSALYS_HELPER_DLL_IMPORT
#endif /* PERSALYS_VIEW_DLL_EXPORTS */
#define PERSALYS_VIEW_LOCAL PERSALYS_HELPER_DLL_LOCAL
#else /* PERSALYS_VIEW_STATIC is defined: this means Persalys is compiled as static lib. */
#define PERSALYS_VIEW_API
#define PERSALYS_VIEW_LOCAL
#endif /* !PERSALYS_VIEW_STATIC */

#endif // PERSALYS_PRIVATE_HXX

