
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

/* Now we use the generic helper definitions above to define PERSALYS_API and PERSALYS_LOCAL.
 * PERSALYS_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
 * PERSALYS_LOCAL is used for non-api symbols. */

#ifndef PERSALYS_STATIC /* defined if OT is compiled as a DLL */
#ifdef PERSALYS_DLL_EXPORTS /* defined if we are building the OT DLL (instead of using it) */
#define PERSALYS_API PERSALYS_HELPER_DLL_EXPORT
#else
#define PERSALYS_API PERSALYS_HELPER_DLL_IMPORT
#endif /* PERSALYS_DLL_EXPORTS */
#define PERSALYS_LOCAL PERSALYS_HELPER_DLL_LOCAL
#else /* PERSALYS_STATIC is defined: this means OT is a static lib. */
#define PERSALYS_API
#define PERSALYS_LOCAL
#endif /* !PERSALYS_STATIC */


#endif // PERSALYS_PRIVATE_HXX

